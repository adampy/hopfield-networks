cmake_minimum_required(VERSION 3.20)
project(hopfield_networks LANGUAGES C CXX)

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

# ---- App setup here ----

add_executable(hopfield_networks
  # Root level
  main.cpp
  config.hpp

  # Helpers
  helpers/file_helpers.hpp
  helpers/math_helpers.hpp
  helpers/opengl_helpers.hpp

  # Hopfield
  hopfield/hopfield_network.hpp

  # UI widgets
  ui/hopfield_widget.cpp
  ui/hopfield_widget.hpp

  # imgui
  imgui/imconfig.h
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_impl_glfw.cpp
  imgui/imgui_impl_glfw.h
  imgui/imgui_impl_opengl3.cpp
  imgui/imgui_impl_opengl3.h
  imgui/imgui_internal.h
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui.cpp
  imgui/imgui.h
  imgui/imstb_rectpack.h
  imgui/imstb_textedit.h
  imgui/imstb_truetype.h
)

target_include_directories(hopfield_networks PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/ui
  ${CMAKE_SOURCE_DIR}/hopfield
  ${CMAKE_SOURCE_DIR}/helpers
  ${CMAKE_SOURCE_DIR}/imgui
)

# ----- More setup below -----

# glad provides target glad, glfw provides glfw
target_link_libraries(hopfield_networks PRIVATE glfw glad)

# On some Linux distros you may need explicit link to dl
if(UNIX AND NOT APPLE)
  target_link_libraries(hopfield_networks PRIVATE dl)
endif()