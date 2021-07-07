cmake_minimum_required(VERSION 3.20.3)

project(local-chat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x86-windows/include)

find_package(Boost)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

add_executable(server ./src/server/server.cpp ./src/server/server.hpp)
