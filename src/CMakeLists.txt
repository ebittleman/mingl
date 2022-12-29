add_executable(mingl main.c)

FILE(GLOB SOURCES *.c)
FILE(GLOB SOURCES_HEADERS *.h)
FILE(GLOB GEOMETRY geometry/*.c)
FILE(GLOB GEOMETRY_HEADERS geometry/*.h)
FILE(GLOB SCENES scenes/*.c)
FILE(GLOB SCENES_HEADERS scenes/*.h)

target_include_directories(mingl PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry"
    "${CMAKE_CURRENT_SOURCE_DIR}/scenes"
    "${GLFW_SOURCE_DIR}/include"
    "${mingl_SOURCE_DIR}/third_party/GLAD/include"
)

target_sources(mingl PRIVATE
    ${SOURCES} ${SOURCES_HEADERS}
    ${GEOMETRY} ${GEOMETRY_HEADERS}
    ${SCENES} ${SCENES_HEADERS}
)
target_link_libraries(mingl PUBLIC opengl32 gdi32 m glfw)
