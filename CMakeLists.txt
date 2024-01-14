cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(FL_PCG VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


# Add static libraries folder
file(GLOB LIB_FILES 
    ${CMAKE_SOURCE_DIR}/lib/*.lib)

target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_FILES})

# Fetch dependencies remotely
include(FetchContent)

# FetchContent_Declare(
#   FluxLumina
#   GIT_REPOSITORY 	https://github.com/diobat/FluxLumina.git
#   GIT_TAG         22-fbo-objects-do-not-actually-own-the-underlying-buffer-textures
# )
# FetchContent_MakeAvailable(FluxLumina)

add_subdirectory(X:/Repos/FluxLumina X:/Repos/FluxLumina/build)
target_link_libraries(${PROJECT_NAME} PRIVATE FluxLumina)

# FastNoise2

FetchContent_Declare(
    FastNoise2
    GIT_REPOSITORY    https://github.com/Auburn/FastNoise2.git
    GIT_TAG           master
)
option(FASTNOISE2_NOISETOOL OFF)
option(FASTNOISE2_TESTS OFF)
FetchContent_MakeAvailable(FastNoise2)

target_link_libraries(${PROJECT_NAME} PUBLIC FastNoise2)