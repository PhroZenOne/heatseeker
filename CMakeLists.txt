cmake_minimum_required (VERSION 2.8)
project ( heatseeker )

SET(COMPILE_DEFINITIONS -Werror)

find_package( OpenCV REQUIRED )

link_directories(
    /opt/vc/lib
    /usr/include
)

set(RPi_LIBS
# Raspberry Pi - Broadcom
    libbcm_host.so
    libvcos.so
)

set(GL_LIBS
    GLESv2
    EGL
)


file(GLOB src *h *.cpp)

add_executable(${PROJECT_NAME}
    main.cpp
    RegularCameraResource.cpp
)

file(
    COPY
    fragmentshader.glsl
    vertexshader.glsl
    DESTINATION ${CMAKE_BINARY_DIR}
)

include_directories(
    /opt/vc/include
    /opt/vc/include/interface/vcos/pthreads
    /opt/vc/include/interface/vmcs_host/linux
    ${CMAKE_SOURCE_DIR}/common
)

add_library(common
   ${CMAKE_SOURCE_DIR}/common/esUtil.c
   ${CMAKE_SOURCE_DIR}/common/esTransform.c
   ${CMAKE_SOURCE_DIR}/common/esShader.c
)

target_link_libraries(${PROJECT_NAME} 
    ${OpenCV_LIBS}
    ${RPi_LIBS}
    ${GL_LIBS}
    common
)
