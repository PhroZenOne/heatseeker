# CMake entry point
cmake_minimum_required (VERSION 2.8)
project (HeatSeeker)

find_package(OpenGL REQUIRED)

add_subdirectory (external)

cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)

include_directories(
	external/AntTweakBar-1.16/include/
	external/glm-0.9.7.1/
	.
)

add_executable(
	playground 
	playground/playground.cpp
	common/shader.cpp
	common/shader.hpp
)

target_link_libraries(
	playground
	${OPENGL_LIBRARY}
	glfw
	GLEW
)

create_target_launcher(playground WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/playground/")

add_custom_command(
	TARGET playground POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/playground${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/playground/"
)
