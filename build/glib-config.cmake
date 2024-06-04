########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(glib_FIND_QUIETLY)
    set(glib_MESSAGE_MODE VERBOSE)
else()
    set(glib_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/glibTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${glib_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(glib_VERSION_STRING "2.78.0")
set(glib_INCLUDE_DIRS ${glib_INCLUDE_DIRS_DEBUG} )
set(glib_INCLUDE_DIR ${glib_INCLUDE_DIRS_DEBUG} )
set(glib_LIBRARIES ${glib_LIBRARIES_DEBUG} )
set(glib_DEFINITIONS ${glib_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${glib_BUILD_MODULES_PATHS_DEBUG} )
    message(${glib_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


