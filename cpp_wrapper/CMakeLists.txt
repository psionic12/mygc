add_subdirectory(test)

find_package(Boost REQUIRED)
add_library(mygc_cpp src/PtrHeaders.cpp)
target_link_libraries(mygc_cpp PUBLIC mygc)
target_include_directories(mygc_cpp PUBLIC ../includes headers ${Boost_INCLUDE_DIRS})