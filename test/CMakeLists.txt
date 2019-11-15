set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(test HeapTest.cpp GCTest.cpp StopTheWorldTest.cpp AlignmentTest.cpp ToolTest.cpp BitSetTest.cpp)
target_link_libraries(test ${GTEST_BOTH_LIBRARIES} mygc)
target_include_directories(test PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(test Threads::Threads)

add_executable(non_gtest_stop_test NonGTestStopTheWorldTest.cpp)
target_link_libraries(non_gtest_stop_test Threads::Threads)
target_link_libraries(non_gtest_stop_test mygc Threads::Threads)

add_test(AllTests test)