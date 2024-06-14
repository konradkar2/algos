cmake_minimum_required(VERSION 3.10)

project(graph_printer_demo VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wunused -Wcast-align -fsanitize=undefined")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME}
    graph_printer_demo/main.c
    common/graph.c
    common/graph_printer.c
)

target_link_libraries(${PROJECT_NAME}
    raylib
    m
)


