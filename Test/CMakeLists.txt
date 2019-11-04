set(project_name "Test")

file (GLOB _src "*.c*")

# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cpp)
# Adds sources to the Solution Explorer
#######################################
add_executable (${project_name}
                ${_src})

# source filters
source_group(""				FILES ${_src})				

# Properties->C/C++->General->Additional Include Directories
include_directories (.)
include_directories ("../System/include")


target_link_libraries(${project_name} "System")