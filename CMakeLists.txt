cmake_minimum_required(VERSION 3.20.3)

project(local-chat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x86-windows/include)

find_package(Boost)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

include_directories("${CMAKE_SOURCE_DIR}/include")


add_executable(server "${CMAKE_SOURCE_DIR}/src/server/server.cpp" "${CMAKE_SOURCE_DIR}/include/server/server.hpp" "${CMAKE_SOURCE_DIR}/include/chat_message.hpp")
