
add_executable(mingl)

set_property(TARGET mingl PROPERTY COMPILE_WARNING_AS_ERROR ON)

FILE(GLOB SOURCES *.c)
FILE(GLOB SOURCES_HEADERS *.h)
FILE(GLOB GEOMETRY geometry/*.c)
FILE(GLOB GEOMETRY_HEADERS geometry/*.h)
FILE(GLOB SCENES scenes/*.c)
FILE(GLOB SCENES_HEADERS scenes/*.h)
FILE(GLOB SHADERS shaders/*.c)
FILE(GLOB SHADERS_HEADERS shaders/*.h)


target_include_directories(mingl PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry"
    "${CMAKE_CURRENT_SOURCE_DIR}/scenes"
    "${GLFW_SOURCE_DIR}/include"
    "${GLAD2_SOURCE_DIR}/include"
    "${mingl_SOURCE_DIR}/third_party/stb/include"
    "${mingl_SOURCE_DIR}/third_party/linmath/include"
)

target_sources(mingl PRIVATE
    ${SOURCES} ${SOURCES_HEADERS}
    ${GEOMETRY} ${GEOMETRY_HEADERS}
    ${SCENES} ${SCENES_HEADERS}
    ${SHADERS} ${SHADERS_HEADERS}
)

set(LIBS OpenGL::GL m glfw GLAD2)

if (MINGL_BUILD_WIN32)
    message(STATUS "Including Win32 support")
    list(APPEND LIBS gdi32)
endif()

target_link_libraries(mingl PUBLIC ${LIBS})
