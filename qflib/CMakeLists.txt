set(qflib_SOURCES
    math/interpol/piecewisepolynomial.cpp 
    math/stats/errorfunction.cpp
    pricers/simplepricers.cpp
    pricers/bsmcpricer.cpp
    market/market.cpp
    market/yieldcurve.cpp
    market/volatilitytermstructure.cpp
)

add_library(qflib STATIC ${qflib_SOURCES})

target_include_directories(qflib PRIVATE 
        ..
    ${Armadillo_INCLUDE_DIRS}
    "C:/ProgramData/miniconda3/envs/qfgb/Library/include"
)

# compiler and linker option adjustments for targets
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(qflib PRIVATE "/permissive-")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") 
    target_compile_options(qflib PRIVATE -fPIC)
endif()
