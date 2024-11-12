cmake_minimum_required(VERSION 3.16)
project(server)

file(GLOB srcs src/*.cpp)
file(GLOB hdrs include/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${srcs} ${hdrs})

