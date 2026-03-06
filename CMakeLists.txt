cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

get_filename_component(ProjectName ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${ProjectName})

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_compile_options(-Wconversion)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost/include)