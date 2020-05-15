cmake_minimum_required(VERSION 2.4.0)

# Set the plugin name to build
project(simple-python)

# Supported options:
# -DFOGLAMP_INCLUDE
# -DFOGLAMP_LIB
# -DFOGLAMP_SRC
# -DFOGLAMP_INSTALL
#
# If no -D options are given and FOGLAMP_ROOT environment variable is set
# then FogLAMP libraries and header files are pulled from FOGLAMP_ROOT path.

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

# Set plugin type (south, north, filter)
set(PLUGIN_TYPE "filter")

# Fetch OS RELEASE CODENAME (i.e stretch, buster, xenial)
execute_process(COMMAND lsb_release -cs
    OUTPUT_VARIABLE RELEASE_CODENAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Generation version header file
set_source_files_properties(version.h PROPERTIES GENERATED TRUE)
add_custom_command(
  OUTPUT version.h
  DEPENDS ${CMAKE_SOURCE_DIR}/VERSION
  COMMAND ${CMAKE_SOURCE_DIR}/mkversion ${CMAKE_SOURCE_DIR}
  COMMENT "Generating version header"
  VERBATIM
)
include_directories(${CMAKE_BINARY_DIR})

# Add here all needed FogLAMP libraries as list
set(NEEDED_FOGLAMP_LIBS common-lib plugins-common-lib filters-common-lib services-common-lib)

# Find source files
file(GLOB SOURCES *.cpp)

# Find python3.x dev/lib package
find_package(PkgConfig REQUIRED)
pkg_check_modules(PYTHON REQUIRED python3)

# Find FogLAMP includes and libs, by including FindFogLAMP.cmak file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(FogLAMP)
# If errors: make clean and remove Makefile
if (NOT FOGLAMP_FOUND)
	if (EXISTS "${CMAKE_BINARY_DIR}/Makefile")
		execute_process(COMMAND make clean WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		file(REMOVE "${CMAKE_BINARY_DIR}/Makefile")
	endif()
	# Stop the build process
	message(FATAL_ERROR "FogLAMP plugin '${PROJECT_NAME}' build error.")
endif()
# On success, FOGLAMP_INCLUDE_DIRS and FOGLAMP_LIB_DIRS variables are set 

# Add ./include
include_directories(include)

# Add FogLAMP include dir(s)
include_directories(${FOGLAMP_INCLUDE_DIRS})

# Add other include paths this plugin needs
if (FOGLAMP_SRC)
	message(STATUS "Using third-party includes " ${FOGLAMP_SRC}/C/thirdparty/Simple-Web-Server)
	include_directories(${FOGLAMP_SRC}/C/thirdparty/Simple-Web-Server)
else()
	include_directories(${FOGLAMP_INCLUDE_DIRS}/Simple-Web-Server)
endif()

# Add Python 3.x header files
include_directories(${PYTHON_INCLUDE_DIRS})

# Pass RELEASE_CODENAME to C++ build for "buster" only
if (RELEASE_CODENAME)
	if (${RELEASE_CODENAME} MATCHES "buster")
		add_definitions(-DPLUGIN_PYTHON_SHARED_LIBRARY=lib${PYTHON_LIBRARIES}.so)
		message(STATUS "Setting -DPLUGIN_PYTHON_SHARED_LIBRARY=lib${PYTHON_LIBRARIES}.so for ${CMAKE_SYSTEM} on ${RELEASE_CODENAME}")
	endif()
endif()

link_directories(${PYTHON_LIBRARY_DIRS})

# Add FogLAMP lib path
link_directories(${FOGLAMP_LIB_DIRS})

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES} version.h)

# Add FogLAMP library names
target_link_libraries(${PROJECT_NAME} ${NEEDED_FOGLAMP_LIBS})

# Add additional libraries
# Add Python 3.x library
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})

# Set the build version 
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

set(FOGLAMP_INSTALL "" CACHE INTERNAL "")
# Install library
if (FOGLAMP_INSTALL)
	message(STATUS "Installing ${PROJECT_NAME} in ${FOGLAMP_INSTALL}/plugins/${PLUGIN_TYPE}/${PROJECT_NAME}")
	install(TARGETS ${PROJECT_NAME} DESTINATION ${FOGLAMP_INSTALL}/plugins/${PLUGIN_TYPE}/${PROJECT_NAME})
endif()
