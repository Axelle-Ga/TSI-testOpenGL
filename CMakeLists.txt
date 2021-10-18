cmake_minimum_required(VERSION 3.5)

project(project1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(project1 main.cpp)

target_link_libraries(project1 OpenGL::GL)
target_link_libraries(project1 glfw)
target_link_libraries(project1 GLEW)
