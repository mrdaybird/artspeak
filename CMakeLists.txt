cmake_minimum_required(VERSION 3.5)
project(artspeak)

find_package(fmt)    

add_compile_options(-g -Wall)
add_link_options(-fsanitize=address)

add_library(lang  
    lang/chunk.cpp
    lang/compiler.cpp
    lang/debug.cpp
    lang/lang.cpp
    lang/scanner.cpp
    lang/tokentype.cpp
    lang/vm.cpp
    )
target_link_libraries(
    lang fmt::fmt
)

add_executable(canvas
    canvas/canvas.cpp)

target_include_directories(canvas PUBLIC lang)
target_link_libraries(canvas lang raylib fmt::fmt)

add_executable(lang-test
    lang/lang-test.cpp 
    )

target_link_libraries(lang-test lang fmt::fmt)

