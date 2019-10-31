add_subdirectory(test)

find_package(Boost REQUIRED)
add_library(gcptr src/PtrHeaders.cpp)
target_link_libraries(gcptr PUBLIC mygc)
target_include_directories(gcptr PUBLIC ../includes headers ${Boost_INCLUDE_DIRS})