set(BOTSASHI_HEADERS
	botsashi_api.h
	botsashi.h)

set(BOTSASHI_SOURCES
	botsashi.cpp)

add_library(botsashi ${BOTSASHI_SOURCES} ${BOTSASHI_HEADERS})
target_include_directories(botsashi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(libbotsashi ALIAS botsashi)