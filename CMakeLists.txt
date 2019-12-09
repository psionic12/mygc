cmake_minimum_required(VERSION 3.12)
project(mygc)

set(CMAKE_CXX_STANDARD 14)
#set (CMAKE_C_COMPILER clang)
#set (CMAKE_CXX_COMPILER clang++)

add_subdirectory(src)

add_subdirectory(test)

add_subdirectory(gc_ptr)
