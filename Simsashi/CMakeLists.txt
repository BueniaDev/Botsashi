project(simsashi)

message(STATUS "Building Simsashi...")
message(WARNING "Simsashi is still a huge WIP, so tread lightly here!")
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia MultimediaWidgets REQUIRED)
set(QT_LINK_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SIMSASHI_SOURCES
	qsimsashi.h
	qsimsashi.cpp
	main.cpp)

add_executable(simsashi ${SIMSASHI_SOURCES})
include_directories(simsashi ${BOTSASHI_INCLUDE_DIR})
target_link_libraries(simsashi botsashi ${QT_LINK_LIBS})