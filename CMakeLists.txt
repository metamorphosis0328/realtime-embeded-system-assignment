cmake_minimum_required(VERSION 3.10.0)

project(gomoku_robot VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wno-deprecated-enum-enum-conversion) # Ignore OpenCV warning due to C++20

# Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app/arm
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app/algorithm
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app/coordinator
    ${CMAKE_CURRENT_SOURCE_DIR}/include/driver
)

find_package(PkgConfig REQUIRED) # OpenCV
pkg_check_modules(OpenCV REQUIRED opencv4) # OpenCV

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS}) # OpenCV

set(SOURCES
    src/main.cpp
    src/driver/PCA9685Driver.cpp
    src/driver/Servo.cpp
    src/driver/Pump.cpp
    src/driver/Electromagnet.cpp
    src/app/arm/ArmController.cpp
    src/app/camera/GomokuVision.cpp
    src/app/algorithm/GomokuAI.cpp
    src/app/algorithm/MinimaxAlgorithm.cpp
    src/app/coordinator/GomokuCoordinator.cpp
)

add_executable(gomoku_robot ${SOURCES})
target_link_libraries(gomoku_robot ${OpenCV_LIBRARIES}) # OpenCV
