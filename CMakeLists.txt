cmake_minimum_required(VERSION 3.10)

project(physics)



include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

set(CMAKEJ_C_STANDARD 11)

add_executable(physics src/main.c)

target_link_libraries(physics raylib)



