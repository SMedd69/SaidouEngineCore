cmake_minimum_required(VERSION 3.16)
project(SaidouEngine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(USE_OPENGL "Use OpenGL with GLFW" ON)

# Paths
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(GLFW_DIR ${CMAKE_SOURCE_DIR}/external/glfw)
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/external/glm)

# Sources
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${SOURCE_DIR}/Engine/*.cpp
    ${SOURCE_DIR}/Input/*.cpp
    ${SOURCE_DIR}/UI/*.cpp
    ${SOURCE_DIR}/main.cpp
)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add GLAD
add_library(glad ${GLAD_DIR}/src/gl.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# Add GLFW
add_subdirectory(${GLFW_DIR})

# Executable
add_executable(SaidouEngine ${ENGINE_SOURCES} ${IMGUI_SOURCES})

# Includes
target_include_directories(SaidouEngine PRIVATE
    ${INCLUDE_DIR}
    ${GLFW_DIR}/include
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLM_DIR}
)

# Definitions for ImGui
target_compile_definitions(SaidouEngine PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    IMGUI_ENABLE_VIEWPORTS
)

# Link libraries
if(WIN32)
    target_link_libraries(SaidouEngine PRIVATE glfw glad OpenGL32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(SaidouEngine PRIVATE glfw glad OpenGL::GL)
endif()