# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set C++ standard and flags
set(CMAKE_CXX_STANDARD 17)


# add_executable(seq_test_arrangement seq_test_arrangement.cpp)
# target_link_libraries(seq_test_arrangement libseqarrange)

add_executable(seq_test_polygon seq_test_polygon.cpp prusaparts.cpp)
target_link_libraries(seq_test_polygon libseqarrange)

add_executable(seq_test_sequential seq_test_sequential.cpp)
target_link_libraries(seq_test_sequential libseqarrange)

add_executable(seq_test_preprocess seq_test_preprocess.cpp prusaparts.cpp)
target_link_libraries(seq_test_preprocess libseqarrange)

add_executable(seq_test_interface seq_test_interface.cpp)
target_link_libraries(seq_test_interface libseqarrange)

