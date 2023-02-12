cmake_minimum_required(VERSION 2.8)

project(demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(EXE_NAME "threadpoolDemo")

include_directories(include)

add_executable(${EXE_NAME}
  src/main.cpp
)

install(TARGETS ${EXE_NAME}
  RUNTIME DESTINATION bin
)

#This is necessary for the bin/threadpoolDemo executable to find libthreadpool.so in ${CMAKE_INSTALL_PREFIX}/lib
#Comment it to search libraries globally
set_target_properties(${EXE_NAME} PROPERTIES
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

target_include_directories(${EXE_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../libthreadpool/include>
)

target_link_libraries(${EXE_NAME} threadpool)
