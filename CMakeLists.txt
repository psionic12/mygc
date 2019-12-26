cmake_minimum_required(VERSION 3.12)
project(mygc)

set(CMAKE_CXX_STANDARD 14)
#set (CMAKE_C_COMPILER clang)
#set (CMAKE_CXX_COMPILER clang++)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")

add_subdirectory(src)

add_subdirectory(test)

add_subdirectory(gc_ptr)
