///|/ Copyright (c) Prusa Research 2024 Filip Sykala @Jony01
///|/
///|/ PrusaSlicer is released under the terms of the AGPLv3 or higher
///|/

#include "SupportPointGeneratorNew.hpp"

#include <unordered_map> // point grid

#include "libslic3r/Execution/ExecutionTBB.hpp" // parallel preparation of data for sampling
#include "libslic3r/Execution/Execution.hpp"

using namespace Slic3r;
using namespace Slic3r::sla;

namespace {

/// <summary>
/// Struct to store support points in 2d grid to faster search for nearest support points
/// </summary>
class Grid2D
{
    coord_t m_cell_size; // Squar: x and y are same
    coord_t m_cell_size_half;
    using Key = Point;
    using Grid = std::unordered_multimap<Key, SupportPoint>;
    Grid m_grid;

public:
    /// <summary>
    /// Set cell size for grid
    /// </summary>
    /// <param name="cell_size">Granularity of stored points
    /// Must be bigger than maximal used radius</param>
    explicit Grid2D(const coord_t &cell_size)
        : m_cell_size(cell_size), m_cell_size_half(cell_size / 2) {}

    Key cell_id(const Point &point) const {
        return Key(point.x() / m_cell_size, point.y() / m_cell_size);
    }

    void add(SupportPoint &&point) {
        m_grid.emplace(cell_id(point.position_on_layer), std::move(point));
    }

    using CheckFnc = std::function<bool(const SupportPoint &, const Point&)>;
    bool exist_true_in_4cell_neighbor(const Point &pos, const CheckFnc& fnc) const {
        Key key = cell_id(pos);
        if (exist_true_for_cell(key, pos, fnc)) return true;
        Point un_cell_pos(
            key.x() * m_cell_size + m_cell_size_half, 
            key.y() * m_cell_size + m_cell_size_half );
        Key key2(
            (un_cell_pos.x() > pos.x()) ? key.x() + 1 : key.x() - 1,
            (un_cell_pos.y() > pos.y()) ? key.y() + 1 : key.y() - 1);
        if (exist_true_for_cell(key2, pos, fnc)) return true;
        if (exist_true_for_cell({key.x(), key2.y()}, pos, fnc)) return true;
        if (exist_true_for_cell({key2.x(), key.y()}, pos, fnc)) return true;
        return false;
    }

    void merge(Grid2D &&grid) {
        // support to merge only grid with same size
        assert(m_cell_size == grid.m_cell_size);
        m_grid.merge(std::move(grid.m_grid));
    }

