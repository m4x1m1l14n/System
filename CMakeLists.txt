cmake_minimum_required(VERSION 3.15.5)
project (System)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

include_directories("System/include/")

set(SOURCES "System/src/System/TimeSpan.cpp")

add_library(System ${SOURCES})
