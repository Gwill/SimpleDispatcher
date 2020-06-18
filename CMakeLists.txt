cmake_minimum_required(VERSION 3.16)
project(dispatcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(dispatcher main.cpp DispatchQueue.h)