find_package(Z3 REQUIRED)
slic3r_remap_configs("z3::libz3" RelWithDebInfo Release)

#find_package(Catch2 2.9 REQUIRED)
#include(Catch)

add_library(libseqarrange STATIC src/seq_interface.cpp src/seq_preprocess.cpp src/seq_sequential.cpp src/seq_utilities.cpp)
target_include_directories(libseqarrange PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(libseqarrange PUBLIC libslic3r PRIVATE z3::libz3)

add_executable(sequential_arrange src/sequential_prusa.cpp)
target_include_directories(sequential_arrange PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(sequential_arrange PRIVATE libseqarrange)

add_executable(sequential_decimator src/sequential_decimator.cpp)
target_include_directories(sequential_decimator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange)
target_link_libraries(sequential_decimator PRIVATE libseqarrange)



# Tests - commented out for now

#add_executable(seq_test_polygon test/seq_test_polygon.cpp test/prusaparts.cpp)
#target_link_libraries(seq_test_polygon PRIVATE Catch2::Catch2 libseqarrange)
#target_include_directories(seq_test_polygon PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

#add_executable(seq_test_sequential test/seq_test_sequential.cpp)
#target_link_libraries(seq_test_sequential PRIVATE libseqarrange)
#target_include_directories(seq_test_sequential PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

#add_executable(seq_test_preprocess test/seq_test_preprocess.cpp test/prusaparts.cpp)
#target_link_libraries(seq_test_preprocess PRIVATE libseqarrange)
#target_include_directories(seq_test_preprocess PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

#add_executable(seq_test_interface test/seq_test_interface.cpp)
#target_link_libraries(seq_test_interface PUBLIC libseqarrange)
#target_include_directories(seq_test_interface PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include/libseqarrange")

#add_test(seq_test_polygon seq_test_polygon)
#add_test(seq_test_preprocess seq_test_preprocess)
#add_test(seq_test_sequential seq_test_sequential)
#add_test(seq_test_interface seq_test_interface)
