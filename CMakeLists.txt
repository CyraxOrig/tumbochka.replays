cmake_minimum_required(VERSION 3.15)
project(tubaretka-replays)

find_package(geode REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
geode_add_mod(${PROJECT_NAME} ${SRC_FILES})