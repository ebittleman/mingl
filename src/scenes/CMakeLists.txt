
FILE(GLOB SCENE_SOURCES *.c)

target_include_directories(mingl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(mingl PRIVATE ${SCENE_SOURCES})
