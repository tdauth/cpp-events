cmake_minimum_required(VERSION 3.10)

project(cpp-events)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)

ExternalProject_Add(cpp-mvar
    GIT_REPOSITORY https://github.com/tdauth/cpp-mvar.git
)

set(cpp_mvar_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpp-mvar-prefix/src/cpp-mvar/src/")

enable_testing()

find_package(Boost REQUIRED)

include_directories("${cpp_mvar_INCLUDE_DIR}" ${Boost_INCLUDE_DIR})

add_executable(events src/test/events.cpp)
add_test(EventsTest events)
