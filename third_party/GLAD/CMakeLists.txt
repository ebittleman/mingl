
add_library(GLAD2 src/gl.c)

FILE(GLOB SOURCES *.c)
FILE(GLOB SOURCES_HEADERS *.h)
FILE(GLOB GEOMETRY geometry/*.c)
FILE(GLOB GEOMETRY_HEADERS geometry/*.h)
FILE(GLOB SCENES scenes/*.c)
FILE(GLOB SCENES_HEADERS scenes/*.h)
FILE(GLOB SHADERS shaders/*.c)
FILE(GLOB SHADERS_HEADERS shaders/*.h)


target_include_directories(GLAD2 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
