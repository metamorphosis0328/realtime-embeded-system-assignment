cmake_minimum_required(VERSION 3.10)
project(GobangAI)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    minimax_algorithm.cpp
    how_to_use.cpp
)

# Create executable
add_executable(gobang_ai ${SOURCES})

# Output binary to bin directory
set_target_properties(gobang_ai PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install
install(TARGETS gobang_ai DESTINATION bin) 