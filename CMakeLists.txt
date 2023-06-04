
project(lbp)

add_executable(lbp 
    main.cpp mem.cpp input.cpp
    renderer/renderer.cpp renderer/mesh.cpp renderer/shader.cpp renderer/texture.cpp
    polygon/polygon.cpp polygon/geoutil.cpp
    audio/audio.cpp
    level.cpp block.cpp material.cpp player.cpp)

add_subdirectory(lib/glfw)
target_link_libraries(lbp glfw)

add_subdirectory(lib/glew)
target_link_libraries(lbp libglew_static)

add_subdirectory(lib/glm)
target_link_libraries(lbp glm)

add_subdirectory(lib/box2d)
target_link_libraries(lbp Box2D-cmake)

if(LINUX)
    target_link_libraries(lbp pthread m dl)
endif()
