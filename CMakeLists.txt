cmake_minimum_required(VERSION 3.5)

if(DEFINED ENV{VCPKG_ROOT})
    string(REPLACE "\\" "/" vcpkgRoot "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    # message(${vcpkgRoot})
    set(CMAKE_TOOLCHAIN_FILE ${vcpkgRoot})
endif()

project(linerederer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(linerenderer src/main.cpp src/renderer.cpp)
target_link_libraries(linerenderer PRIVATE glfw)
target_link_libraries(linerenderer PRIVATE unofficial::gl3w::gl3w)
target_link_libraries(linerenderer PRIVATE glm::glm)