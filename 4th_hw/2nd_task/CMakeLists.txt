cmake_minimum_required(VERSION 3.16)
project(PointMotion VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SimulationEngine.cpp
    src/TrajectoryWidget.cpp
    src/LogStorage.cpp
    src/ConfigManager.cpp
    src/SimulationWorker.cpp
)

set(HEADERS
    include/MainWindow.h
    include/SimulationEngine.h
    include/TrajectoryWidget.h
    include/LogStorage.h
    include/ConfigManager.h
    include/SimulationWorker.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::Charts)
