cmake_minimum_required(VERSION 3.20)
project(glfw_template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Workaround for dependencies that declare very old cmake_minimum_required()
# which newer CMake versions reject.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# ---- Dependencies (GLFW + GLAD) ----
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.9
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLAD (OpenGL loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# ---- App ----
add_executable(glfw_template main.cpp)

# glad provides target glad, glfw provides glfw
target_link_libraries(glfw_template PRIVATE glfw glad)

# On some Linux distros you may need explicit link to dl
if(UNIX AND NOT APPLE)
  target_link_libraries(glfw_template PRIVATE dl)
endif()