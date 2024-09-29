cmake_minimum_required(VERSION 3.10)

project(Surynek LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_library(libseqarrange STATIC seq_interface.cpp seq_preprocess.cpp seq_sequential.cpp seq_utilities.cpp)
add_executable(sequential_arrange sequential_prusa.cpp)
add_executable(sequential_decimator sequential_decimator.cpp)

target_include_directories(libseqarrange PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

find_package(Z3 REQUIRED)
slic3r_remap_configs("z3::libz3" RelWithDebInfo Release)

target_link_libraries(libseqarrange libslic3r z3::libz3)
target_link_libraries(sequential_arrange libseqarrange)
target_link_libraries(sequential_decimator libseqarrange)



