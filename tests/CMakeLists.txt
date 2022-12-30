
file(GLOB TEST_SOURCES *.c)
add_executable(all_tests ${TEST_SOURCES})


get_target_property(MINGL_LINK_LIBRARIES mingl LINK_LIBRARIES)
get_target_property(MINGL_SOURCE_DIR mingl SOURCE_DIR)
get_target_property(MINGL_SOURCES mingl SOURCES)
remove (MINGL_SOURCES "${MINGL_SOURCE_DIR}/main.c")
remove (MINGL_SOURCES "main.c")

get_target_property(MINGL_INCLUDE_DIRECTORIES mingl INCLUDE_DIRECTORIES)
target_include_directories(all_tests PUBLIC
    ${MINGL_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_sources(all_tests PRIVATE ${MINGL_SOURCES})
target_link_libraries(all_tests PUBLIC ${MINGL_LINK_LIBRARIES})

add_test(tests all_tests)
