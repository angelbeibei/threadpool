cmake_minimum_required(VERSION 2.8)

project(dy_thread_pool)

SET(CMAKE_CXX_COMPILER "g++")  

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )

add_executable(main main.cpp thread.cpp)

