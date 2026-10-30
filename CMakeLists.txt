cmake_minimum_required(VERSION 3.15) 

set(OPENGL_VERSION "4.3" CACHE STRING "" FORCE)
set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/raylib SYSTEM)

project("anysize minesweeper")

if(MINGW)
    # Force static linking for standard libraries
    add_link_options(-static -static-libgcc -static-libstdc++)
    
    # Optional: If you use pthreads, ensure they are linked statically
    add_link_options(-Wl,-Bstatic,--whole-archive -lpthread -Wl,--no-whole-archive)
endif()

option(GENERATE_SHADERS "Generate shaders.hpp using bytify" OFF)
message(STATUS "GENERATE_SHADERS = ${GENERATE_SHADERS}")

set(CMAKE_CXX_STANDARD 20)

add_executable(minesweeper "src/main.cpp" "src/minefield.cpp") 

if(GENERATE_SHADERS)
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/shaders/shaders.hpp
        COMMAND ${CMAKE_SOURCE_DIR}/bytify
        -or ${CMAKE_SOURCE_DIR}/shaders/shaders.hpp 
        -is ${CMAKE_SOURCE_DIR}/shaders/chunk_renderer.vert 
        -is ${CMAKE_SOURCE_DIR}/shaders/chunk_renderer.frag 
        -is ${CMAKE_SOURCE_DIR}/shaders/minefield_proccessor_optomized.comp
        -is ${CMAKE_SOURCE_DIR}/shaders/testing.vert
        -is ${CMAKE_SOURCE_DIR}/shaders/testing.frag
        DEPENDS 
            ${CMAKE_SOURCE_DIR}/bytify 
            ${CMAKE_SOURCE_DIR}/shaders/chunk_renderer.vert
            ${CMAKE_SOURCE_DIR}/shaders/chunk_renderer.frag 
            ${CMAKE_SOURCE_DIR}/shaders/minefield_proccessor_optomized.comp
            ${CMAKE_SOURCE_DIR}/shaders/testing.vert
            ${CMAKE_SOURCE_DIR}/shaders/testing.frag
        COMMENT "Generating Shaders"
        VERBATIM
    )

    add_custom_target(generate_shaders
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.hpp
    )

    add_dependencies(minesweeper generate_shaders)
endif()

target_include_directories(minesweeper PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
    ${CMAKE_CURRENT_SOURCE_DIR}/textures/ 
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

target_link_libraries(minesweeper PRIVATE raylib)

target_compile_options(minesweeper PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -g -O0 -fsanitize=address -fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O3 -DNDEBUG -flto>
) 

target_link_options(minesweeper PRIVATE
    $<$<CONFIG:Debug>: -fsanitize=address >
    $<$<CONFIG:Release>: -flto>
)