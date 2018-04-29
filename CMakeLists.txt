cmake_minimum_required(VERSION 3.1)
project(pacman LANGUAGES CXX)

file( GLOB SOURCES src/*.cpp include/*.hpp)
add_executable(pacman_the_retaliation
${PROJECT_SOURCE_DIR}/src ${SOURCES})

set_target_properties(pacman_the_retaliation
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "bin"
    LIBRARY_OUTPUT_DIRECTORY "lib")

find_package(OpenGL REQUIRED)
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "turn off build examples")

add_subdirectory(${PROJECT_SOURCE_DIR}/glfw-3.2.1)
add_subdirectory(${PROJECT_SOURCE_DIR}/assimp)


target_include_directories(pacman_the_retaliation
  PRIVATE ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/glew-2.1.0/include
  PRIVATE ${PROJECT_SOURCE_DIR}/glfw-3.2.1/include
  PRIVATE ${PROJECT_SOURCE_DIR}/glm-0.9.9-a2/glm
  PRIVATE ${PROJECT_SOURCE_DIR}/assimp/include
  PRIVATE ${PROJECT_SOURCE_DIR}/build/assimp/include
  PRIVATE ${PROJECT_SOURCE_DIR}/SOIL/src)

set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/glew-2.1.0/)
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/glew-2.1.0/lib/Release/Win32/)
find_package(GLEW REQUIRED)
configure_file(${PROJECT_SOURCE_DIR}/glew-2.1.0/bin/Release/Win32/glew32.dll ${PROJECT_SOURCE_DIR}/build/glew32.dll COPYONLY)
file(COPY ${PROJECT_SOURCE_DIR}/shaders/ DESTINATION ${PROJECT_SOURCE_DIR}/build/shaders/)
file(COPY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${PROJECT_SOURCE_DIR}/build/assets/)
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib32)

target_link_libraries(pacman_the_retaliation
  PRIVATE ${OPENGL_LIBRARIES}
  PRIVATE ${GLEW_LIBRARIES}
  PRIVATE glfw ${GLFW_LIBRARIES}
  PRIVATE ${SOIL_LIBRARY}
  PRIVATE assimp)
