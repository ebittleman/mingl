add_executable(all_tests main_test.c)


file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c)
file(GLOB TEST_SOURCES *.c)

remove (SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/main.c)
remove (TEST_SOURCES main_test.c)

target_sources(all_tests PRIVATE ${SOURCES}  ${TEST_SOURCES})
target_include_directories(all_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_include_directories(all_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/GLAD/include)
target_link_libraries(all_tests PUBLIC opengl32 gdi32 m glfw3)

add_test(tests all_tests)
