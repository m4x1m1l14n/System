set(project_name "UnitTest")

set(CMAKE_SHARED_LINKER_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")

file (GLOB _include "*.h*")
file (GLOB _src "*.c*")

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
#######################################
add_library (${project_name} SHARED
                ${_include}
                ${_src})

# source filters
source_group(""				FILES ${_include} ${_src})

# Properties->C/C++->General->Additional Include Directories
include_directories (.)
include_directories ("../System/include")

add_definitions("-I$(VCInstallDir)UnitTest\\include")

#target_link_libraries(${project_name} "${PROJECT_BINARY_DIR}/bin/$(Configuration)_$(PlatformTarget)/System.lib")
target_link_libraries(${project_name} "System")