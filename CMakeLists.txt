cmake_minimum_required(VERSION 3.12)
project(mygc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")


add_subdirectory(src)

add_subdirectory(test)

add_subdirectory(gc_ptr)
