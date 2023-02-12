cmake_minimum_required(VERSION 2.8)

project(libthreadpool LANGUAGES CXX)
set(LIB_NAME "threadpool")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include_directories(
  include
)

add_library(${LIB_NAME} SHARED
  src/threadpool.cpp
  src/task.cpp
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include/${LIB_NAME}
)