    SupportPoints get_points() const { 
        SupportPoints result;
        result.reserve(m_grid.size());
        for (const auto& [key, support] : m_grid)
            result.push_back(support);
        return result;
    }
private:
    bool exist_true_for_cell(const Key &key, const Point &pos, const CheckFnc& fnc) const{
        auto [begin_it, end_it] = m_grid.equal_range(key);
        for (Grid::const_iterator it = begin_it; it != end_it; ++it) {
            const SupportPoint &support_point = it->second;
            if (fnc(support_point, pos))
                return true;
        }
        return false;
    }
};

/// <summary>
/// Intersection of line segment and circle
/// </summary>
/// <param name="p1">Line segment point A, Point lay inside circle</param>
/// <param name="p2">Line segment point B, Point lay outside or on circle</param>
/// <param name="cnt">Circle center point</param>
/// <param name="r2">squared value of Circle Radius (r2 = r*r)</param>
/// <returns>Intersection point</returns>
Point intersection(const Point &p1, const Point &p2, const Point &cnt, double r2) {
    // Vector from p1 to p2
    Vec2d dp_d((p2 - p1).cast<double>());
    // Vector from circle center to p1
    Vec2d f_d((p1 - cnt).cast<double>());

    double a = dp_d.squaredNorm();
    double b = 2 * (f_d.x() * dp_d.x() + f_d.y() * dp_d.y());
    double c = f_d.squaredNorm() - r2;

    // Discriminant of the quadratic equation
    double discriminant = b * b - 4 * a * c;

    // No intersection if discriminant is negative
    assert(discriminant > 0);
    if (discriminant < 0)
        return {}; // No intersection

    // Calculate the two possible values of t (parametric parameter)
    discriminant = sqrt(discriminant);
    double t1 = (-b - discriminant) / (2 * a);

    // Check for valid intersection points within the line segment
    if (t1 >= 0 && t1 <= 1) {
        return {p1.x() + t1 * dp_d.x(), p1.y() + t1 * dp_d.y()};
    }

    // should not be in use
    double t2 = (-b + discriminant) / (2 * a);
    if (t2 >= 0 && t2 <= 1 && t1 != t2) {
        return {p1.x() + t2 * dp_d.x(), p1.y() + t2 * dp_d.y()};
    }
    return {};
}

/// <summary>
/// Uniformly sample Polygon,
/// Use first point and each next point is first crosing radius from last added
/// </summary>
/// <param name="p">Polygon to sample</param>
/// <param name="dist2">Squared distance for sampling</param>
/// <returns>Uniformly distributed points laying on input polygon
/// with exception of first and last point(they are closer than dist2)</returns>
Slic3r::Points sample(const Polygon &p, double dist2) {
    if (p.empty())
        return {};

    Slic3r::Points r;
    r.push_back(p.front());
    const Point *prev_pt = nullptr;
    for (size_t prev_i = 0; prev_i < p.size(); prev_i++) {
        size_t curr_i = (prev_i != p.size() - 1) ? prev_i + 1 : 0;
        const Point &pt = p.points[curr_i];
        double p_dist2 = (r.back() - pt).cast<double>().squaredNorm();
        while (p_dist2 > dist2) { // line segment goes out of radius
            if (prev_pt == nullptr)
                prev_pt = &p.points[prev_i];
            r.push_back(intersection(*prev_pt, pt, r.back(), dist2));
            p_dist2 = (r.back() - pt).cast<double>().squaredNorm();
            prev_pt = &r.back();
        }
        prev_pt = nullptr;
    }
    return r;
}

coord_t get_supported_radius(const SupportPoint &p, float z_distance, const SupportPointGeneratorConfig &config
) {
    // TODO: calculate support radius
    return scale_(5.);
}

void sample_part(
    const LayerPart &part,
    size_t layer_id,
    const SupportPointGeneratorData &data,
    const SupportPointGeneratorConfig &config,
    std::vector<Grid2D> &grids,
    std::vector<Grid2D> &prev_grids
) {
    // NOTE: first layer do not have prev part
    assert(layer_id != 0);

    const Layers &layers = data.layers;
    const LayerParts &prev_layer_parts = layers[layer_id - 1].parts;
    const LayerParts::const_iterator &prev_part_it = part.prev_parts.front().part_it;
    size_t index_of_prev_part = prev_part_it - prev_layer_parts.begin();
    if (prev_part_it->next_parts.size() == 1) {
        grids.push_back(std::move(prev_grids[index_of_prev_part]));
    } else { // Need a copy there are multiple parts above previus one
        grids.push_back(prev_grids[index_of_prev_part]); // copy
    }
    // current part grid
    Grid2D &part_grid = grids.back();

    // merge other grid in case of multiple previous parts
    for (size_t i = 1; i < part.prev_parts.size(); ++i) {
        const LayerParts::const_iterator &prev_part_it = part.prev_parts[i].part_it;
        size_t index_of_prev_part = prev_part_it - prev_layer_parts.begin();
        if (prev_part_it->next_parts.size() == 1) {
            part_grid.merge(std::move(prev_grids[index_of_prev_part]));
        } else { // Need a copy there are multiple parts above previus one
            Grid2D grid_ = prev_grids[index_of_prev_part]; // copy
            part_grid.merge(std::move(grid_));
        }
    }

    float part_height = data.heights[layer_id];
    Grid2D::CheckFnc is_supported = [part_height, &config]
    (const SupportPoint &support_point, const Point &p) -> bool {
        float diff_height = part_height - support_point.z_height;
        coord_t r_ = get_supported_radius(support_point, diff_height, config);
        Point dp = support_point.position_on_layer - p;
        if (std::abs(dp.x()) > r_) return false;
        if (std::abs(dp.y()) > r_) return false;
        double r2 = static_cast<double>(r_);
        r2 *= r2;
        return dp.cast<double>().squaredNorm() < r2;
    };

    // check distance to nearest support points from grid
    float maximal_radius = scale_(5.f);
    for (const Point &p : part.samples) {
        if (!part_grid.exist_true_in_4cell_neighbor(p, is_supported)) {
            // not supported sample, soo create new support point
            part_grid.add(SupportPoint{
                /* head_front_radius */ 0.4,
                SupportPointType::slope,
                &part,
                /* position_on_layer */ p,
                part_height,
                /* direction_to_mass */ Point(1,0)
                });
        }    
    }
}

Points uniformly_sample(const ExPolygon &island, const SupportPointGeneratorConfig &cfg) {
    // TODO: Implement it
    return Points{island.contour.centroid()};
}

Grid2D support_island(const LayerPart &part, float part_z, const SupportPointGeneratorConfig &cfg) {
    // Maximal radius of supported area of one support point
    double max_support_radius = 10.; // cfg.cell_size;

    // maximal radius of support
    coord_t cell_size = scale_(max_support_radius);

    Grid2D part_grid(cell_size);
    Points pts = uniformly_sample(*part.shape, cfg);
    for (const Point &pt : pts)
        part_grid.add(SupportPoint{
            /* head_front_radius */ 0.4,
            SupportPointType::island,
            &part,
            /* position_on_layer */ pt,
            part_z,
            /* direction_to_mass */ Point(0,0) // from bottom
        });
}

}; 

