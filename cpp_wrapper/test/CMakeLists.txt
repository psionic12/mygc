set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(cpp_wrapper_test CppWrapperTest.cpp)
target_link_libraries(cpp_wrapper_test ${GTEST_BOTH_LIBRARIES} Threads::Threads mygc_cpp)
target_include_directories(cpp_wrapper_test PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(cpp_wrapper_test PUBLIC ../../includes)

add_test(AllTests cpp_wrapper_test)