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

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
if(CMAKE_COMPILER_IS_GNUCXX)
    # include(CodeCoverage)
    # setup_target_for_coverage_lcov(
    # 	NAME "coverage"								# New target name
    # 	EXECUTABLE ${PROJECT_BINARY_DIR}/unittests	# Executable in PROJECT_BINARY_DIR
    # 	DEPENDENCIES "${test}"						# Dependencies to build first
    # 	BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"		# Base directory for report
    # 												#  (defaults to PROJECT_SOURCE_DIR)
    # 	EXCLUDE "${PROJECT_SOURCE_DIR}/test/*"		# Patterns to exclude (can be relative
    # 												#  to BASE_DIRECTORY, with CMake 3.4+)
    # 	#NO_DEMANGLE								# Don't demangle C++ symbols
	# 	#											#  even if c++filt is found
	# )
	set(COVERAGE_DIR coverage)
	set(COVERAGE_TARGET_NAME coverage)
	add_custom_target(${COVERAGE_TARGET_NAME}
        COMMAND lcov -directory . -b ${COVERAGE_DIR} --zerocounters
        COMMAND lcov -c -i -d . -b ${COVERAGE_DIR} -o ${COVERAGE_TARGET_NAME}.base
        COMMAND ${PROJECT_BINARY_DIR}/unittests
        COMMAND lcov --directory . -b ${COVERAGE_DIR} --capture --output-file ${COVERAGE_TARGET_NAME}.capture
        COMMAND lcov -a ${COVERAGE_TARGET_NAME}.base -a ${COVERAGE_TARGET_NAME}.capture --output-file ${COVERAGE_TARGET_NAME}.total
        COMMAND lcov --remove ${COVERAGE_TARGET_NAME}.total ${PROJECT_SOURCE_DIR}/test/* --output-file ${COVERAGE_TARGET_NAME}.info
        COMMAND genhtml -o ../${COVERAGE_TARGET_NAME} ${COVERAGE_TARGET_NAME}.info

        # Set output files as GENERATED (will be removed on 'make clean')
        BYPRODUCTS
            ${COVERAGE_TARGET_NAME}.base
            ${COVERAGE_TARGET_NAME}.capture
            ${COVERAGE_TARGET_NAME}.total
            ${COVERAGE_TARGET_NAME}.info
            ${COVERAGE_TARGET_NAME}/index.html
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${test}
        VERBATIM # Protect arguments to commands
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
    )
endif()


