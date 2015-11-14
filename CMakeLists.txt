cmake_minimum_required(VERSION 2.8)

project(topology)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -g -O0")

add_executable(top
    main.cpp
    test.cpp
    test_metric.cpp
    test_topology.cpp
    test_algorithm.cpp)
