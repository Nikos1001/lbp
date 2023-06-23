
project(lbp)

add_executable(lbp 
    main.cpp common/mem.cpp input.cpp common/util.cpp
    renderer/renderer.cpp renderer/mesh.cpp renderer/shader.cpp renderer/texture.cpp renderer/framebuffer.cpp
    polygon/polygon.cpp polygon/geoutil.cpp
    audio/audio.cpp
    assets.cpp
    level.cpp editor.cpp block.cpp bolt.cpp material.cpp player.cpp
    lib/imgui/imgui.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/backends/imgui_impl_glfw.cpp lib/imgui/backends/imgui_impl_opengl3.cpp lib/imgui/imgui_demo.cpp)

target_include_directories(lbp PUBLIC lib/imgui)

add_subdirectory(lib/glfw)
target_link_libraries(lbp glfw)

add_subdirectory(lib/glew)
target_link_libraries(lbp libglew_static)

add_subdirectory(lib/glm)
target_link_libraries(lbp glm)

add_subdirectory(lib/box2d)
target_link_libraries(lbp Box2D-cmake)

add_subdirectory(lib/assimp)
target_link_libraries(lbp assimp)

if(LINUX)
    target_link_libraries(lbp pthread m dl)
endif()

target_link_options(lbp PUBLIC -fsanitize=address)
