#include "SampleConfigFactory.hpp"

using namespace Slic3r::sla;

bool SampleConfigFactory::verify(SampleConfig &cfg) {
    auto verify_max = [](coord_t &c, coord_t max) {
        assert(c <= max);
        if (c > max) {
            c = max;
            return false;
        }
        return true;
    };
    auto verify_min = [](coord_t &c, coord_t min) {
        assert(c >= min);
        if (c < min) {
            c = min;
            return false;
        }
        return true;
    };
    auto verify_min_max = [](coord_t &min, coord_t &max) {
        // min must be smaller than max
        assert(min < max);
        if (min > max) {
            std::swap(min, max);
            return false;
        } else if (min == max) {
            min /= 2; // cut in half
            return false;
        }
        return true;
    };
    bool res = true;
    res &= verify_min_max(cfg.max_length_for_one_support_point, cfg.max_length_for_two_support_points);        
    res &= verify_min_max(cfg.min_width_for_outline_support, cfg.max_width_for_center_support_line); // check histeresis
    res &= verify_max(cfg.max_length_for_one_support_point,
        2 * cfg.max_distance +
        2 * cfg.head_radius +
        2 * cfg.minimal_distance_from_outline);
    res &= verify_min(cfg.max_length_for_one_support_point,
        2 * cfg.head_radius + 2 * cfg.minimal_distance_from_outline);
    res &= verify_max(cfg.max_length_for_two_support_points,
        2 * cfg.max_distance + 
        2 * 2 * cfg.head_radius +
        2 * cfg.minimal_distance_from_outline);
    res &= verify_min(cfg.max_width_for_center_support_line, 
        2 * cfg.head_radius + 2 * cfg.minimal_distance_from_outline);
    res &= verify_max(cfg.max_width_for_center_support_line,
        2 * cfg.max_distance + 2 * cfg.head_radius);
    if (!res) while (!verify(cfg));
    return res;
}

SampleConfig SampleConfigFactory::create(float support_head_diameter_in_mm)
{
    coord_t head_diameter = static_cast<coord_t>(scale_(support_head_diameter_in_mm));
    coord_t minimal_distance = head_diameter * 7;
    coord_t min_distance = head_diameter / 2 + minimal_distance;
    coord_t max_distance = 3 * min_distance;
        
    // TODO: find valid params !!!!
    SampleConfig result;
    result.max_distance                  = max_distance;
    result.head_radius                   = head_diameter / 2;
    result.minimal_distance_from_outline = result.head_radius;
    result.maximal_distance_from_outline = result.max_distance/3;
    assert(result.minimal_distance_from_outline < result.maximal_distance_from_outline);
    result.max_length_for_one_support_point =
        max_distance / 3 +
        2 * result.minimal_distance_from_outline + 
        head_diameter;
    result.max_length_for_two_support_points =
        result.max_length_for_one_support_point + max_distance / 2;
    result.max_width_for_center_support_line =
        2 * head_diameter + 2 * result.minimal_distance_from_outline +
        max_distance / 2;
    result.min_width_for_outline_support = result.max_width_for_center_support_line - 2 * head_diameter;
    result.outline_sample_distance = 3 * result.max_distance/4;
    result.min_part_length = result.max_distance;

    // Align support points
    // TODO: propagate print resolution
    result.minimal_move = scale_(0.1); // 0.1 mm is enough
    // [in nanometers --> 0.01mm ], devide from print resolution to quater pixel is too strict
    result.count_iteration = 30; // speed VS precission
    result.max_align_distance = result.max_distance / 2;

    verify(result);
    return result;
}

std::optional<SampleConfig> SampleConfigFactory::gui_sample_config_opt;
SampleConfig &SampleConfigFactory::get_sample_config() {
    // init config
    if (!gui_sample_config_opt.has_value())
        // create default configuration
        gui_sample_config_opt = sla::SampleConfigFactory::create(.4f); 
    return *gui_sample_config_opt;
}
