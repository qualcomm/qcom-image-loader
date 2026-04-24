cmake_minimum_required(VERSION 3.5.0)
project(QcDevice VERSION 0.1.0 LANGUAGES CXX)
if(UNIX)
	# Build QcDevice library for Linux
	set(QDS_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX/qcdev.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX/qdaio.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX/QdIoQmi.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/libLNX/liblpc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/common/qdver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/common/utils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/libusb/source/qcom-libusb.cpp
	)
	
	add_library(QcDevice STATIC ${QDS_SOURCES})
	
	# Set output name and location
	set_target_properties(QcDevice PROPERTIES
		OUTPUT_NAME "QcDevice"
	)
	
	# Include directories
	target_include_directories(QcDevice PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/qds_lnx/inc
		${CMAKE_CURRENT_SOURCE_DIR}/common
		${CMAKE_CURRENT_SOURCE_DIR}/libusb/source
	)
	
	# Find libusb-1.0
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
	target_include_directories(QcDevice PRIVATE ${LIBUSB_INCLUDE_DIRS})
	target_link_libraries(QcDevice PRIVATE ${LIBUSB_LIBRARIES})
	
	# Compiler flags (relaxed for legacy code)
	target_compile_options(QcDevice PRIVATE
		-fPIC
		-Wno-narrowing
		-Wno-write-strings
		-fpermissive
		-UWIN32       
        -U_WIN32      
	)
	
	# Preprocessor definitions
	target_compile_definitions(QcDevice PRIVATE
		QCDEVLIB_STATIC_LINK
		TOOLS_TARGET_LINUX
		QCOM_LNX_ENV
		QCDEV_LINUX
	)

elseif(WIN32)
    set(CMAKE_GENERATOR_PLATFORM "x64")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/qds_win/QdIoQmi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/qds_win/scandev.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common/qdver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/source/qcom-libusb.cpp
    )
    add_library(QcDevice STATIC ${SOURCES})

    target_include_directories(QcDevice PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/qds_win"
        "${CMAKE_CURRENT_SOURCE_DIR}/common"
        "${CMAKE_CURRENT_SOURCE_DIR}/libusb/source"
        "${CMAKE_CURRENT_SOURCE_DIR}/libusb/lib-1.0.27/include/libusb-1.0"
    )

    target_link_directories(QcDevice PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libusb/lib-1.0.27/lib/${CMAKE_VS_PLATFORM_NAME}/Release"
    )

    target_link_libraries(QcDevice PUBLIC libusb-1.0)

    target_compile_definitions(QcDevice PRIVATE
        QCDEVLIB_STATIC_LINK
        TOOLS_TARGET_WINDOWS
        QCOM_WIN_ENV
    )
endif()
