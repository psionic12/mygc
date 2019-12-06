set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(gcptr_test GcPtrTest.cpp GcPtrThreadTest.cpp RootTest.cpp)
target_link_libraries(gcptr_test ${GTEST_BOTH_LIBRARIES} Threads::Threads mygc)
target_include_directories(gcptr_test PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(gcptr_test PUBLIC ../../includes)

add_test(AllTests gcptr_test)