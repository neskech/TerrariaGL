cmake_minimum_required(VERSION 3.10)
project(terraria_time)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "-O3")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-g")
endif ()

file(GLOB_RECURSE file_sources src/*.cpp)
add_executable(terraria ${file_sources})
include_directories(terraria include)

target_precompile_headers(terraria PRIVATE include/pch.h)

# glfw
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)

# GLAD
add_subdirectory(libs/Glad EXCLUDE_FROM_ALL)

# glm
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)

# imgui
add_library(
        imgui EXCLUDE_FROM_ALL
        libs/imgui/imgui.cpp
        libs/imgui/imgui.h
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_internal.h
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imstb_rectpack.h
        libs/imgui/imstb_textedit.h
        libs/imgui/imstb_truetype.h
        libs/imgui/imconfig.h
        libs/imgui/imgui_tables.cpp
        libs/imgui/backends/imgui_impl_glfw.h
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_opengl3.h
        libs/imgui/backends/imgui_impl_opengl3.cpp
)


include_directories(imgui libs/imgui libs/imgui/backends)
include_directories(entt libs/entt)
include_directories(Glad libs/Glad/include)



# stb_image
add_library(stb_image EXCLUDE_FROM_ALL libs/stb_image/stb_image.h libs/stb_image/stb_image.cpp)
include_directories(stb_image libs/stb_image)

#find_package(OpenGL REQUIRED)
#target_include_directories(terraria PUBLIC ${OPENGL_INCLUDE_DIR})

# link glfw to imgui and link everything to the main app
#if (APPLE)
    target_link_libraries(terraria "-framework Cocoa")
    target_link_libraries(terraria "-framework OpenGL")
    target_link_libraries(terraria "-framework IOKit")
    target_link_libraries(terraria "-framework Foundation")
    target_link_libraries(terraria "-framework CoreFoundation")
    target_link_libraries(terraria "-framework CoreVideo")
   # target_link_libraries(terraria PUBLIC "-framework CoreGraphics")
#endif (APPLE)

target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(terraria glfw glm glad imgui stb_image )

