# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libelf_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libelf_FRAMEWORKS_FOUND_DEBUG "${libelf_FRAMEWORKS_DEBUG}" "${libelf_FRAMEWORK_DIRS_DEBUG}")

set(libelf_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libelf_DEPS_TARGET)
    add_library(libelf_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libelf_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libelf_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${libelf_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libelf_DEPS_TARGET to all of them
conan_package_library_targets("${libelf_LIBS_DEBUG}"    # libraries
                              "${libelf_LIB_DIRS_DEBUG}" # package_libdir
                              "${libelf_BIN_DIRS_DEBUG}" # package_bindir
                              "${libelf_LIBRARY_TYPE_DEBUG}"
                              "${libelf_IS_HOST_WINDOWS_DEBUG}"
                              libelf_DEPS_TARGET
                              libelf_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "libelf"    # package_name
                              "${libelf_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libelf_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${libelf_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${libelf_LIBRARIES_TARGETS}>
                 )

    if("${libelf_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libelf::libelf
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libelf_DEPS_TARGET)
    endif()

    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${libelf_LINKER_FLAGS_DEBUG}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${libelf_INCLUDE_DIRS_DEBUG}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${libelf_LIB_DIRS_DEBUG}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${libelf_COMPILE_DEFINITIONS_DEBUG}>)
    set_property(TARGET libelf::libelf
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${libelf_COMPILE_OPTIONS_DEBUG}>)

########## For the modules (FindXXX)
set(libelf_LIBRARIES_DEBUG libelf::libelf)
