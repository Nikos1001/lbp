
project(lbp)

set(CMAKE_C_FLAGS_INIT "-fsanitize=undefined")
set(CMAKE_CXX_FLAGS_INIT "-fsanitize=undefined")

add_executable(lbp 
    main.cpp mem.cpp input.cpp 
    renderer/renderer.cpp renderer/mesh.cpp renderer/shader.cpp renderer/texture.cpp
    polygon/polygon.cpp polygon/geoutil.cpp
    block.cpp player.cpp)

add_subdirectory(lib/glfw)
target_link_libraries(lbp glfw)

add_subdirectory(lib/glew)
target_link_libraries(lbp libglew_static)

add_subdirectory(lib/glm)
target_link_libraries(lbp glm)

add_subdirectory(lib/box2d)
target_link_libraries(lbp Box2D-cmake)
