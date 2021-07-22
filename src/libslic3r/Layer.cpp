#include "Layer.hpp"
#include "ClipperUtils.hpp"
#include "Print.hpp"
#include "Fill/Fill.hpp"
#include "ShortestPath.hpp"
#include "SVG.hpp"
#include "BoundingBox.hpp"

#include <boost/log/trivial.hpp>

namespace Slic3r {

Layer::~Layer()
{
    this->lower_layer = this->upper_layer = nullptr;
    for (LayerRegion *region : m_regions)
        delete region;
    m_regions.clear();
}

// Test whether whether there are any slices assigned to this layer.
bool Layer::empty() const
{
	for (const LayerRegion *layerm : m_regions)
        if (layerm != nullptr && ! layerm->slices.empty())
            // Non empty layer.
            return false;
    return true;
}

LayerRegion* Layer::add_region(const PrintRegion *print_region)
{
    m_regions.emplace_back(new LayerRegion(this, print_region));
    return m_regions.back();
}

// merge all regions' slices to get islands
void Layer::make_slices()
{
    ExPolygons slices;
    if (m_regions.size() == 1) {
        // optimization: if we only have one region, take its slices
        slices = to_expolygons(m_regions.front()->slices.surfaces);
    } else {
        Polygons slices_p;
        for (LayerRegion *layerm : m_regions)
            polygons_append(slices_p, to_polygons(layerm->slices.surfaces));
        slices = union_safety_offset_ex(slices_p);
    }
    
    this->lslices.clear();
    this->lslices.reserve(slices.size());
    
    // prepare ordering points
    Points ordering_points;
    ordering_points.reserve(slices.size());
    for (const ExPolygon &ex : slices)
        ordering_points.push_back(ex.contour.first_point());
    
    // sort slices
    std::vector<Points::size_type> order = chain_points(ordering_points);
    
    // populate slices vector
    for (size_t i : order)
        this->lslices.emplace_back(std::move(slices[i]));
}

static inline bool layer_needs_raw_backup(const Layer *layer)
{
    return ! (layer->regions().size() == 1 && (layer->id() > 0 || layer->object()->config().elefant_foot_compensation.value == 0));
}

void Layer::backup_untyped_slices()
{
    if (layer_needs_raw_backup(this)) {
        for (LayerRegion *layerm : m_regions)
            layerm->raw_slices = to_expolygons(layerm->slices.surfaces);
    } else {
        assert(m_regions.size() == 1);
        m_regions.front()->raw_slices.clear();
    }
}

void Layer::restore_untyped_slices()
{
    if (layer_needs_raw_backup(this)) {
        for (LayerRegion *layerm : m_regions)
            layerm->slices.set(layerm->raw_slices, stInternal);
    } else {
        assert(m_regions.size() == 1);
        m_regions.front()->slices.set(this->lslices, stInternal);
    }
}

// Similar to Layer::restore_untyped_slices()
// To improve robustness of detect_surfaces_type() when reslicing (working with typed slices), see GH issue #7442.
// Only resetting layerm->slices if Slice::extra_perimeters is always zero or it will not be used anymore
// after the perimeter generator.
void Layer::restore_untyped_slices_no_extra_perimeters()
{
    if (layer_needs_raw_backup(this)) {
        for (LayerRegion *layerm : m_regions)
        	if (! layerm->region().config().extra_perimeters.value)
            	layerm->slices.set(layerm->raw_slices, stInternal);
    } else {
    	assert(m_regions.size() == 1);
    	LayerRegion *layerm = m_regions.front();
    	// This optimization is correct, as extra_perimeters are only reused by prepare_infill() with multi-regions.
        //if (! layerm->region().config().extra_perimeters.value)
        	layerm->slices.set(this->lslices, stInternal);
    }
}

ExPolygons Layer::merged(float offset_scaled) const
{
	assert(offset_scaled >= 0.f);
    // If no offset is set, apply EPSILON offset before union, and revert it afterwards.
	float offset_scaled2 = 0;
	if (offset_scaled == 0.f) {
		offset_scaled  = float(  EPSILON);
		offset_scaled2 = float(- EPSILON);
    }
    Polygons polygons;
	for (LayerRegion *layerm : m_regions) {
		const PrintRegionConfig &config = layerm->region().config();
		// Our users learned to bend Slic3r to produce empty volumes to act as subtracters. Only add the region if it is non-empty.
		if (config.bottom_solid_layers > 0 || config.top_solid_layers > 0 || config.fill_density > 0. || config.perimeters > 0)
			append(polygons, offset(layerm->slices.surfaces, offset_scaled));
	}
    ExPolygons out = union_ex(polygons);
	if (offset_scaled2 != 0.f)
		out = offset_ex(out, offset_scaled2);
    return out;
}

// Here the perimeters are created cummulatively for all layer regions sharing the same parameters influencing the perimeters.
// The perimeter paths and the thin fills (ExtrusionEntityCollection) are assigned to the first compatible layer region.
// The resulting fill surface is split back among the originating regions.
void Layer::make_perimeters()
{
    BOOST_LOG_TRIVIAL(trace) << "Generating perimeters for layer " << this->id();
    
    // keep track of regions whose perimeters we have already generated
    std::vector<unsigned char> done(m_regions.size(), false);
    
    for (LayerRegionPtrs::iterator layerm = m_regions.begin(); layerm != m_regions.end(); ++ layerm) 
    	if ((*layerm)->slices.empty()) {
 			(*layerm)->perimeters.clear();
 			(*layerm)->fills.clear();
 			(*layerm)->thin_fills.clear();
    	} else {
	        size_t region_id = layerm - m_regions.begin();
	        if (done[region_id])
	            continue;
	        BOOST_LOG_TRIVIAL(trace) << "Generating perimeters for layer " << this->id() << ", region " << region_id;
	        done[region_id] = true;
	        const PrintRegionConfig &config = (*layerm)->region().config();
	        
	        // find compatible regions
	        LayerRegionPtrs layerms;
	        layerms.push_back(*layerm);
	        for (LayerRegionPtrs::const_iterator it = layerm + 1; it != m_regions.end(); ++it)
	            if (! (*it)->slices.empty()) {
		            LayerRegion* other_layerm = *it;
		            const PrintRegionConfig &other_config = other_layerm->region().config();
		            if (config.perimeter_extruder             == other_config.perimeter_extruder
		                && config.perimeters                  == other_config.perimeters
		                && config.perimeter_speed             == other_config.perimeter_speed
		                && config.external_perimeter_speed    == other_config.external_perimeter_speed
		                && (config.gap_fill_enabled ? config.gap_fill_speed.value : 0.) == 
                           (other_config.gap_fill_enabled ? other_config.gap_fill_speed.value : 0.)
		                && config.overhangs                   == other_config.overhangs
		                && config.opt_serialize("perimeter_extrusion_width") == other_config.opt_serialize("perimeter_extrusion_width")
		                && config.thin_walls                  == other_config.thin_walls
		                && config.external_perimeters_first   == other_config.external_perimeters_first
		                && config.infill_overlap              == other_config.infill_overlap
                        && config.fuzzy_skin                  == other_config.fuzzy_skin
                        && config.fuzzy_skin_thickness        == other_config.fuzzy_skin_thickness
                        && config.fuzzy_skin_point_dist       == other_config.fuzzy_skin_point_dist)
		            {
			 			other_layerm->perimeters.clear();
			 			other_layerm->fills.clear();
			 			other_layerm->thin_fills.clear();
		                layerms.push_back(other_layerm);
		                done[it - m_regions.begin()] = true;
		            }
		        }
	        
	        if (layerms.size() == 1) {  // optimization
	            (*layerm)->fill_surfaces.surfaces.clear();
	            (*layerm)->make_perimeters((*layerm)->slices, &(*layerm)->fill_surfaces);
	            (*layerm)->fill_expolygons = to_expolygons((*layerm)->fill_surfaces.surfaces);
	        } else {
	            SurfaceCollection new_slices;
	            // Use the region with highest infill rate, as the make_perimeters() function below decides on the gap fill based on the infill existence.
	            LayerRegion *layerm_config = layerms.front();
	            {
	                // group slices (surfaces) according to number of extra perimeters
	                std::map<unsigned short, Surfaces> slices;  // extra_perimeters => [ surface, surface... ]
	                for (LayerRegion *layerm : layerms) {
	                    for (const Surface &surface : layerm->slices.surfaces)
	                        slices[surface.extra_perimeters].emplace_back(surface);
	                    if (layerm->region().config().fill_density > layerm_config->region().config().fill_density)
	                    	layerm_config = layerm;
	                }
	                // merge the surfaces assigned to each group
	                for (std::pair<const unsigned short,Surfaces> &surfaces_with_extra_perimeters : slices)
	                    new_slices.append(offset_ex(surfaces_with_extra_perimeters.second, ClipperSafetyOffset), surfaces_with_extra_perimeters.second.front());
	            }
	            
	            // make perimeters
	            SurfaceCollection fill_surfaces;
	            layerm_config->make_perimeters(new_slices, &fill_surfaces);

	            // assign fill_surfaces to each layer
	            if (!fill_surfaces.surfaces.empty()) { 
	                for (LayerRegionPtrs::iterator l = layerms.begin(); l != layerms.end(); ++l) {
	                    // Separate the fill surfaces.
	                    ExPolygons expp = intersection_ex(fill_surfaces.surfaces, (*l)->slices.surfaces);
	                    (*l)->fill_expolygons = expp;
	                    (*l)->fill_surfaces.set(std::move(expp), fill_surfaces.surfaces.front());
	                }
	            }
	        }
	    }
    BOOST_LOG_TRIVIAL(trace) << "Generating perimeters for layer " << this->id() << " - Done";
}

void Layer::export_region_slices_to_svg(const char *path) const
{
    BoundingBox bbox;
    for (const auto *region : m_regions)
        for (const auto &surface : region->slices.surfaces)
            bbox.merge(get_extents(surface.expolygon));
    Point legend_size = export_surface_type_legend_to_svg_box_size();
    Point legend_pos(bbox.min(0), bbox.max(1));
    bbox.merge(Point(std::max(bbox.min(0) + legend_size(0), bbox.max(0)), bbox.max(1) + legend_size(1)));

    SVG svg(path, bbox);
    const float transparency = 0.5f;
    for (const auto *region : m_regions)
        for (const auto &surface : region->slices.surfaces)
            svg.draw(surface.expolygon, surface_type_to_color_name(surface.surface_type), transparency);
    export_surface_type_legend_to_svg(svg, legend_pos);
    svg.Close(); 
}

// Export to "out/LayerRegion-name-%d.svg" with an increasing index with every export.
void Layer::export_region_slices_to_svg_debug(const char *name) const
{
    static size_t idx = 0;
    this->export_region_slices_to_svg(debug_out_path("Layer-slices-%s-%d.svg", name, idx ++).c_str());
}

void Layer::export_region_fill_surfaces_to_svg(const char *path) const
{
    BoundingBox bbox;
    for (const auto *region : m_regions)
        for (const auto &surface : region->slices.surfaces)
            bbox.merge(get_extents(surface.expolygon));
    Point legend_size = export_surface_type_legend_to_svg_box_size();
    Point legend_pos(bbox.min(0), bbox.max(1));
    bbox.merge(Point(std::max(bbox.min(0) + legend_size(0), bbox.max(0)), bbox.max(1) + legend_size(1)));

    SVG svg(path, bbox);
    const float transparency = 0.5f;
    for (const auto *region : m_regions)
        for (const auto &surface : region->slices.surfaces)
            svg.draw(surface.expolygon, surface_type_to_color_name(surface.surface_type), transparency);
    export_surface_type_legend_to_svg(svg, legend_pos);
    svg.Close();
}

// Export to "out/LayerRegion-name-%d.svg" with an increasing index with every export.
void Layer::export_region_fill_surfaces_to_svg_debug(const char *name) const
{
    static size_t idx = 0;
    this->export_region_fill_surfaces_to_svg(debug_out_path("Layer-fill_surfaces-%s-%d.svg", name, idx ++).c_str());
}



BoundingBox get_extents(const LayerRegion &layer_region)
{
    BoundingBox bbox;
    if (!layer_region.slices.surfaces.empty()) {
        bbox = get_extents(layer_region.slices.surfaces.front());
        for (auto it = layer_region.slices.surfaces.cbegin() + 1; it != layer_region.slices.surfaces.cend(); ++it)
            bbox.merge(get_extents(*it));
    }
    return bbox;
}

BoundingBox get_extents(const LayerRegionPtrs &layer_regions)
{
    BoundingBox bbox;
    if (!layer_regions.empty()) {
        bbox = get_extents(*layer_regions.front());
        for (auto it = layer_regions.begin() + 1; it != layer_regions.end(); ++it)
            bbox.merge(get_extents(**it));
    }
    return bbox;
}



void Layer::extend_bridging_infill()
{
    double angle = -Geometry::deg2rad(45.);

    for (LayerRegion* region : m_regions) {

        // is there some bridging infill?
        if (std::none_of(region->fills.entities.begin(),
                         region->fills.entities.end(),
                         [](const ExtrusionEntity* ee) { return ee->role()==erBridgeInfill; }))
            continue;

        // collect all infill (except yet unmodified bridge infill)
        Polylines extrusions;
        Polylines temp;
        for (const ExtrusionEntityCollection& ee_collection: { region->fills, region->perimeters}) {
            for (const ExtrusionEntity* ee : ee_collection.entities) {
                assert(ee->is_collection());
                if (ee->role() != erBridgeInfill) {
                    temp = dynamic_cast<const ExtrusionEntityCollection*>(ee)->as_polylines();
                    for (Polyline& p : temp)
                        p.rotate(angle);
                    extrusions.insert(extrusions.end(), temp.begin(), temp.end());
                }
            }
        }


        // Now go through the bridging infill patches.
        for (ExtrusionEntity* ee : region->fills.entities) {
            if (ee->role() != erBridgeInfill)
                continue;

            // Make a rotated copy.
            auto eec = dynamic_cast<ExtrusionEntityCollection*>(ee);
            Polylines bridges = eec->as_polylines();
            for (Polyline& p : bridges)
                p.rotate(angle);

            // Bridging infill lines should now be vertical with equidistant spacing.
            struct VertLine {
                coord_t x;
                coord_t y_top;
                coord_t y_bottom;
                std::vector<coord_t> ys;
            };

            std::vector<VertLine> lines;
            for (const Polyline& bridge : bridges) {
                const Points& pts = bridge.points;
                for (size_t i=0; i < pts.size(); i+=1) {
                    lines.push_back(VertLine{pts[i].x(), pts[i].y(), pts[i+1].y(), std::vector<coord_t>()});
                    if (lines.back().y_top < lines.back().y_bottom)
                        std::swap(lines.back().y_top, lines.back().y_bottom);
                    // Filter out very short segments and lines that are not vertical.
                    if (lines.back().y_top - lines.back().y_bottom < 500000
                     || std::abs((pts[i+1].x()-pts[i].x())/(lines.back().y_top-lines.back().y_bottom)) > 0.0001)
                        lines.pop_back();
                }
            }
            if (lines.empty())
                continue;
            std::sort(lines.begin(), lines.end(), [](const VertLine& a, const VertLine& b) { return a.x < b.x; });
            double spacing = lines.size() > 1 ? double(lines.back().x - lines.front().x) / (lines.size() - 1) : 0.;

            // Now go through infill polylines, pick those that will intersect
            // the bridging lines (extended to infinity), calculate intersections
            // with all of them and save the intersection with the respective line.
            for (const Polyline& p : extrusions) {
                for (size_t i=1; i<p.points.size(); ++i) {
                    Point start = p.points[i-1];
                    Point end = p.points[i];
                    if (start.x() > end.x())
                        std::swap(start, end);
                    if (start.x() == end.x())
                        continue; // vertical lines will not intersect

                    // Which of the lines this segment intersects?
                    int idx_start = 0;
                    int idx_end   = 0;
                    if (spacing != 0.) {
                        idx_start = std::floor((start.x() - lines.front().x) / spacing) + 1;
                        idx_end = std::floor((end.x() - lines.front().x) / spacing);
                    }
                    else {
                        if (! (start.x() <= lines.front().x && end.x() >= lines.front().x))
                            continue; // there is just one line
                    }

                    if (idx_start > idx_end)
                        continue;

                    if (idx_end >= int(lines.size()))
                        idx_end = lines.size() - 1;
                    double slope = std::nan("");
                    double increment = 0;
                    while (idx_end >= idx_start && idx_end >= 0) {
                        assert(lines[idx_end].x >= start.x() && lines[idx_end].x <= end.x());
                        // Find and save an intersection with lines[idx_end].
                        if (std::isnan(slope)) { // first run
                            slope = (end.y()-start.y())/(end.x()-start.x());
                            lines[idx_end].ys.push_back(start.y() + slope * (lines[idx_end].x - start.x()));
                            increment = slope * spacing;
                        } else
                            lines[idx_end].ys.push_back(lines[idx_end+1].ys.back() - increment);
                        --idx_end;
                    }
                }
            }


            // Each of the vertical lines should now have candidates
            // for extension. Extend them.
            for (VertLine& line : lines) {
                coord_t top_cand = std::numeric_limits<coord_t>::max();
                coord_t bot_cand = std::numeric_limits<coord_t>::min();
                for (coord_t a : line.ys) {
                    if (a > line.y_top && a < top_cand)
                        top_cand = a;
                    if (a < line.y_bottom && a > bot_cand)
                        bot_cand = a;
                }
                if (top_cand != std::numeric_limits<coord_t>::max())
                    line.y_top = top_cand;
                if (bot_cand != std::numeric_limits<coord_t>::min())
                    line.y_bottom = bot_cand;

            }


            {
                eec->clear();
                Polyline polyline;
                for (VertLine& line : lines) {
                    Point a(line.x, line.y_bottom);
                    Point b(line.x, line.y_top);
                    if ((polyline.size()/2) % 2)
                        std::swap(a, b);
                    polyline.append(a);
                    polyline.append(b);
                }
                polyline.rotate(-angle);
                Polylines out;
                out.emplace_back(std::move(polyline));
                extrusion_entities_append_paths(eec->entities, out, erBridgeInfill, 0.3, 0.3, 0.15);
            }
        }






    }
}


} // namespace Slic3r
