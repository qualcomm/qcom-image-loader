cmake_minimum_required(VERSION 3.15)
project(SoftwareDownload VERSION 0.1.0 LANGUAGES CXX)

# Set output directories
if(WIN32)
    set(PLATFORM_NAME ${CMAKE_VS_PLATFORM_NAME})
else()
    set(PLATFORM_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE}/bin)

# ----------------------------------------------------------------------
# Global language settings – C++17 required
# ----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------------
# Platform‑specific flags (Visual Studio 2019)
# ----------------------------------------------------------------------
if (WIN32)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MDd /RTC1")

    # Enable correct C++ standard macro reporting (/Zc:__cplusplus)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")

    # Global Windows definitions (shared across all targets)
    if(PLATFORM_NAME MATCHES "ARM64")
        message(STATUS "Configuring for ARM64 architecture")
        add_definitions(-DTOOLS_TARGET_WINDOWS -DQFS_IMGMGMT -DTOOLS_ARCH_ARM64
                        -DWIN32 -DNDEBUG -D_LIB -D_UNICODE -DUNICODE -DNOMINMAX)
        set(CMAKE_GENERATOR_PLATFORM "ARM64")
    elseif(PLATFORM_NAME MATCHES "Win32")
        message(STATUS "Configuring for Win32 (x86) architecture")
        add_definitions(-DTOOLS_TARGET_WINDOWS -DQFS_IMGMGMT -DTOOLS_ARCH_32BIT
                        -DWIN32 -DNDEBUG -D_LIB -D_UNICODE -DUNICODE -DNOMINMAX)
        set(CMAKE_GENERATOR_PLATFORM "Win32")
    else()
        message(STATUS "Configuring for x64 architecture")
        add_definitions(-DTOOLS_TARGET_WINDOWS -DQFS_IMGMGMT -DTOOLS_ARCH_64BIT
                        -DWIN32 -DNDEBUG -D_LIB -D_UNICODE -DUNICODE -DNOMINMAX)
        set(CMAKE_GENERATOR_PLATFORM "x64")
    endif()
    set(CMAKE_GENERATOR "Visual Studio 16 2019")
    set(CMAKE_GENERATOR_TOOLSET "v142")
endif()

