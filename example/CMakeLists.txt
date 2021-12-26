cmake_minimum_required(VERSION 3.0.0)

add_executable(coroutine coroutine.cpp)
target_include_directories(coroutine PUBLIC ../src)
target_link_libraries(coroutine PUBLIC kiedis)

add_executable(awaitable awaitable.cpp)
target_include_directories(awaitable PUBLIC ../src)
target_link_libraries(awaitable PUBLIC kiedis)