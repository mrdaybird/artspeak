cmake_minimum_required(VERSION 3.5)

project(artspeak)
cmake_policy(SET CMP0054 NEW)

set(EMSCRIPTEN_SYSTEM_PROCESSOR x86_64)
set(CMAKE_SYSTEM_PROCESSOR ${EMSCRIPTEN_SYSTEM_PROCESSOR})

include(FetchContent)

find_package(fmt QUIET)
if(NOT fmt_FOUND)
    FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
    )
    FetchContent_MakeAvailable(fmt)
endif()

if(${PLATFORM} MATCHES "Desktop")
    add_compile_options(-g -Wall)
    add_link_options(-fsanitize=address,undefined)

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
endif ()

if(${PLATFORM} MATCHES "Web")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")    
    
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sEXPORTED_FUNCTIONS=_compile,_main 
    -sEXPORTED_RUNTIME_METHODS=ccall,stringToNewUTF8,UTF8ToString 
    --shell-file ${CMAKE_SOURCE_DIR}/web_canvas/shell.html")

    set(RAYLIB_VERSION 5.0)
    find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
    if (NOT raylib_FOUND) # If there's none, fetch and build raylib
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
            set(FETCHCONTENT_QUIET NO)
            FetchContent_Populate(raylib)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
            add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
        endif()
    endif()


    add_executable(canvas_web
        web_canvas/canvas_web.cpp
        lang/chunk.cpp
        lang/compiler.cpp
        lang/debug.cpp
        lang/lang.cpp
        lang/scanner.cpp
        lang/tokentype.cpp
        lang/vm.cpp
    )
    target_include_directories(canvas_web PUBLIC lang)
    target_compile_options(canvas_web PUBLIC "-std=c++20")
    set_target_properties(canvas_web PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/canvas_web) 
    
   add_custom_command(
        TARGET canvas_web PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/web_canvas/style.css
                ${CMAKE_BINARY_DIR}/canvas_web/style.css)
 

    set_target_properties(canvas_web PROPERTIES SUFFIX ".js")
    target_link_options(canvas_web PUBLIC "-sUSE_GLFW=3")
    target_link_libraries(canvas_web raylib fmt::fmt)
endif()

