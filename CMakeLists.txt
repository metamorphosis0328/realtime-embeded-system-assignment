cmake_minimum_required(VERSION 3.10.0)

project(gomoku_robot VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wno-deprecated-enum-enum-conversion) # Ignore OpenCV warning due to C++20

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

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
)

add_executable(gomoku_robot ${SOURCES})
target_link_libraries(gomoku_robot ${OpenCV_LIBRARIES}) # OpenCV

option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    add_executable(test_pca9685
    tests/test_pca9685.cpp
    src/driver/PCA9685Driver.cpp
    )

    add_executable(test_servo
    tests/test_servo.cpp
    src/driver/PCA9685Driver.cpp
    src/driver/Servo.cpp
)

    add_executable(test_arm
    tests/test_arm.cpp
    src/driver/PCA9685Driver.cpp
    src/driver/Servo.cpp
    src/driver/Pump.cpp
    src/driver/Electromagnet.cpp
    src/app/arm/ArmController.cpp
    )

    # add_executable(test_vision_ai
    # tests/test_vision.cpp
    # src/app/camera/GomokuVision.cpp
    # )
    # target_link_libraries(test_vision ${OpenCV_LIBRARIES}) # OpenCV

    add_executable(test_ai
    tests/test_ai.cpp
    src/app/camera/GomokuVision.cpp
    src/app/algorithm/GomokuAI.cpp
    )
    target_link_libraries(test_ai ${OpenCV_LIBRARIES}) # OpenCV
endif()