
cmake_minimum_required(VERSION 3.20.3)
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "D:/Programming/C++/Libraries/boost_1_73_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "D:/Programming/C++/Libraries/boost_1_73_0/libs")
project(server)
find_package(Boost)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable("${PROJECT_NAME}" server.cpp "server.hpp") 
    target_link_libraries("${PROJECT_NAME}" ${Boost_LIBRARIES})
    target_link_libraries("${PROJECT_NAME}" ws2_32)
    target_link_libraries("${PROJECT_NAME}" mswsock)
endif()
