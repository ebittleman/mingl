set(GLFW_LIBRARY_TYPE STATIC)
cmake_dependent_option(GLFW_USE_HYBRID_HPG "Force use of high-performance GPU on hybrid systems" OFF
                       "WIN32" ON)

set(GLFW_USE_HYBRID_HPG ON)
set(GLFW_STANDALONE FALSE)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

add_subdirectory(third_party/glfw)
