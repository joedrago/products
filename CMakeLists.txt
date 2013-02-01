cmake_minimum_required(VERSION 2.6)

project(products)

# ----------------------------------------------------------------------------

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g")
    set(CMAKE_C_FLAGS "-g")
endif()

if(MSVC)
    set(CMAKE_C_FLAGS /wd4996)
endif()

add_executable(products
    products.cpp
)

target_link_libraries(products
    stdc++
)
