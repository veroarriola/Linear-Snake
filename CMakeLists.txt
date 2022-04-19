cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_BUILD_TYPE Debug)


# -+- Project

project(LinearSnake C CXX)



# Extra flags
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()


# -+- Sources
set(INCLUDE_DIRECTORIES include)
set(SRCS src/exceptions
         src/perception
         src/geometry
         src/cvshortcuts)

# -*- Dependencies

# Custum directories
set(OpenCV_DIR "/home/blackzafiro/Descargas/Desarrollo/OpenCV/opencv-3.2.0/build/share/OpenCV")

#
# -*- OpenCV
#
find_package(OpenCV 3.2 REQUIRED COMPONENTS highgui cudacodec cudaimgproc cudaarithm)


# -+- Indicate where include files are
include_directories("include"
                    ${OpenCV_INCLUDE_DIRS})

# -+- Indicate where include libraries are
link_directories(${OpenCV_INSTALL_PATH}/lib)

# -+- Define object files
file(GLOB exceptions_srcs       src/exceptions/*.cpp)
file(GLOB cvshortcuts_srcs      src/cvshortcuts/*.cpp)
file(GLOB geometry_srcs         src/geometry/*.cpp)
file(GLOB perception_srcs       src/perception/*.cpp)
add_library(Exceptions  OBJECT  ${exceptions_srcs})
add_library(CVShortcuts OBJECT  ${cvshortcuts_srcs})
add_library(Geometry    OBJECT  ${geometry_srcs})
add_library(Perception  OBJECT  ${perception_srcs})

# -+- Include executables
add_subdirectory(apps)

