cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 14)

project (System)

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
)
message(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})

add_definitions(-DNOMINMAX)

if (MSVC)
    # set basic VS flags
    set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /Gd /GR /GS /EHsc /Wall /EHa /nologo")
    
    # disable optimization for debug
    # /W4 - setting the Visual Studio warning level to 4
    set(CMAKE_CXX_FLAGS_DEBUG " /Gd /RTC1 /ZI /Od /MDd /Fd /WX- /fp:precise /W3")
    
    # /O2 - Maximize Speed
    # /W0 - setting the Visual Studio warning level to 0
    set(CMAKE_CXX_FLAGS_RELEASE "/Zi /GL /O2 /Gm- /W0")
    # optimize linker
    #set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG")
     
    # Turn on the ability to create folders to organize projects (.vcproj)
    # It creates "CMakePredefinedTargets" folder by default and adds CMake
    # defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
     
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
	
	# set Debug and Release only allowed configuration types
    if(CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES Debug Release)
        set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
            "Reset the configurations to what we need"
        FORCE)
    endif()

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)")

ENDIF()


add_subdirectory("System")

if (MSVC)
	add_subdirectory("Test")
	add_subdirectory("UnitTest")
ENDIF()
