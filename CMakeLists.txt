cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp config.h PthreadPool.cpp PthreadPool.h Task.cpp Task.h Listener.cpp Listener.h)

find_package(Threads REQUIRED)

target_link_libraries(server Threads::Threads)