########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(sigc++-3_FIND_QUIETLY)
    set(sigc++-3_MESSAGE_MODE VERBOSE)
else()
    set(sigc++-3_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sigc++-3Targets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${libsigcpp_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(sigc++-3_VERSION_STRING "3.0.7")
set(sigc++-3_INCLUDE_DIRS ${libsigcpp_INCLUDE_DIRS_RELEASE} )
set(sigc++-3_INCLUDE_DIR ${libsigcpp_INCLUDE_DIRS_RELEASE} )
set(sigc++-3_LIBRARIES ${libsigcpp_LIBRARIES_RELEASE} )
set(sigc++-3_DEFINITIONS ${libsigcpp_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libsigcpp_BUILD_MODULES_PATHS_RELEASE} )
    message(${sigc++-3_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


