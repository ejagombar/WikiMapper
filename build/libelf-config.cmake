########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(libelf_FIND_QUIETLY)
    set(libelf_MESSAGE_MODE VERBOSE)
else()
    set(libelf_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/libelfTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${libelf_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(libelf_VERSION_STRING "0.8.13")
set(libelf_INCLUDE_DIRS ${libelf_INCLUDE_DIRS_DEBUG} )
set(libelf_INCLUDE_DIR ${libelf_INCLUDE_DIRS_DEBUG} )
set(libelf_LIBRARIES ${libelf_LIBRARIES_DEBUG} )
set(libelf_DEFINITIONS ${libelf_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libelf_BUILD_MODULES_PATHS_DEBUG} )
    message(${libelf_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


