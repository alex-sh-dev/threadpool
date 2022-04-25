set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_library(threadpool SHARED
    threadpool.cpp
    task.cpp)

target_include_directories(threadpool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