# ----------------------------------------------------------------------
# Paths to internal components
# ----------------------------------------------------------------------
set(INTERNAL_QDS_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/src/qds")
set(INTERNAL_KLOGGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external/kLogger")

set(EXTERNAL_QDS_DIR    "${INTERNAL_QDS_DIR}")
set(EXTERNAL_KLOGGER_DIR "${INTERNAL_KLOGGER_DIR}")

# ==============================================================================
# SYSTEM DEPENDENCIES
# ==============================================================================

if(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
	find_package(Threads REQUIRED)

	find_library(UDEV_LIBRARY udev)
	if(NOT UDEV_LIBRARY)
		message(FATAL_ERROR "libudev not found. Install with: sudo apt-get install libudev-dev")
	endif()

	find_library(UUID_LIBRARY uuid)
	if(NOT UUID_LIBRARY)
		message(FATAL_ERROR "libuuid not found. Install with: sudo apt-get install uuid-dev")
	endif()

	pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
elseif(WIN32)
	# Use locally built libxml2 static library
	set(LIBXML2_WIN_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external/libxml2-win-build")

	# Determine platform (x64, ARM64, or Win32)
	if(PLATFORM_NAME MATCHES "ARM64")
		set(LIBXML2_LIB_DIR "${LIBXML2_WIN_BUILD_DIR}/build/ARM64")
	elseif(PLATFORM_NAME MATCHES "Win32")
		set(LIBXML2_LIB_DIR "${LIBXML2_WIN_BUILD_DIR}/build/Win32")
	else()
		set(LIBXML2_LIB_DIR "${LIBXML2_WIN_BUILD_DIR}/build/x64")
	endif()

	# Set include directories (including iconv dependency)
	set(LIBXML2_INCLUDE_DIRS
		"${LIBXML2_WIN_BUILD_DIR}/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/external/libiconv-win-build/include"
	)

	# Set library files (static linking requires all dependencies)
	set(LIBXML2_LIBRARIES
		"${LIBXML2_LIB_DIR}/libxml2-static.lib"
		"${LIBXML2_LIB_DIR}/libiconv-static.lib"
		"${LIBXML2_LIB_DIR}/liblzma-static.lib"
#		"${LIBXML2_LIB_DIR}/libz-static.lib"
	)
    set(LIBXML2_INCLUDE_DIR "${LIBXML2_INCLUDE_DIRS}")
    set(LIBXML2_LIBRARY "${LIBXML2_LIBRARIES}")

	# LIBXML_STATIC will be added per-target below
endif()

# ==============================================================================
# INCLUDE DIRECTORY DEFINITIONS
# ==============================================================================

# Core QFS includes (for SoftwareDownload library - full access)
set(QFS_CORE_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/exports"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# SDK headers are in protected src/sdk directory (not built, header-only reference if needed)
# Note: Per CLAUDE.md, src/sdk/ is a protected directory and should not be modified

# QDS – platform specific + the “common” folder that qcom‑libusb.cpp expects
set(QDS_INCLUDES)
if (WIN32)
    list(APPEND QDS_INCLUDES "${EXTERNAL_QDS_DIR}/qds_win")
else()
    list(APPEND QDS_INCLUDES "${EXTERNAL_QDS_DIR}/qds_lnx/inc")
endif()
list(APPEND QDS_INCLUDES "${EXTERNAL_QDS_DIR}/common")
list(APPEND QDS_INCLUDES "${EXTERNAL_QDS_DIR}/libusb/source")

# ----------------------------------------------------------------------
# kLogger – header‑only INTERFACE target
# ----------------------------------------------------------------------
add_library(kLogger INTERFACE)
target_include_directories(kLogger INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/kLogger/include"
)

# Combined includes for SoftwareDownload library (full access)
set(SOFTWAREDOWNLOAD_LIB_INCLUDES
	${QFS_CORE_INCLUDES}
	${QDS_INCLUDES}
	${LIBXML2_INCLUDE_DIRS}
)

# CLI executable includes (minimal restricted access)
set(CLI_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/src/cli
	${LIBXML2_INCLUDE_DIRS}
)


# ==============================================================================
# SOURCE FILE COLLECTIONS
# ==============================================================================

# SoftwareDownload library sources (excluding CLI and TestApplication)
file(GLOB_RECURSE SOFTWAREDOWNLOAD_SOURCES "src/*.cpp" "src/*.h")
# Filter out CLI and QDS sources – we only want the SDK code
set(TEMP_SOURCES)
foreach(src ${SOFTWAREDOWNLOAD_SOURCES})
    if (NOT src MATCHES "cli|qds")
        list(APPEND TEMP_SOURCES "${src}")
    endif()
endforeach()
set(SOFTWAREDOWNLOAD_SOURCES ${TEMP_SOURCES})

file(GLOB_RECURSE CLI_SOURCES "src/cli/*.cpp")

# ----------------------------------------------------------------------
# External sub‑projects (QDS)
# ----------------------------------------------------------------------
add_subdirectory(${EXTERNAL_QDS_DIR} ${EXTERNAL_QDS_DIR})

# ----------------------------------------------------------------------
# SDK static library
# ----------------------------------------------------------------------
add_library(SoftwareDownloadLib STATIC ${SOFTWAREDOWNLOAD_SOURCES})

target_include_directories(SoftwareDownloadLib PUBLIC
    ${QFS_CORE_INCLUDES}
    ${QDS_INCLUDES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/kLogger/include"
)

set_target_properties(SoftwareDownloadLib PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    LINKER_LANGUAGE CXX
    OUTPUT_NAME "SoftwareDownloadLib"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Apply full include directories to SoftwareDownload library
target_include_directories(SoftwareDownloadLib PRIVATE ${SOFTWAREDOWNLOAD_LIB_INCLUDES})

# Platform-specific preprocessor definitions
if(WIN32)
	# Match Visual Studio project preprocessor definitions
	target_compile_definitions(SoftwareDownloadLib PRIVATE
		_HAS_STD_BYTE=0
		WIN32_LEAN_AND_MEAN
		NOMINMAX
		NOGDI
		LIBXML_STATIC
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
		_CONSOLE
	)
endif()

# Platform-specific library configuration
if(UNIX)
	# AddressSanitizer flags for Debug builds
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(SoftwareDownloadLib PRIVATE -fPIC -fpermissive -fsanitize=address -fno-omit-frame-pointer -g)
		target_link_options(SoftwareDownloadLib PRIVATE -fsanitize=address)
	else()
		target_compile_options(SoftwareDownloadLib PRIVATE -fPIC -fpermissive)
	endif()
	target_compile_definitions(SoftwareDownloadLib PRIVATE TOOLS_TARGET_LINUX)
	target_link_libraries(SoftwareDownloadLib PUBLIC
		${LIBUSB_LIBRARIES}
		${UDEV_LIBRARY}
		${UUID_LIBRARY}
		${LIBXML2_LIBRARIES}
		pthread
		uuid
		udev		
		kLogger
	)
	target_include_directories(SoftwareDownloadLib PRIVATE ${LIBUSB_INCLUDE_DIRS})
elseif(WIN32)
	target_link_libraries(SoftwareDownloadLib PUBLIC
		${LIBXML2_LIBRARIES}
		ws2_32
		"${LIBXML2_LIB_DIR}/libz-static.lib"
		"${LIBXML2_LIB_DIR}/liblzma-static.lib"
		kLogger
	)
    # target_link_options(SoftwareDownloadLib PRIVATE "/LTCG")
endif()

# ==============================================================================
# CLI EXECUTABLE TARGET
# ==============================================================================

# ----------------------------------------------------------------------
# CLI executable – original VS solution built it with C++17
# ----------------------------------------------------------------------
add_executable(qil ${CLI_SOURCES})
set_target_properties(qil PROPERTIES
    OUTPUT_NAME "qil"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(qil PRIVATE ${CLI_INCLUDES})

add_dependencies(qil SoftwareDownloadLib)

# Platform-specific preprocessor definitions for CLI
if(WIN32)
	# Match Visual Studio project preprocessor definitions
	target_compile_definitions(qil PRIVATE
		WIN32_LEAN_AND_MEAN
		NOMINMAX
		NOGDI
		LIBXML_STATIC
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
	)
    target_link_options(qil PRIVATE "-NODEFAULTLIB:libcmt.lib")
endif()

# ==============================================================================
# LINKING CONFIGURATION
# ==============================================================================

if(WIN32)
    target_link_libraries(qil PUBLIC
        SoftwareDownloadLib
        QcDevice
        ${LIBXML2_LIBRARIES}
        Wtsapi32 Userenv Rpcrt4 Setupapi Shlwapi msvcrt ws2_32
        kLogger)
    # Apply WHOLEARCHIVE flag using target_link_options
    target_link_options(qil PRIVATE
        "/WHOLEARCHIVE:${LIBXML2_LIB_DIR}/libxml2-static.lib"
        "/WHOLEARCHIVE:${LIBXML2_LIB_DIR}/libiconv-static.lib"
    )
elseif(UNIX)
    # Remove static linking of libstdc++ to avoid heap corruption with dynamic libraries
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

    # AddressSanitizer flags for Debug builds
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(qil PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
        target_link_options(qil PRIVATE -fsanitize=address)
    endif()

    target_link_libraries(qil PUBLIC
        SoftwareDownloadLib
        QcDevice
        ${LIBXML2_LIBRARIES}
        pthread dl uuid udev
        kLogger)
    target_compile_definitions(qil PRIVATE TOOLS_TARGET_LINUX)
endif()
