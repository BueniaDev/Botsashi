project(simsashi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SIMSASHI_HEADERS
	simsashi.h)

set(SIMSASHI_SOURCES
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_MAIN_HANDLED")

add_executable(simsashi ${SIMSASHI_SOURCES} ${SIMSASHI_HEADERS})
include_directories(simsashi ${BOTSASHI_INCLUDE_DIR})
target_link_libraries(simsashi botsashi)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (TARGET SDL2::SDL2)
    target_link_libraries(simsashi SDL2::SDL2)
else()
    target_link_libraries(simsashi ${SDL2_LIBRARIES})
endif()

if (APPLE)
    set_target_properties(simsashi PROPERTIES
                         MACOSX_BUNDLE true
                         MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/botsashi-frontend.plist.in
                         OUTPUT_NAME simsashi)
endif()

