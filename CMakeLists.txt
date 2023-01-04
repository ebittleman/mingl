cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)
project(mingl VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CMakeDependentOption)
cmake_dependent_option(MINGL_BUILD_WIN32 "Build support for Win32" ON "WIN32" OFF)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

set(GLFW_DIR .)
find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
add_subdirectory(third_party/GLAD)
add_subdirectory(src)

# TODO: cut tests over to utest.h
add_subdirectory(tests)
