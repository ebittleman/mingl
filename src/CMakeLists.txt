add_executable(mingl main.c)

add_subdirectory(scenes)

FILE(GLOB SOURCES *.c)

target_include_directories(mingl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(mingl PRIVATE ${SOURCES})

target_include_directories(mingl PUBLIC ${mingl_SOURCE_DIR}/third_party/GLAD/include)
target_link_libraries(mingl PUBLIC opengl32 gdi32 m glfw3)
