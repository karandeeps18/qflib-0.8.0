# set location of python headers and libraries
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")           # VC++ on Windows
    set(PYTHON_INCLUDE_DIRECTORY ${Python3_INCLUDE_DIRS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")       # GCC on Linux
    set(PYTHON_INCLUDE_DIRECTORY ${Python3_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Unsupported C++ compiler")
endif()

# Manually set the path to the conda environment
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE NUMPY_INCLUDES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "NumPy includes: ${NUMPY_INCLUDES}")

set(pyqflib_SOURCES
    pymodule.cpp
)

add_library(pyqflib SHARED ${pyqflib_SOURCES})
add_dependencies(pyqflib qflib)

target_include_directories(pyqflib PRIVATE
    ..
    ${PYTHON_INCLUDE_DIRECTORY}
    "${NUMPY_INCLUDES}"          
    ${ARMADILLO_INCLUDE_DIRS}  
)


#  Link your Python extension to required libraries:

target_link_libraries(pyqflib PRIVATE
    qflib                  # so calls in pymodule.cpp can reference qflib
    ${Python3_LIBRARIES}   
    ${ARMADILLO_LIBRARIES}
)

# remove prefix from the DLL name
set_target_properties(pyqflib PROPERTIES PREFIX "")
set_target_properties(pyqflib PROPERTIES DEBUG_POSTFIX "")
# silence the numpy deprecation message
set_target_properties(pyqflib PROPERTIES COMPILE_DEFINITIONS "NPY_NO_DEPRECATED_API")

# linker option adjustments for supported compilers
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")         # VC++ on Windows
    # adjust suffix and output location
    set_target_properties(pyqflib PROPERTIES SUFFIX ".pyd")
    set_target_properties(pyqflib PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
                          ${CMAKE_CURRENT_SOURCE_DIR}/qflib)

    # link libraries and options
    target_link_options(pyqflib PRIVATE /MANIFEST:NO /INCREMENTAL:NO /DEBUG)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")       # GCC on Linux
    # output location
    set_target_properties(pyqflib PROPERTIES 
                          LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qflib)

endif()