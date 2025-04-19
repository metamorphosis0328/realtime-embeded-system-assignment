cmake_minimum_required(VERSION 3.10.0)

project(gomoku_robot VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    lib/PCA9685Driver.cpp
)

add_executable(gomoku_robot ${SOURCES})

option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    add_executable(test_pca9685
        tests/test_pca9685.cpp
        lib/PCA9685Driver.cpp
    )
endif()