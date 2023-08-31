# https://github.com/pybind/scikit_build_example 
cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

# Compiler and Options
message( "Compiler: ${CMAKE_CXX_COMPILER}" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")
message( "Option: ${CMAKE_CXX_FLAGS}" )

# Include Directories
include_directories(".")
include_directories("libs/phat/include/")

# Boost
find_package(Boost 1.5 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Create a shared library for Python
python_add_library(_core_fzzpy MODULE 
    src/fzz.cpp 
    src/fzz_wrapper.cpp
)
target_link_libraries(_core_fzzpy PRIVATE pybind11::module)
target_compile_definitions(_core_fzzpy PRIVATE VERSION_INFO=${PROJECT_VERSION})
set_property(TARGET _core_fzzpy PROPERTY CXX_VISIBILITY_PRESET "default")

install(TARGETS _core_fzzpy DESTINATION ${SKBUILD_PROJECT_NAME})
