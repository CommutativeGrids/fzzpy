cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3" )

message( "Compiler: ${CMAKE_CXX_COMPILER}" )
message( "Option: ${CMAKE_CXX_FLAGS}" )

include_directories( "." )

# phat
include_directories("libs/phat/include/")

# Boost
find_package( Boost 1.5 REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

# Locate pybind11
find_package(pybind11 REQUIRED HINTS "${PYTHON_SITE_PACKAGES}/pybind11/share/cmake/pybind11")

# Create a shared library for Python
pybind11_add_module(fzzpy 
    fzzpy/fzz.cpp 
    fzzpy/fzz_wrapper.cpp
)

target_link_libraries(fzzpy PRIVATE pybind11::module)
