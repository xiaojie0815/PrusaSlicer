///|/ Copyright (c) Prusa Research 2024 Filip Sykala @Jony01
///|/
///|/ PrusaSlicer is released under the terms of the AGPLv3 or higher
///|/
#ifndef SLA_SUPPORTPOINTGENERATOR_HPP
#define SLA_SUPPORTPOINTGENERATOR_HPP

#include <vector>
#include <functional>

#include <boost/container/small_vector.hpp>

#include "libslic3r/Point.hpp"
#include "libslic3r/ExPolygon.hpp"
#include "libslic3r/SLA/SupportPoint.hpp"
#include "libslic3r/SLA/SupportIslands/SampleConfig.hpp"
#include "libslic3r/SLA/SupportIslands/SampleConfigFactory.hpp"

namespace Slic3r::sla {

/// <summary>
/// Configuration for automatic support placement
/// </summary>
struct SupportPointGeneratorConfig{
    /// <summary>
    /// 0 mean only one support point for each island
    /// lower than one mean less amount of support points
    /// 1 mean fine tuned sampling
    /// more than one mean bigger amout of support points
    /// </summary>
    float density_relative{1.f};

    /// <summary>
    /// Size range for support point interface (head)
    /// </summary>
    float head_diameter = 0.4f; // [in mm]

    // FIXME: calculate actual pixel area from printer config:
    // const float pixel_area =
    // pow(wxGetApp().preset_bundle->project_config.option<ConfigOptionFloat>("display_width") /
    // wxGetApp().preset_bundle->project_config.option<ConfigOptionInt>("display_pixels_x"), 2.f); //
    // Minimal island Area to print - TODO: Should be modifiable from UI
    // !! Filter should be out of sampling algorithm !!
    float minimal_island_area = pow(0.047f, 2.f); // [in mm^2] pixel_area

    // maximal distance to nearest support point(define radiuses per layer)
    // x axis .. mean distance on layer(XY)
    // y axis .. mean difference of height(Z)
    // Points of lines [in mm]
    std::vector<Vec2f> support_curve;

    // Configuration for sampling island
    SampleConfig island_configuration = SampleConfigFactory::create(head_diameter);

    // To be able support same 2d area multipletimes,
    // It is neccessary to remove support point form near KDTree structure
    
    // Must be greater than surface texture and lower than self supporting area
    // May be use maximal island distance
    float removing_delta = scale_(5.);
};

struct LayerPart; // forward decl.
using LayerParts = std::vector<LayerPart>;

struct PartLink
{
    LayerParts::const_iterator part_it;
    // float overlap_area; // sum of overlap areas
    // ExPolygons overlap; // clipper intersection_ex
    // ExPolygons overhang; // clipper diff_ex
};
#ifdef NDEBUG
// In release mode, use the optimized container.
using PartLinks = boost::container::small_vector<PartLink, 4>;
#else
// In debug mode, use the standard vector, which is well handled by debugger visualizer.
using PartLinks = std::vector<PartLink>;
#endif

// Part on layer is defined by its shape 
struct LayerPart {
    // Pointer to expolygon stored in input
    const ExPolygon *shape;

    // rectangular bounding box of shape
    BoundingBox shape_extent;

    // uniformly sampled shape contour
    Slic3r::Points samples;
    // IMPROVE: sample only overhangs part of shape

    // Parts from previous printed layer, which is connected to current part
    PartLinks prev_parts;
    PartLinks next_parts;
};

/// <summary>
/// Extend support point with information from layer
/// </summary>
struct LayerSupportPoint: public SupportPoint
{
    // Pointer on source ExPolygon otherwise nullptr
    //const LayerPart *part{nullptr}; 

    // 2d coordinate on layer
    // use only when part is not nullptr
    Point position_on_layer; // [scaled_ unit]

    // 2d direction into expolygon mass
    // used as ray to positioning 3d point on mesh surface
    // Island has direction [0,0] - should be placed on surface from bottom
    Point direction_to_mass;

    // index into curve to faster found radius for current layer
    size_t radius_curve_index = 0;
    coord_t current_radius = 0; // [in scaled mm]

    // information whether support point is active in current investigated layer
    // Flagged false when no part on layer in Radius 'r' around support point
    // Tool to support overlapped overhang area multiple times
    bool active_in_part = true;
};
using LayerSupportPoints = std::vector<LayerSupportPoint>;

/// <summary>
/// One slice divided into 
/// </summary>
struct Layer
{
    // Absolute distance from Zero - copy value from heights<float>
    float print_z; // [in mm]

    // data for one expolygon
    LayerParts parts;
};
using Layers = std::vector<Layer>;

/// <summary>
/// Keep state of Support Point generation
/// Used for resampling with different configuration
/// </summary>
struct SupportPointGeneratorData
{
    // Input slices of mesh
    std::vector<ExPolygons> slices;

    // Keep information about layer and its height
    // and connection between layers for its part
    // NOTE: contain links into slices
    Layers layers;
};

// call during generation of support points to check cancel event
using ThrowOnCancel = std::function<void(void)>;
// call to say progress of generation into gui in range from 0 to 100
using StatusFunction= std::function<void(int)>;

/// <summary>
/// Prepare data for generate support points
/// Used for interactive resampling to store permanent data between configuration changes.,
/// Everything which could be prepared are stored into result.
/// Need to regenerate on mesh change(Should be connected with ObjectId) OR change of slicing heights
/// </summary>
/// <param name="slices">Countour cut from mesh</param>
/// <param name="heights">Heights of the slices - Same size as slices</param>
/// <param name="throw_on_cancel">Call in meanwhile to check cancel event</param>
/// <param name="statusfn">Say progress of generation into gui</param>
/// <returns>Data prepared for generate support points</returns>
SupportPointGeneratorData prepare_generator_data(
    std::vector<ExPolygons> &&slices,
    const std::vector<float> &heights,
    ThrowOnCancel throw_on_cancel,
    StatusFunction statusfn
);

/// <summary>
/// Generate support points on islands by configuration parameters
/// </summary>
/// <param name="data">Preprocessed data needed for sampling</param>
/// <param name="config">Define density of samples</param>
/// <param name="throw_on_cancel">Call in meanwhile to check cancel event</param>
/// <param name="statusfn">Progress of generation into gui</param>
/// <returns>Generated support points</returns>
LayerSupportPoints generate_support_points(
    const SupportPointGeneratorData &data,
    const SupportPointGeneratorConfig &config,
    ThrowOnCancel throw_on_cancel,
    StatusFunction statusfn
);
} // namespace Slic3r::sla

// TODO: Not sure if it is neccessary & Should be in another file
namespace Slic3r{
class AABBMesh;
namespace sla {
/// <summary>
/// Move support points on surface of mesh
/// </summary>
/// <param name="points">Support points to move on surface</param>
/// <param name="mesh">Define surface for move points</param>
/// <param name="throw_on_cancel">Call in meanwhile to check cancel event</param>
/// <returns>Support points laying on mesh surface</returns>
SupportPoints move_on_mesh_surface(
    const LayerSupportPoints &points,
    const AABBMesh &mesh,
    double allowed_move,
    ThrowOnCancel throw_on_cancel
);

}}

#endif // SLA_SUPPORTPOINTGENERATOR_HPP
