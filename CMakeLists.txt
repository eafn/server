cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server config.h PthreadPool.cpp PthreadPool.h Task.cpp Task.h Listener.cpp Listener.h MemoryPool.cpp MemoryPool.h  Daemon.cpp Daemon.h main.cpp)

find_package(Threads REQUIRED)

target_link_libraries(server Threads::Threads)
