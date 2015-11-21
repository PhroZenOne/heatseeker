# CMake entry point
cmake_minimum_required (VERSION 2.8)
project (HeatSeeker)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/modules")
set(GLFW_DIR "external/glfw-3.1.2")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
add_subdirectory (external)
add_subdirectory (${GLFW_DIR})
add_definitions(-DGLEW_STATIC=1)

include_directories(
	${GLEW_INCLUDE_DIR}
	${GLFW_DIR}/include
	external/AntTweakBar-1.16/include/
	external/glm-0.9.7.1/
	.
)

add_executable(
	playground 
	playground/playground.cpp
	playground/GlInit.cpp
	playground/GlUtil.cpp
	common/shader.cpp
	common/shader.hpp
)

target_link_libraries(
	playground
	${OPENGL_LIBRARY}
	glfw 
	${GLFW_LIBRARIES}
	${GLEW_LIBRARY}
)

# Copy vertex shader to output dir
add_custom_command(
	TARGET playground POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/playground/SimpleFragmentShader.fragmentshader" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/SimpleFragmentShader.fragmentshader" 
)

# Copy fragment shader to output dir
add_custom_command(
	TARGET playground POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/playground/SimpleVertexShader.vertexshader" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/SimpleVertexShader.vertexshader" 
)
