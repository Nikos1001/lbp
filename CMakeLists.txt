
project(lbp)

add_executable(lbp 
    main.cpp common/mem.cpp input.cpp common/util.cpp
    renderer/renderer.cpp renderer/mesh.cpp renderer/shader.cpp renderer/texture.cpp renderer/framebuffer.cpp
    polygon/polygon.cpp polygon/geoutil.cpp
    audio/audio.cpp
    assets.cpp
    level.cpp block.cpp bolt.cpp material.cpp player.cpp)

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

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -fsanitize=undefined -fsanitize=address")
#target_link_options(lbp BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address)
