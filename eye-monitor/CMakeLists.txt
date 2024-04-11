cmake_minimum_required(VERSION 3.6)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

project(EyeDetect
	DESCRIPTION "Detect eyes"
	LANGUAGES CXX)



set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "-Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Wno-unused-parameter")

# For eye detection
file(GLOB HAAR_CASCADES haarcascades/*.xml)
file(GLOB LBP_CASCADES lbpcascades/*.xml)

install(FILES ${HAAR_CASCADES} DESTINATION ${OPENCV_OTHER_INSTALL_PATH}/haarcascades COMPONENT libs)
install(FILES ${LBP_CASCADES}  DESTINATION ${OPENCV_OTHER_INSTALL_PATH}/lbpcascades  COMPONENT libs)

if(INSTALL_TESTS AND OPENCV_TEST_DATA_PATH)
  install(DIRECTORY "${OPENCV_TEST_DATA_PATH}/" DESTINATION "${OPENCV_TEST_DATA_INSTALL_PATH}" COMPONENT "tests")
endif()

# For playing sound
find_package(ALSA REQUIRED)

# For GPIO
#find_package(pigpio REQUIRED)


include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/.. ${LIBCAMERA_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIRS})
include(GNUInstallDirs)

# Specify path to pigpio library and include directory
set(PIGPIO_LIBRARY "/home/rpi/pigpio-master/libpigpio.so")
set(PIGPIO_INCLUDE_DIR "/home/rpi/pigpio-master")
set(pigpio_DIR "/usr/include/pigpio-master/cmake")
#set(PIGPIO_INCLUDE_DIR "/usr/include/pigpio-master")

add_executable(eye
  eye.cpp
)


target_link_libraries(eye PkgConfig::LIBCAMERA)
target_link_libraries(eye ${OpenCV_LIBS})
target_link_libraries(eye cam2opencv)

# Include directories for the pigpio library
target_include_directories(eye PRIVATE ${PIGPIO_INCLUDE_DIR})
target_link_libraries(eye ${PIGPIO_LIBRARY})

target_link_libraries(eye ${ALSA_LIBRARY})
target_include_directories(eye PRIVATE ${ALSA_INCLUDE_DIRS})

#set_property(TARGET eye PROPERTY CXX_STANDARD 11)
