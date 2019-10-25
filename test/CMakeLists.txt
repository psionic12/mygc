set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(test ChunkTest.cpp)
target_link_libraries(test ${GTEST_BOTH_LIBRARIES} mygc)
target_include_directories(test PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(test Threads::Threads)

add_test(AllTests test)