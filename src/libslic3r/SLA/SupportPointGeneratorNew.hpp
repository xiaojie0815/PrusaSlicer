///|/ Copyright (c) Prusa Research 2024 Filip Sykala @Jony01
///|/
///|/ PrusaSlicer is released under the terms of the AGPLv3 or higher
///|/
#ifndef SLA_SUPPORTPOINTGENERATOR_NEW_HPP
#define SLA_SUPPORTPOINTGENERATOR_NEW_HPP

#include <vector>
#include <functional>

#include <boost/container/small_vector.hpp>

#include "libslic3r/Point.hpp"
#include "libslic3r/ExPolygon.hpp"

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
    MinMax<float> head_diameter = {0.2f, 0.6f}; // [in mm]

    // FIXME: calculate actual pixel area from printer config:
    // const float pixel_area =
    // pow(wxGetApp().preset_bundle->project_config.option<ConfigOptionFloat>("display_width") /
    // wxGetApp().preset_bundle->project_config.option<ConfigOptionInt>("display_pixels_x"), 2.f); //
    // Minimal island Area to print - TODO: Should be modifiable from UI
    // !! Filter should be out of sampling algorithm !!
    float minimal_island_area = pow(0.047f, 2.f); // [in mm^2] pixel_area
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
/// One slice divided into 
/// </summary>
struct Layer
{
    // index into parent Layesr + heights + slices 
    // [[deprecated]] Use index to layers insted of adress from item
    size_t layer_id;

    // Absolute distance from Zero - copy value from heights<float>
    // [[deprecated]] Use index to layers insted of adress from item
    double print_z; // [in mm]

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
    // Same size as slices
    std::vector<float> heights;

    // link to slices
    Layers layers;
};

// Reason of automatic support placement usage
enum class SupportPointType {
    manual_add,
    island, // no move
    slope,
    thin,
    stability,
    edge
};

/// <summary>
/// Generated support point
/// </summary>
struct SupportPoint
{
    // radius of the touching interface
    // Also define force it must keep
    float head_front_radius{1.f};

    // type
    SupportPointType type{SupportPointType::manual_add};

    // Pointer on source ExPolygon otherwise nullptr
    const LayerPart *part{nullptr}; 

    // 2d coordinate on layer
    // use only when part is not nullptr
    Point position_on_layer; // [scaled_ unit]

    // height of part
    float z_height;

    // 2d direction into expolygon mass
    // used as ray to positioning point on mesh surface
    Point direction_to_mass;
};
using SupportPoints = std::vector<SupportPoint>;

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
    std::vector<float> &&heights,
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
SupportPoints generate_support_points(
    const SupportPointGeneratorData &data,
    const SupportPointGeneratorConfig &config,
    ThrowOnCancel throw_on_cancel,
    StatusFunction statusfn
);

} // namespace Slic3r::sla

#endif // SLA_SUPPORTPOINTGENERATOR_NEW_HPP
