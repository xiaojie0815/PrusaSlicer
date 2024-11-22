#ifndef slic3r_SLA_SuppotstIslands_SampleIslandUtils_hpp_
#define slic3r_SLA_SuppotstIslands_SampleIslandUtils_hpp_

#include <libslic3r/ExPolygon.hpp>
#include "SampleConfig.hpp"
#include "SupportIslandPoint.hpp"

namespace Slic3r::sla {

/// <summary>
/// Utils class with only static function
/// Function for sampling island by Voronoi Graph.
/// </summary>
class SampleIslandUtils
{
public:
    SampleIslandUtils() = delete;

    /// <summary>
    /// Main entry for sample island
    /// </summary>
    /// <param name="island">Shape of island</param>
    /// <param name="config">Configuration for sampler</param>
    /// <returns>List of support points</returns>
    static SupportIslandPoints uniform_cover_island(
        const ExPolygon &island, const SampleConfig &config);

    
    static bool is_uniform_cover_island_visualization_disabled();
};

} // namespace Slic3r::sla
#endif // slic3r_SLA_SuppotstIslands_SampleIslandUtils_hpp_
