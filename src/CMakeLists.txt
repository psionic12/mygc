find_package(GLOG REQUIRED)

add_library(mygc Heap.cpp GarbageCollector.cpp stop_the_world.cpp YoungGeneration.cpp OldGeneration.cpp GcReference.cpp Tools.h BitSet.cpp BitSet.h DynamicSlots.h Block.cpp Block.h TypeDescriptor.cpp TypeDescriptor.h ObjectRecord.h YoungGenerationPool.cpp YoungGenerationPool.h RecordList.h LargeObjects.h LargeObjects.cpp)
target_include_directories(mygc PUBLIC ../includes)

target_link_libraries(mygc glog::glog)
target_include_directories(mygc PUBLIC glog)
target_compile_definitions(mygc PUBLIC -DMYGC_DEBUG)