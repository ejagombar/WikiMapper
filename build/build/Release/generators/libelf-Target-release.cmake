# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libelf_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libelf_FRAMEWORKS_FOUND_RELEASE "${libelf_FRAMEWORKS_RELEASE}" "${libelf_FRAMEWORK_DIRS_RELEASE}")

set(libelf_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libelf_DEPS_TARGET)
    add_library(libelf_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libelf_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libelf_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${libelf_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libelf_DEPS_TARGET to all of them
conan_package_library_targets("${libelf_LIBS_RELEASE}"    # libraries
                              "${libelf_LIB_DIRS_RELEASE}" # package_libdir
                              "${libelf_BIN_DIRS_RELEASE}" # package_bindir
                              "${libelf_LIBRARY_TYPE_RELEASE}"
                              "${libelf_IS_HOST_WINDOWS_RELEASE}"
                              libelf_DEPS_TARGET
                              libelf_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "libelf"    # package_name
                              "${libelf_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libelf_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${libelf_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${libelf_LIBRARIES_TARGETS}>
                 )

    if("${libelf_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libelf::libelf
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libelf_DEPS_TARGET)
    endif()

    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${libelf_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${libelf_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${libelf_LIB_DIRS_RELEASE}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${libelf_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${libelf_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(libelf_LIBRARIES_RELEASE libelf::libelf)
