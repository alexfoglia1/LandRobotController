cmake_minimum_required(VERSION 3.10)

project(LandRobotController LANGUAGES CXX CUDA)

#add_compile_definitions(__HOTSPOT__)

set(CMAKE_PREFIX_PATH "C:/Users/alex1/git/vcpkg/installed/x64-windows")
set(Qt5_DIR C:\\Qt\\5.15.2\\msvc2019_64\\lib\\cmake\\Qt5)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(LandRobotController

${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/JoysticksCommon.h

${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/Android_Joystick.h
${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/Android_Joystick.cpp

${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/SDL_Joysticks.h
${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/SDL_Joysticks.cpp

${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/VirtualJoystick.h
${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/Joysticks/VirtualJoystick.cpp

${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/QJoysticks.cpp
${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/QJoysticks.h

${CMAKE_SOURCE_DIR}/src/LandRobotControllerGui.ui

${CMAKE_SOURCE_DIR}/src/SharedMemoryStream.h
${CMAKE_SOURCE_DIR}/src/SharedMemoryStream.cpp

${CMAKE_SOURCE_DIR}/src/ControllerWindow.h
${CMAKE_SOURCE_DIR}/src/ControllerWindow.cpp

${CMAKE_SOURCE_DIR}/src/Comm.h
${CMAKE_SOURCE_DIR}/src/Comm.cpp

${CMAKE_SOURCE_DIR}/src/VideoWindow.h
${CMAKE_SOURCE_DIR}/src/VideoWindow.cpp

${CMAKE_SOURCE_DIR}/src/VideoWindowOverlay.h
${CMAKE_SOURCE_DIR}/src/VideoWindowOverlay.cpp

${CMAKE_SOURCE_DIR}/src/VideoProcessing.h
${CMAKE_SOURCE_DIR}/src/VideoProcessing.cpp

${CMAKE_SOURCE_DIR}/src/Tracker.h
${CMAKE_SOURCE_DIR}/src/Tracker.cpp

${CMAKE_SOURCE_DIR}/src/RobotData.h
${CMAKE_SOURCE_DIR}/src/RobotData.cpp

${CMAKE_SOURCE_DIR}/src/JoystickBridge.h
${CMAKE_SOURCE_DIR}/src/JoystickBridge.cpp

${CMAKE_SOURCE_DIR}/src/ServoControl.h
${CMAKE_SOURCE_DIR}/src/ServoControl.cpp

${CMAKE_SOURCE_DIR}/src/DigitalZoom.h
${CMAKE_SOURCE_DIR}/src/DigitalZoom.cu
 
${CMAKE_SOURCE_DIR}/src/main.cpp
)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Network OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc cudaoptflow video highgui videoio)
set(OpenCV_LIB_DIRS "C:\\Users\\alex1\\git\\vcpkg\\installed\\x64-windows\\lib")
set(OPENCV_CUDA_LIBS
    opencv_cudaimgproc
    opencv_cudaarithm
    opencv_cudawarping
    opencv_cudaoptflow
)
find_package(CUDA REQUIRED)


# Estrarre la versione major e minor
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" CUDA_VERSION_MATCH ${CUDA_VERSION_STRING})
set(CUDA_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CUDA_VERSION_MINOR ${CMAKE_MATCH_2})


if (SDL2_FOUND)
    target_include_directories(LandRobotController PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(LandRobotController ${SDL2_LIBRARIES})
    add_compile_definitions(SDL_SUPPORTED)
endif()

set(CUDA_LIBS_DIR "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v${CUDA_VERSION_MAJOR}.${CUDA_VERSION_MINOR}\\lib\\x64")
set(Cuda_LIBS "cuda.lib" "cudart.lib" "nppc.lib")


target_link_directories(LandRobotController PUBLIC ${CUDA_LIBS_DIR} ${OpenCV_LIB_DIRS})

target_include_directories(LandRobotController PUBLIC ${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/ ${OpenCV_INCLUDE_DIRS} ${CUDA_INCLUDE_DIRS})
target_link_libraries(LandRobotController Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::OpenGL OpenGL::GL ${OPENCV_CUDA_LIBS} ${OpenCV_LIBS} ${Cuda_LIBS})
