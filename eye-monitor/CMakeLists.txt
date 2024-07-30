cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

#for multi threading
find_package(Threads REQUIRED)

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

target_link_libraries (eye ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(eye PkgConfig::LIBCAMERA)
target_link_libraries(eye ${OpenCV_LIBS})
target_link_libraries(eye cam2opencv)

# Include directories for the pigpio library
target_include_directories(eye PRIVATE ${PIGPIO_INCLUDE_DIR})
target_link_libraries(eye ${PIGPIO_LIBRARY})

# Link pthreads library
target_link_libraries(eye Threads::Threads)

# Link wav sound player library
target_link_libraries(eye ${ALSA_LIBRARY})
target_include_directories(eye PRIVATE ${ALSA_INCLUDE_DIRS})

# Find Doxygen
find_package(Doxygen REQUIRED)

# Set Doxygen options
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX YES)

# Specify the input and output for Doxyfile
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Ensure output directory exists
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

# Configure the Doxyfile
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Add a custom target to run Doxygen
add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with Doxygen"
    VERBATIM
)

# Add source directories to Doxygen input
doxygen_add_docs(
    doc
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generating documentation with Doxygen"
)

# Optionally, install the documentation
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION doc)
