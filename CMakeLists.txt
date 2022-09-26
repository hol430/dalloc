cmake_minimum_required(VERSION 3.13)

# set the project name
project(dalloc VERSION 0.1)

set(dalloc dalloc)
add_library("${dalloc}" SHARED "")
set_property(TARGET "${dalloc}" PROPERTY CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(src)

set(test test)
add_executable("${test}" "")
add_subdirectory(test)
set_target_properties("${test}" PROPERTIES OUTPUT_NAME unittests)
