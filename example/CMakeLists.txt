project(example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXAMPLE_SOURCES
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(example ${EXAMPLE_SOURCES})
include_directories(example ${BOTSASHI_INCLUDE_DIR})
target_link_libraries(example botsashi)

if (APPLE)
    set_target_properties(example PROPERTIES
                         MACOSX_BUNDLE true
                         MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/botsashi-frontend.plist.in
                         OUTPUT_NAME example)
endif()

