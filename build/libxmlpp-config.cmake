########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(libxmlpp_FIND_QUIETLY)
    set(libxmlpp_MESSAGE_MODE VERBOSE)
else()
    set(libxmlpp_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/libxmlppTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${libxmlpp_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(libxmlpp_VERSION_STRING "5.2.0")
set(libxmlpp_INCLUDE_DIRS ${libxmlpp_INCLUDE_DIRS_DEBUG} )
set(libxmlpp_INCLUDE_DIR ${libxmlpp_INCLUDE_DIRS_DEBUG} )
set(libxmlpp_LIBRARIES ${libxmlpp_LIBRARIES_DEBUG} )
set(libxmlpp_DEFINITIONS ${libxmlpp_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libxmlpp_BUILD_MODULES_PATHS_DEBUG} )
    message(${libxmlpp_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


