cmake_minimum_required(VERSION 3.10)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

find_package(Z3 REQUIRED)
slic3r_remap_configs("z3::libz3" RelWithDebInfo Release)

add_library(libseqarrange STATIC src/seq_interface.cpp src/seq_preprocess.cpp src/seq_sequential.cpp src/seq_utilities.cpp)
target_include_directories(libseqarrange PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(libseqarrange libslic3r z3::libz3)

add_executable(sequential_arrange src/sequential_prusa.cpp)
target_include_directories(sequential_arrange PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(sequential_arrange libseqarrange)

add_executable(sequential_decimator src/sequential_decimator.cpp)
target_include_directories(sequential_decimator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(sequential_decimator libseqarrange)



# The tests - separate executables for now.

# Commented-out for now - depends on Gecode
# add_executable(seq_test_arrangement test/seq_test_arrangement.cpp)
# target_link_libraries(seq_test_arrangement libseqarrange)

add_executable(seq_test_polygon test/seq_test_polygon.cpp test/prusaparts.cpp)
target_link_libraries(seq_test_polygon libseqarrange)
target_include_directories(seq_test_polygon PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

add_executable(seq_test_sequential test/seq_test_sequential.cpp)
target_link_libraries(seq_test_sequential libseqarrange)
target_include_directories(seq_test_sequential PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

add_executable(seq_test_preprocess test/seq_test_preprocess.cpp test/prusaparts.cpp)
target_link_libraries(seq_test_preprocess libseqarrange)
target_include_directories(seq_test_preprocess PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

add_executable(seq_test_interface test/seq_test_interface.cpp)
target_link_libraries(seq_test_interface libseqarrange)
target_include_directories(seq_test_interface PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

