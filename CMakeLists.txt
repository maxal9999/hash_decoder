cmake_minimum_required(VERSION 2.8)

set (PROJECT hash_decoder)

project (${PROJECT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pthread")

include_directories(headers)

#------------------------------------------------------------------

set (LIB_HEADERS headers/md5.hpp
                 headers/decoder.hpp)

set (LIB_SOURCES sources/decoder.cpp)

set (LIB ${PROJECT}_lib)

add_library (${LIB} ${LIB_HEADERS} ${LIB_SOURCES})

#------------------------------------------------------------------

set (HEADERS headers/targetver.hpp)

set (SOURCES hash_decoder.cpp)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries (${PROJECT} ${CMAKE_THREAD_LIBS_INIT} ${LIB})

#------------------------------------------------------------------

set (TESTS_SOURCES test/hash_decoder_tests.cpp)

set (TEST test_${PROJECT})
add_executable (${TEST} ${TESTS_SOURCES})
find_package (Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries (${TEST} ${Boost_LIBRARIES} ${LIB})
enable_testing ()
add_test (${TEST} ${TEST})
