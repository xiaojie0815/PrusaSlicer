#ifndef SLIC3R_HPP
#define SLIC3R_HPP

#include "libslic3r/Config.hpp"
#include "libslic3r/Model.hpp"

namespace Slic3r {

namespace IO {
	enum ExportFormat : int { 
        OBJ, 
        STL, 
        // SVG, 
        TMF, 
        Gcode
    };
}

struct CliInParams 
{
    DynamicPrintAndCLIConfig    config;
    std::vector<std::string>    input_files;
    std::vector<std::string>    actions;
    std::vector<std::string>    transforms;
    std::vector<std::string>    profiles_sharing;
};

struct CliOutParams 
{
    DynamicPrintConfig                      print_config;
    PrinterTechnology                       printer_technology;
    ForwardCompatibilitySubstitutionRule    config_substitution_rule;
    std::vector<Model>                      models;
    bool                                    user_center_specified       { false };
    bool                                    delete_after_load           { false };
};

class CLI {
public:
    int run(int argc, char **argv);

private:

    bool setup(int argc, char **argv, CliInParams& in);
};

}

#endif
