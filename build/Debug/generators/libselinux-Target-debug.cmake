# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libselinux_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libselinux_FRAMEWORKS_FOUND_DEBUG "${libselinux_FRAMEWORKS_DEBUG}" "${libselinux_FRAMEWORK_DIRS_DEBUG}")

set(libselinux_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libselinux_DEPS_TARGET)
    add_library(libselinux_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libselinux_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libselinux_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${libselinux_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:libselinux::sepol;pcre2::pcre2>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libselinux_DEPS_TARGET to all of them
conan_package_library_targets("${libselinux_LIBS_DEBUG}"    # libraries
                              "${libselinux_LIB_DIRS_DEBUG}" # package_libdir
                              "${libselinux_BIN_DIRS_DEBUG}" # package_bindir
                              "${libselinux_LIBRARY_TYPE_DEBUG}"
                              "${libselinux_IS_HOST_WINDOWS_DEBUG}"
                              libselinux_DEPS_TARGET
                              libselinux_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "libselinux"    # package_name
                              "${libselinux_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libselinux_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT libselinux::selinux #############

        set(libselinux_libselinux_selinux_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(libselinux_libselinux_selinux_FRAMEWORKS_FOUND_DEBUG "${libselinux_libselinux_selinux_FRAMEWORKS_DEBUG}" "${libselinux_libselinux_selinux_FRAMEWORK_DIRS_DEBUG}")

        set(libselinux_libselinux_selinux_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET libselinux_libselinux_selinux_DEPS_TARGET)
            add_library(libselinux_libselinux_selinux_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET libselinux_libselinux_selinux_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'libselinux_libselinux_selinux_DEPS_TARGET' to all of them
        conan_package_library_targets("${libselinux_libselinux_selinux_LIBS_DEBUG}"
                              "${libselinux_libselinux_selinux_LIB_DIRS_DEBUG}"
                              "${libselinux_libselinux_selinux_BIN_DIRS_DEBUG}" # package_bindir
                              "${libselinux_libselinux_selinux_LIBRARY_TYPE_DEBUG}"
                              "${libselinux_libselinux_selinux_IS_HOST_WINDOWS_DEBUG}"
                              libselinux_libselinux_selinux_DEPS_TARGET
                              libselinux_libselinux_selinux_LIBRARIES_TARGETS
                              "_DEBUG"
                              "libselinux_libselinux_selinux"
                              "${libselinux_libselinux_selinux_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET libselinux::selinux
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_LIBRARIES_TARGETS}>
                     )

        if("${libselinux_libselinux_selinux_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET libselinux::selinux
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         libselinux_libselinux_selinux_DEPS_TARGET)
        endif()

        set_property(TARGET libselinux::selinux APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET libselinux::selinux APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET libselinux::selinux APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_LIB_DIRS_DEBUG}>)
        set_property(TARGET libselinux::selinux APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET libselinux::selinux APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_selinux_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT libselinux::sepol #############

        set(libselinux_libselinux_sepol_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(libselinux_libselinux_sepol_FRAMEWORKS_FOUND_DEBUG "${libselinux_libselinux_sepol_FRAMEWORKS_DEBUG}" "${libselinux_libselinux_sepol_FRAMEWORK_DIRS_DEBUG}")

        set(libselinux_libselinux_sepol_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET libselinux_libselinux_sepol_DEPS_TARGET)
            add_library(libselinux_libselinux_sepol_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET libselinux_libselinux_sepol_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'libselinux_libselinux_sepol_DEPS_TARGET' to all of them
        conan_package_library_targets("${libselinux_libselinux_sepol_LIBS_DEBUG}"
                              "${libselinux_libselinux_sepol_LIB_DIRS_DEBUG}"
                              "${libselinux_libselinux_sepol_BIN_DIRS_DEBUG}" # package_bindir
                              "${libselinux_libselinux_sepol_LIBRARY_TYPE_DEBUG}"
                              "${libselinux_libselinux_sepol_IS_HOST_WINDOWS_DEBUG}"
                              libselinux_libselinux_sepol_DEPS_TARGET
                              libselinux_libselinux_sepol_LIBRARIES_TARGETS
                              "_DEBUG"
                              "libselinux_libselinux_sepol"
                              "${libselinux_libselinux_sepol_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET libselinux::sepol
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_LIBRARIES_TARGETS}>
                     )

        if("${libselinux_libselinux_sepol_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET libselinux::sepol
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         libselinux_libselinux_sepol_DEPS_TARGET)
        endif()

        set_property(TARGET libselinux::sepol APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET libselinux::sepol APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET libselinux::sepol APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_LIB_DIRS_DEBUG}>)
        set_property(TARGET libselinux::sepol APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET libselinux::sepol APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${libselinux_libselinux_sepol_COMPILE_OPTIONS_DEBUG}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET libselinux::libselinux APPEND PROPERTY INTERFACE_LINK_LIBRARIES libselinux::selinux)
    set_property(TARGET libselinux::libselinux APPEND PROPERTY INTERFACE_LINK_LIBRARIES libselinux::sepol)

########## For the modules (FindXXX)
set(libselinux_LIBRARIES_DEBUG libselinux::libselinux)
