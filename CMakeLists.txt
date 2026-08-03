cmake_minimum_required(VERSION 3.15) 
add_subdirectory(thirdparty/raylib SYSTEM)

project("anysize minesweeper")

set(CMAKE_CXX_STANDARD 17)

add_executable(minesweeper "src/main.cpp") 

target_link_libraries(minesweeper PRIVATE raylib)
target_include_directories(minesweeper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_options(minesweeper PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
) 

target_compile_definitions(minesweeper PRIVATE
    $<$<CONFIG:Debug>:RESOURCES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/resources/\">
    $<$<CONFIG:Debug>:SHADER_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\">
    $<$<CONFIG:Release>:RESOURCES_PATH=\"resources/\">
     $<$<CONFIG:Release>:SHADER_PATH=\"shaders/\">
)