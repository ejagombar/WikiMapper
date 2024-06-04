########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(libmount_FIND_QUIETLY)
    set(libmount_MESSAGE_MODE VERBOSE)
else()
    set(libmount_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/libmountTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${libmount_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(libmount_VERSION_STRING "2.39")
set(libmount_INCLUDE_DIRS ${libmount_INCLUDE_DIRS_RELEASE} )
set(libmount_INCLUDE_DIR ${libmount_INCLUDE_DIRS_RELEASE} )
set(libmount_LIBRARIES ${libmount_LIBRARIES_RELEASE} )
set(libmount_DEFINITIONS ${libmount_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libmount_BUILD_MODULES_PATHS_RELEASE} )
    message(${libmount_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


