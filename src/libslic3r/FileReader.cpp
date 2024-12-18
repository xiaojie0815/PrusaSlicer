///|/ Copyright (c) Prusa Research 2016 - 2023 Tomáš Mészáros @tamasmeszaros, Oleksandra Iushchenko @YuSanka, David Kocík @kocikdav, Enrico Turri @enricoturri1966, Lukáš Matěna @lukasmatena, Vojtěch Bubník @bubnikv, Lukáš Hejl @hejllukas, Filip Sykala @Jony01, Vojtěch Král @vojtechkral
///|/ Copyright (c) 2021 Boleslaw Ciesielski
///|/ Copyright (c) 2019 John Drake @foxox
///|/ Copyright (c) 2019 Sijmen Schoon
///|/ Copyright (c) Slic3r 2014 - 2016 Alessandro Ranellucci @alranel
///|/ Copyright (c) 2015 Maksim Derbasov @ntfshard
///|/
///|/ ported from lib/Slic3r/Model.pm:
///|/ Copyright (c) Prusa Research 2016 - 2022 Vojtěch Bubník @bubnikv, Enrico Turri @enricoturri1966
///|/ Copyright (c) Slic3r 2012 - 2016 Alessandro Ranellucci @alranel
///|/
///|/ PrusaSlicer is released under the terms of the AGPLv3 or higher
///|/
#include "FileReader.hpp"
#include "Model.hpp"
#include "TriangleMesh.hpp"

#include "Format/AMF.hpp"
#include "Format/OBJ.hpp"
#include "Format/STL.hpp"
#include "Format/3mf.hpp"
#include "Format/STEP.hpp"
#include "Format/SVG.hpp"
#include "Format/PrintRequest.hpp"

namespace Slic3r::FileReader
{
// Loading model from a file, it may be a simple geometry file as STL or OBJ, however it may be a project file as well.
Model read_from_file(const std::string& input_file, DynamicPrintConfig* config, ConfigSubstitutionContext* config_substitutions, LoadAttributes options)
{
    Model model;

    DynamicPrintConfig temp_config;
    ConfigSubstitutionContext temp_config_substitutions_context(ForwardCompatibilitySubstitutionRule::EnableSilent);
    if (config == nullptr)
        config = &temp_config;
    if (config_substitutions == nullptr)
        config_substitutions = &temp_config_substitutions_context;

    bool result = false;
    if (boost::algorithm::iends_with(input_file, ".stl"))
        result = load_stl(input_file.c_str(), &model);
    else if (boost::algorithm::iends_with(input_file, ".obj"))
        result = load_obj(input_file.c_str(), &model);
    else if (boost::algorithm::iends_with(input_file, ".step") || boost::algorithm::iends_with(input_file, ".stp"))
        result = load_step(input_file.c_str(), &model);
    else if (boost::algorithm::iends_with(input_file, ".amf") || boost::algorithm::iends_with(input_file, ".amf.xml"))
        result = load_amf(input_file.c_str(), config, config_substitutions, &model, options & LoadAttribute::CheckVersion);
    else if (boost::algorithm::iends_with(input_file, ".3mf") || boost::algorithm::iends_with(input_file, ".zip")) {
        //FIXME options & LoadAttribute::CheckVersion ? 
        boost::optional<Semver> prusaslicer_generator_version;
        result = load_3mf(input_file.c_str(), *config, *config_substitutions, &model, false, prusaslicer_generator_version);
    } else if (boost::algorithm::iends_with(input_file, ".svg"))
        result = load_svg(input_file, model);
    else if (boost::ends_with(input_file, ".printRequest"))
        result = load_printRequest(input_file.c_str(), &model);
    else
        throw Slic3r::RuntimeError("Unknown file format. Input file must have .stl, .obj, .step/.stp, .svg, .amf(.xml) or extension .3mf(.zip).");

    if (!result)
        throw Slic3r::RuntimeError("Loading of a model file failed.");

    if (model.objects.empty())
        throw Slic3r::RuntimeError("The supplied file couldn't be read because it's empty");

    if (!boost::ends_with(input_file, ".printRequest"))
        for (ModelObject* o : model.objects)
            o->input_file = input_file;

    if (options & LoadAttribute::AddDefaultInstances)
        model.add_default_instances();

    for (CustomGCode::Info& info : model.get_custom_gcode_per_print_z_vector()) {
        CustomGCode::update_custom_gcode_per_print_z_from_config(info, config);
        CustomGCode::check_mode_for_custom_gcode_per_print_z(info);
    }

    sort_remove_duplicates(config_substitutions->substitutions);
    return model;
}

// Loading model from a file (3MF or AMF), not from a simple geometry file (STL or OBJ).
Model read_from_archive(const std::string& input_file,
                        DynamicPrintConfig* config,
                        ConfigSubstitutionContext*,
                        config_substitutions,
                        boost::optional<Semver> &prusaslicer_generator_version,
                        LoadAttributes options)
{
    assert(config != nullptr);
    assert(config_substitutions != nullptr);

    Model model;

    bool result = false;
    if (boost::algorithm::iends_with(input_file, ".3mf") || boost::algorithm::iends_with(input_file, ".zip"))
        result = load_3mf(input_file.c_str(), *config, *config_substitutions, &model, options & LoadAttribute::CheckVersion, prusaslicer_generator_version);
    else if (boost::algorithm::iends_with(input_file, ".zip.amf"))
        result = load_amf(input_file.c_str(), config, config_substitutions, &model, options & LoadAttribute::CheckVersion);
    else
        throw Slic3r::RuntimeError("Unknown file format. Input file must have .3mf or .zip.amf extension.");

    if (!result)
        throw Slic3r::RuntimeError("Loading of a model file failed.");

    for (ModelObject* o : model.objects) {
        //if (boost::algorithm::iends_with(input_file, ".zip.amf")) {
        //    // we remove the .zip part of the extension to avoid it be added to filenames when exporting
        //    o->input_file = boost::ireplace_last_copy(input_file, ".zip.", ".");
        //}
        //else
        o->input_file = input_file;
    }

    if (options & LoadAttribute::AddDefaultInstances)
        model.add_default_instances();

    for (CustomGCode::Info& info : model.get_custom_gcode_per_print_z_vector()) {
        CustomGCode::update_custom_gcode_per_print_z_from_config(info, config);
        CustomGCode::check_mode_for_custom_gcode_per_print_z(info);
    }
    handle_legacy_sla(*config);

    return model;
}

Model load_model(const std::string& input_file, std::string& errors)
{
    Model model;
    try {
        model = read_from_file(input_file);
    }
    catch (std::exception& e) {
        errors = input_file + " : " + e.what();
        return model;
    }

    return model;
}

Model load_model(const std::string& input_file)
{
    std::string errors;
    Model model = load_model(input_file, errors);
    return model;
}

TriangleMesh load_mesh(const std::string& input_file)
{
    Model model = load_model(input_file);
    return model.mesh();
}


}
