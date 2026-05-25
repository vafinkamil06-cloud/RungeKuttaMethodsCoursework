cmake_minimum_required(VERSION 3.18)

project(RungeKuttaMethods LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

add_executable(Runge-Cuttas main.cpp)

target_include_directories(Runge-Cuttas PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(Runge-Cuttas PRIVATE
    Python3::Python
    Python3::NumPy
)
