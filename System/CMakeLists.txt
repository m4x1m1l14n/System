set(project_name "System")

file (GLOB _include "include/System/*.h*")
file (GLOB _include_Diagnostics "include/System/Diagnostics/*.h*")
file (GLOB _include_Drawing "include/System/Drawing/*.h*")
file (GLOB _include_Net "include/System/Net/*.h*")
file (GLOB _include_Text "include/System/Text/*.h*")
file (GLOB _include_Threading "include/System/Threading/*.h*")

file (GLOB _src "src/System/*.c*")
file (GLOB _src_Diagnostics "src/System/Diagnostics/*.c*")
file (GLOB _src_Text "src/System/Text/*.c*")
file (GLOB _src_Threading "src/System/Threading/*.c*")

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
#######################################
add_library (${project_name}
                ${_include}
				${_include_Diagnostics}
				${_include_Drawing}
				${_include_Net}
				${_include_Text}
				${_include_Threading}
                ${_src}
				${_src_Diagnostics}
				${_src_Text}
				${_src_Threading})

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
source_group("include\\System"				FILES ${_include})
source_group("include\\System\\Diagnostics"	FILES ${_include_Diagnostics})				
source_group("include\\System\\Drawing"		FILES ${_include_Drawing})				
source_group("include\\System\\Net"			FILES ${_include_Net})				
source_group("include\\System\\Text"		FILES ${_include_Text})				
source_group("include\\System\\Threading"	FILES ${_include_Threading})	
			
source_group("src\\System"				FILES ${_src})				
source_group("src\\System\\Diagnostics"	FILES ${_src_Diagnostics})				
source_group("src\\System\\Text"		FILES ${_src_Text})				
source_group("src\\System\\Threading"	FILES ${_src_Threading})				

# Properties->C/C++->General->Additional Include Directories
include_directories (.)
include_directories ("include")

# add preprocessor definitions
add_definitions("-D_LIB")