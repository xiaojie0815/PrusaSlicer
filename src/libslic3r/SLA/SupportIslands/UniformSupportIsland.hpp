#ifndef slic3r_SLA_SuppotstIslands_UniformSupportIsland_hpp_
#define slic3r_SLA_SuppotstIslands_UniformSupportIsland_hpp_

#include <libslic3r/ExPolygon.hpp>
#include "SampleConfig.hpp"
#include "SupportIslandPoint.hpp"

namespace Slic3r::sla {

/// <summary>
/// Distribute support points across island area defined by ExPolygon.
/// </summary>
/// <param name="island">Shape of island</param>
/// <param name="config">Configuration of support density</param>
/// <returns>Support points laying inside of island</returns>
SupportIslandPoints uniform_support_island(const ExPolygon &island, const SampleConfig &config);

/// <summary>
/// Check for tests that developer do not forget disable visualization after debuging.
/// </summary>
bool is_uniform_support_island_visualization_disabled();

} // namespace Slic3r::sla
#endif // slic3r_SLA_SuppotstIslands_UniformSupportIsland_hpp_
