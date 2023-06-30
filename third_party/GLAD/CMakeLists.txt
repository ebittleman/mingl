
add_library(GLAD2 src/gl.c)

target_include_directories(GLAD2 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
