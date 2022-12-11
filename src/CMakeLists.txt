add_executable(mingl main.c)

target_sources(mingl PRIVATE
    camera.h camera.c
    data_structures.h data_structures.c
    gl.c
    linmath.h
    obj3d.h obj3d.c
    shaders.h shaders.c
    shapes.c
    types.h
    uv.h
)

target_include_directories(mingl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/GLAD/include)
target_link_libraries(mingl PRIVATE opengl32 gdi32 m glfw3)
