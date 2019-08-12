cmake_minimum_required(VERSION 2.8)

set (PROJECT hash_decoder)

set (HEADERS headers/targetver.hpp
             headers/md5.hpp
             headers/decoder.hpp)

set (SOURCES hash_decoder.cpp
             sources/decoder.cpp)

set (BOOST_COMPONENTS thread
                      filesystem)

set (Boost_USE_STATIC_LIBS ON)

set (Boost_USE_MULTITHREADED ON)

project (${PROJECT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

include_directories (../)
include_directories(headers)

find_package (Threads)

find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries (${PROJECT} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})








