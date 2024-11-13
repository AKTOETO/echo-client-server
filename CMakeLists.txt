cmake_minimum_required(VERSION 3.16)
project(server)

set(server_src 
    src/server.cpp
)
set(client_src
    src/client.cpp
)

add_executable(server ${server_src})
target_compile_options(server PRIVATE -pthread)

add_executable(client ${client_src})
