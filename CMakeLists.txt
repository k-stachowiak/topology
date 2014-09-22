cmake_minimum_required(VERSION 2.8)

project(topology)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g -O0")

add_executable(top main.cpp)
