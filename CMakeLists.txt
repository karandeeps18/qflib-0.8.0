cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
project(qflib)

# find Armadillo
set(CONDA_ENV_PATH "C:/ProgramData/miniconda3/envs/qfgb/Library")
set(CMAKE_PREFIX_PATH "${CONDA_ENV_PATH};${CMAKE_PREFIX_PATH}")
find_package(Armadillo REQUIRED)
message(STATUS "Armadillo include dirs: ${Armadillo_INCLUDE_DIRS}")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# set location of artifacts
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# set the debug postfix
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX "-gd")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_DEBUG_POSTFIX "")
endif()

# global compiler and linker option adjustments
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/EHsc /permissive-")
	# use /Zi for Debug configs
	string(REGEX REPLACE "/Z[iI7]" "" TEMP "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG "${TEMP} /Zi")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fPIC -Wno-deprecated-declarations -Wno-attributes")
else()
    message(FATAL_ERROR "unknown compiler; only MSVC and GNU are currently supported" )
endif()

add_subdirectory(qflib)
add_subdirectory(pyqflib)