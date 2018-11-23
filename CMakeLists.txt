cmake_minimum_required(VERSION 2.8)

project(temp_code)
add_executable(${PROJECT_NAME} main.cpp
                               lexer.cpp lexer.h
                               parser.cpp parser.h
                               grammar.cpp grammar.h
                               utility.cpp utility.h)

set(CMAKE_C_COMPILER /usr/bin/gcc-7)
set(CMAKE_CXX_COMPILER /usr/bin/g++-7)

add_definitions(-Dsrc_folder=CMAKE_CURRENT_SOURCE_DIR)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fsanitize=address  -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -Wall -fsanitize=address")
endif()

# from https://stackoverflow.com/questions/17018477/cmake-adding-custom-resources-to-build-directory
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/tree.css" "${CMAKE_CURRENT_SOURCE_DIR}/index.html" "${CMAKE_CURRENT_SOURCE_DIR}/treant-js"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
