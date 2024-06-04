########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(glibmm_FIND_QUIETLY)
    set(glibmm_MESSAGE_MODE VERBOSE)
else()
    set(glibmm_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/glibmmTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${glibmm_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(glibmm_VERSION_STRING "2.75.0")
set(glibmm_INCLUDE_DIRS ${glibmm_INCLUDE_DIRS_RELEASE} )
set(glibmm_INCLUDE_DIR ${glibmm_INCLUDE_DIRS_RELEASE} )
set(glibmm_LIBRARIES ${glibmm_LIBRARIES_RELEASE} )
set(glibmm_DEFINITIONS ${glibmm_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${glibmm_BUILD_MODULES_PATHS_RELEASE} )
    message(${glibmm_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


