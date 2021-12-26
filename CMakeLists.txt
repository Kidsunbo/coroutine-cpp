cmake_minimum_required(VERSION 3.0.0)
project(kiedis VERSION 0.1.0)

include(CTest)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_EXAMPLE "Compile example" true)
option(ENABLE_SANITIZER "Use sanitizer" true)

if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

aux_source_directory(src SOURCE)
add_library(kiedis ${SOURCE})

if(ENABLE_EXAMPLE)
    add_subdirectory(example)
endif()

