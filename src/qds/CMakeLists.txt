cmake_minimum_required(VERSION 3.5.0)
project(QcDevice VERSION 0.1.0 LANGUAGES CXX)

if(UNIX)
	execute_process(
		COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX
		OUTPUT_VARIABLE output
		ERROR_VARIABLE error
	)

	message("Build Output: ${output}")

	add_library(QcDevice SHARED IMPORTED GLOBAL)
	set_target_properties(QcDevice PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX/libqcdev.so"
	)
elseif(WIN32)
	file(GLOB_RECURSE SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/qds_win/*.*"
		"${CMAKE_CURRENT_SOURCE_DIR}/libusb/source/*.*"
		"${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/common/*.h"
	)
	add_library(QcDevice STATIC
		${SOURCES}
	)
	target_include_directories(QcDevice PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/qds_win"
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/libusb/lib-1.0.27/include"
	)
	target_compile_definitions(QcDevice PRIVATE
		TOOLS_TARGET_WINDOWS
		_UNICODE
		UNICODE
		WIN32
		NOMINMAX
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
	)
endif()