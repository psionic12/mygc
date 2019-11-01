find_package(GLOG REQUIRED)

add_library(mygc Heap.cpp GarbageCollector.cpp stop_the_world.cpp)
target_include_directories(mygc PUBLIC ../includes)

target_link_libraries(mygc glog::glog)
target_include_directories(mygc PUBLIC glog)