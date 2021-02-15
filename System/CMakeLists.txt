set(project_name "System")

file (GLOB _include "include/System/*.h*")
file (GLOB _include_Drawing "include/System/Drawing/*.h*")
file (GLOB _include_Net "include/System/Net/*.h*")
file (GLOB _include_Net_Http "include/System/Net/Http/*.h*")
file (GLOB _include_Net_IPC "include/System/Net/IPC/*.h*")
file (GLOB _include_Net_Sockets "include/System/Net/Sockets/*.h*")
file (GLOB _include_Net_WebSockets "include/System/Net/WebSockets/*.h*")

if (MSVC)
file (GLOB _include_Text "include/System/Text/*.h*")
file (GLOB _include_Diagnostics "include/System/Diagnostics/*.h*")
file (GLOB _include_Threading "include/System/Threading/*.h*")
endif()

file (GLOB _src "src/System/*.c*")
file (GLOB _src_Net "src/System/Net/*.c*")
file (GLOB _src_Net_Http "src/System/Net/Http/*.c*")
file (GLOB _src_Net_IPC "src/System/Net/IPC/*.c*")
file (GLOB _src_Net_Sockets "src/System/Net/Sockets/*.c*")
file (GLOB _src_Net_WebSockets "src/System/Net/WebSockets/*.c*")

if (MSVC)
file (GLOB _src_Diagnostics "src/System/Diagnostics/*.c*")
file (GLOB _src_Threading "src/System/Threading/*.c*")
file (GLOB _src_Text "src/System/Text/*.c*")
endif()

if (MSVC)
file (GLOB _inc_Windows "include/Windows/*.h*")
else()
file (GLOB _inc_Linux "include/Linux/*.h*")
endif()

#
# Temporarily excludes TlsSocket class from build
#
list (FILTER _include_Net_Sockets EXCLUDE REGEX "TlsSocket")
list (FILTER _src_Net_Sockets EXCLUDE REGEX "TlsSocket")

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
#######################################
add_library (${project_name}
                ${_include}
				${_include_Diagnostics}
				${_include_Drawing}
				${_include_Net}
				${_include_Net_Http}
				${_include_Net_IPC}
				${_include_Net_Sockets}
				${_include_Net_WebSockets}
				${_include_Text}
				${_include_Threading}
				${_inc_Windows}
				${_inc_Linux}
                ${_src}
				${_src_Diagnostics}
				${_src_Text}
				${_src_Threading}
				${_src_Net}
				${_src_Net_Http}
				${_src_Net_IPC}
				${_src_Net_Sockets}
				${_src_Net_WebSockets})

# Properties->General->Output Directory
#if(WIN32)
#	set_target_properties(${project_name}
#    PROPERTIES
#        CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib
#        CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
#		CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
#	)
#endif()

#if ( MSVC )
#    set_target_properties(${project_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test )
#    set_target_properties(${project_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/test )
#    set_target_properties(${project_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/test )
#    # etc for the other available configuration types (MinSizeRel, RelWithDebInfo)
#endif ( MSVC )
				
# source filters
source_group("include\\System"					FILES ${_include})
source_group("include\\System\\Diagnostics"		FILES ${_include_Diagnostics})
source_group("include\\System\\Drawing"			FILES ${_include_Drawing})

# System::Net namespace
source_group("include\\System\\Net"				FILES ${_include_Net})
source_group("include\\System\\Net\\Http"		FILES ${_include_Net_Http})
source_group("include\\System\\Net\\IPC"		FILES ${_include_Net_IPC})
source_group("include\\System\\Net\\Sockets"	FILES ${_include_Net_Sockets})
source_group("include\\System\\Net\\WebSockets"	FILES ${_include_Net_WebSockets})

source_group("include\\System\\Text"			FILES ${_include_Text})				
source_group("include\\System\\Threading"		FILES ${_include_Threading})	

if (MSVC)
source_group("include\\Windows"					FILES ${_inc_Windows})
else()
source_group("include\\Linux"					FILES ${_inc_Linux})
endif()
			
source_group("src\\System"						FILES ${_src})				
source_group("src\\System\\Diagnostics"			FILES ${_src_Diagnostics})				
source_group("src\\System\\Text"				FILES ${_src_Text})

# System::Net namespace
source_group("src\\System\\Net"					FILES ${_src_Net})
source_group("src\\System\\Net\\Http"			FILES ${_src_Net_Http})
source_group("src\\System\\Net\\IPC"			FILES ${_src_Net_IPC})
source_group("src\\System\\Net\\Sockets"		FILES ${_src_Net_Sockets})
source_group("src\\System\\Net\\WebSockets"		FILES ${_src_Net_WebSockets})

source_group("src\\System\\Threading"			FILES ${_src_Threading})				

# Properties->C/C++->General->Additional Include Directories
include_directories (.)
include_directories ("include")

# add preprocessor definitions
add_definitions("-D_LIB")