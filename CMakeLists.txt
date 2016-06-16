cmake_minimum_required (VERSION 2.8)
project ( heatseeker )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++11 -Werror -Wfatal-errors")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/lib/cmake/)

find_package( OpenCV REQUIRED )
find_package( raspicam REQUIRED )

link_directories(
    /opt/vc/lib
    /usr/include
    /usr/lib/arm-linux-gnueabihf
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

set(USB
    libusb-1.0.so
)

file(GLOB src *h *.cpp)

add_executable(${PROJECT_NAME}
    main.cpp
    regular_camera.cpp
    thermal.cpp
    thermal_frame.cpp
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
    ${USB}
    ${raspicam_CV_LIBS}
    common
)
