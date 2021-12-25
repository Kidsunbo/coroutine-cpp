cmake_minimum_required(VERSION 3.0.0)
project(kiedis VERSION 0.1.0)

include(CTest)

if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

aux_source_directory(src SOURCE)
add_library(kiedis ${SOURCE})

