# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libffi_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libffi_FRAMEWORKS_FOUND_DEBUG "${libffi_FRAMEWORKS_DEBUG}" "${libffi_FRAMEWORK_DIRS_DEBUG}")

set(libffi_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libffi_DEPS_TARGET)
    add_library(libffi_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libffi_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libffi_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${libffi_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libffi_DEPS_TARGET to all of them
conan_package_library_targets("${libffi_LIBS_DEBUG}"    # libraries
                              "${libffi_LIB_DIRS_DEBUG}" # package_libdir
                              "${libffi_BIN_DIRS_DEBUG}" # package_bindir
                              "${libffi_LIBRARY_TYPE_DEBUG}"
                              "${libffi_IS_HOST_WINDOWS_DEBUG}"
                              libffi_DEPS_TARGET
                              libffi_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "libffi"    # package_name
                              "${libffi_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libffi_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${libffi_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${libffi_LIBRARIES_TARGETS}>
                 )

    if("${libffi_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libffi::libffi
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libffi_DEPS_TARGET)
    endif()

    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${libffi_LINKER_FLAGS_DEBUG}>)
    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${libffi_INCLUDE_DIRS_DEBUG}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${libffi_LIB_DIRS_DEBUG}>)
    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${libffi_COMPILE_DEFINITIONS_DEBUG}>)
    set_property(TARGET libffi::libffi
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${libffi_COMPILE_OPTIONS_DEBUG}>)

########## For the modules (FindXXX)
set(libffi_LIBRARIES_DEBUG libffi::libffi)
