cmake_minimum_required(VERSION 3.20)

# Set the project name and version
project(SDL2Template VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and link vcpkg packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Manually find tmxlite (since pkg-config isn't working)
find_library(TMXLITE_LIBRARY 
    NAMES tmxlite-s
    PATHS ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/arm64-osx/lib
    REQUIRED
)

find_path(TMXLITE_INCLUDE_DIR 
    NAMES tmxlite/Map.hpp
    PATHS ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/arm64-osx/include
    REQUIRED
)

# Create the executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    box2d::box2d
    glm::glm
    spdlog::spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json
    ${TMXLITE_LIBRARY}
    glad::glad
    imgui::imgui
)

# Add tmxlite include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${TMXLITE_INCLUDE_DIR})

# Copy assets folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})