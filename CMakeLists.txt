project(mersenne_twister)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

add_definitions(-Wall -g -lm)

find_package(NTL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${NTL_LIBRARY})
include_directories(${NTL_INCLUDE_DIR})
