///|/ Copyright (c) Prusa Research 2016 - 2023 Tomáš Mészáros @tamasmeszaros, Oleksandra Iushchenko @YuSanka, Enrico Turri @enricoturri1966, Lukáš Matěna @lukasmatena, Vojtěch Bubník @bubnikv, Filip Sykala @Jony01, Lukáš Hejl @hejllukas, David Kocík @kocikdav, Vojtěch Král @vojtechkral
///|/ Copyright (c) 2019 John Drake @foxox
///|/ Copyright (c) 2019 Sijmen Schoon
///|/ Copyright (c) 2017 Eyal Soha @eyal0
///|/ Copyright (c) Slic3r 2014 - 2015 Alessandro Ranellucci @alranel
///|/
///|/ ported from lib/Slic3r/Model.pm:
///|/ Copyright (c) Prusa Research 2016 - 2022 Vojtěch Bubník @bubnikv, Enrico Turri @enricoturri1966
///|/ Copyright (c) Slic3r 2012 - 2016 Alessandro Ranellucci @alranel
///|/
///|/ PrusaSlicer is released under the terms of the AGPLv3 or higher
///|/
#pragma once

//#include "TriangleMesh.hpp"

#include "PrintConfig.hpp"
#include "enum_bitmask.hpp"

namespace Slic3r {

class Model;
class TriangleMesh;

namespace FileReader
{
    enum class LoadAttribute : int {
        AddDefaultInstances,
        CheckVersion
    };
    using LoadAttributes = enum_bitmask<LoadAttribute>;

    Model read_from_file(const std::string& input_file,
                         DynamicPrintConfig* config = nullptr, 
                         ConfigSubstitutionContext* config_substitutions = nullptr,
                         LoadAttributes options = LoadAttribute::AddDefaultInstances);
    Model read_from_archive(const std::string& input_file,
                            DynamicPrintConfig* config, 
                            ConfigSubstitutionContext* config_substitutions,
                            boost::optional<Semver> &prusaslicer_generator_version,
                            LoadAttributes options = LoadAttribute::AddDefaultInstances);

    Model           load_model(const std::string& input_file);
    Model           load_model(const std::string& input_file, std::string& errors);
    TriangleMesh    load_mesh(const std::string& input_file);
}
    
ENABLE_ENUM_BITMASK_OPERATORS(FileReader::LoadAttribute)

} // namespace Slic3r::ModelProcessing
