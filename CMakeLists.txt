cmake_minimum_required(VERSION 3.27)
project(net-cpp)

file(GLOB SOURCE "src/*.cpp")
file(GLOB HEADER "include/*.h")
file(GLOB TESTS "tests/*.cpp")

set(CMAKE_CXX_STANDARD 17)

find_package(spdlog REQUIRED)
add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCE} ${HEADER})
target_link_libraries(${CMAKE_PROJECT_NAME} spdlog::spdlog)

add_executable(net-cpp-tests ${TESTS})
target_link_libraries(net-cpp-tests ${CMAKE_PROJECT_NAME})