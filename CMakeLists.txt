cmake_minimum_required(VERSION 3.6)

project(LandRobotController)

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

${CMAKE_SOURCE_DIR}/src/ControllerWindow.h
${CMAKE_SOURCE_DIR}/src/ControllerWindow.cpp

${CMAKE_SOURCE_DIR}/src/Comm.h
${CMAKE_SOURCE_DIR}/src/Comm.cpp

${CMAKE_SOURCE_DIR}/src/VideoWindow.h
${CMAKE_SOURCE_DIR}/src/VideoWindow.cpp

${CMAKE_SOURCE_DIR}/src/VideoWindowRender.h
${CMAKE_SOURCE_DIR}/src/VideoWindowRender.cpp

${CMAKE_SOURCE_DIR}/src/JoystickBridge.h
${CMAKE_SOURCE_DIR}/src/JoystickBridge.cpp
 
${CMAKE_SOURCE_DIR}/src/main.cpp
)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets Network OpenGL REQUIRED)

find_package(SDL2 REQUIRED)

find_package(OpenGL REQUIRED)
set(OpenCV_ROOT "${VCPKG_INSTALLED_DIR}/x64-windows-static/share/opencv4")
find_package(OpenCV REQUIRED COMPONENTS core imgproc video highgui videoio)

if (SDL2_FOUND)
    target_include_directories(LandRobotController PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(LandRobotController ${SDL2_LIBRARIES})
    add_compile_definitions(SDL_SUPPORTED)
    message(STATUS "SDL2_FOUND: ${SDL2_FOUND}")
    message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
endif()
message(STATUS "OpenCV_FOUND: ${OpenCV_FOUND}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBRARIES: ${OpenCV_LIBRARIES}")


target_include_directories(LandRobotController PUBLIC ${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/3rdparty/QJoysticks/ ${OpenCV_INCLUDE_DIRS})
target_link_libraries(LandRobotController Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::OpenGL OpenGL::GL ${OpenCV_LIBS})