SupportPointGeneratorData Slic3r::sla::prepare_generator_data(
    std::vector<ExPolygons> &&slices,
    std::vector<float> &&heights,
    ThrowOnCancel throw_on_cancel,
    StatusFunction statusfn
) {
    // check input
    assert(!slices.empty());
    assert(slices.size() == heights.size());
    if (slices.empty() || slices.size() != heights.size())
        return SupportPointGeneratorData{};

    // Move input into result
    SupportPointGeneratorData result;
    result.slices = std::move(slices);
    result.heights = std::move(heights);

    // Allocate empty layers.
    result.layers = Layers(result.slices.size(), {});

    // Generate Extents and SampleLayers
    execution::for_each(ex_tbb, size_t(0), result.slices.size(),
        [&result, throw_on_cancel](size_t layer_id) {
        if ((layer_id % 8) == 0)
            // Don't call the following function too often as it flushes
            // CPU write caches due to synchronization primitves.
            throw_on_cancel();

        const double sample_distance_in_mm = scale_(2);
        const double sample_distance_in_mm2 = sample_distance_in_mm * sample_distance_in_mm;

        Layer &layer = result.layers[layer_id];
        const ExPolygons &islands = result.slices[layer_id];
        layer.parts.reserve(islands.size());
        for (const ExPolygon &island : islands)                        
            layer.parts.push_back(LayerPart{
                &island, 
                get_extents(island.contour), 
                sample(island.contour, sample_distance_in_mm2)
            });
        
    }, 32 /*gransize*/);

    // Link parts by intersections
    execution::for_each(ex_tbb, size_t(1), result.slices.size(),
                      [&result, throw_on_cancel] (size_t layer_id) {
        if ((layer_id % 2) == 0)
            // Don't call the following function too often as it flushes CPU write caches due to synchronization primitves.
            throw_on_cancel();

        LayerParts &parts_above = result.layers[layer_id].parts;
        LayerParts &parts_below = result.layers[layer_id-1].parts;
        for (auto it_above = parts_above.begin(); it_above < parts_above.end(); ++it_above) {
            for (auto it_below = parts_below.begin(); it_below < parts_below.end(); ++it_below) {
                // Improve: do some sort of parts + skip some of them
                if (!it_above->shape_extent.overlap(it_below->shape_extent))
                    continue; // no bounding box overlap

                // Improve: test could be done faster way
                Polygons polys = intersection(*it_above->shape, *it_below->shape);
                if (polys.empty())
                    continue; // no intersection

                // TODO: check minimal intersection!

                it_above->prev_parts.emplace_back(PartLink{it_below});
                it_below->next_parts.emplace_back(PartLink{it_above});
            }
        }
    }, 8 /* gransize */);
    return result;
}

SupportPoints Slic3r::sla::generate_support_points(
    const SupportPointGeneratorData &data,
    const SupportPointGeneratorConfig &config,
    ThrowOnCancel throw_on_cancel,
    StatusFunction statusfn
){
    const Layers &layers = data.layers;
    double increment = 100.0 / static_cast<double>(layers.size());
    double status = 0; // current progress
    int status_int = 0; 

    SupportPoints result;
    std::vector<Grid2D> prev_grids; // same count as previous layer item size
    for (size_t layer_id = 0; layer_id < layers.size(); ++layer_id) {
        const Layer &layer = layers[layer_id];

        std::vector<Grid2D> grids;
        grids.reserve(layer.parts.size());
        
        for (const LayerPart &part : layer.parts) {
            if (part.prev_parts.empty()) {
                // new island - needs support no doubt
                float part_z = data.heights[layer_id];
                grids.push_back(support_island(part, part_z, config));
            } else {
                sample_part(part, layer_id, data, config, grids, prev_grids);
            }

            // collect result from grid of top part
            if (part.next_parts.empty()) {
                const Grid2D &part_grid = grids.back();
                SupportPoints sps = part_grid.get_points();
                result.insert(result.end(), 
                    std::make_move_iterator(sps.begin()),
                    std::make_move_iterator(sps.end()));
            }
        }
        prev_grids = std::move(grids);

        throw_on_cancel();

        int old_status_int = status_int;
        status += increment;
        status_int = static_cast<int>(std::round(status));
        if (old_status_int < status_int)
            statusfn(status_int);
    }
    return result;
}


