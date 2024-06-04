# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(glibmm_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(glibmm_FRAMEWORKS_FOUND_DEBUG "${glibmm_FRAMEWORKS_DEBUG}" "${glibmm_FRAMEWORK_DIRS_DEBUG}")

set(glibmm_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET glibmm_DEPS_TARGET)
    add_library(glibmm_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET glibmm_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${glibmm_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${glibmm_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:glib::gobject-2.0;sigc-3.0;glibmm::glibmm-2.68;glib::gio-2.0>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### glibmm_DEPS_TARGET to all of them
conan_package_library_targets("${glibmm_LIBS_DEBUG}"    # libraries
                              "${glibmm_LIB_DIRS_DEBUG}" # package_libdir
                              "${glibmm_BIN_DIRS_DEBUG}" # package_bindir
                              "${glibmm_LIBRARY_TYPE_DEBUG}"
                              "${glibmm_IS_HOST_WINDOWS_DEBUG}"
                              glibmm_DEPS_TARGET
                              glibmm_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "glibmm"    # package_name
                              "${glibmm_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${glibmm_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT glibmm::giomm-2.68 #############

        set(glibmm_glibmm_giomm-2.68_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glibmm_glibmm_giomm-2.68_FRAMEWORKS_FOUND_DEBUG "${glibmm_glibmm_giomm-2.68_FRAMEWORKS_DEBUG}" "${glibmm_glibmm_giomm-2.68_FRAMEWORK_DIRS_DEBUG}")

        set(glibmm_glibmm_giomm-2.68_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glibmm_glibmm_giomm-2.68_DEPS_TARGET)
            add_library(glibmm_glibmm_giomm-2.68_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glibmm_glibmm_giomm-2.68_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glibmm_glibmm_giomm-2.68_DEPS_TARGET' to all of them
        conan_package_library_targets("${glibmm_glibmm_giomm-2.68_LIBS_DEBUG}"
                              "${glibmm_glibmm_giomm-2.68_LIB_DIRS_DEBUG}"
                              "${glibmm_glibmm_giomm-2.68_BIN_DIRS_DEBUG}" # package_bindir
                              "${glibmm_glibmm_giomm-2.68_LIBRARY_TYPE_DEBUG}"
                              "${glibmm_glibmm_giomm-2.68_IS_HOST_WINDOWS_DEBUG}"
                              glibmm_glibmm_giomm-2.68_DEPS_TARGET
                              glibmm_glibmm_giomm-2.68_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glibmm_glibmm_giomm-2.68"
                              "${glibmm_glibmm_giomm-2.68_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glibmm::giomm-2.68
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_LIBRARIES_TARGETS}>
                     )

        if("${glibmm_glibmm_giomm-2.68_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glibmm::giomm-2.68
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glibmm_glibmm_giomm-2.68_DEPS_TARGET)
        endif()

        set_property(TARGET glibmm::giomm-2.68 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glibmm::giomm-2.68 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glibmm::giomm-2.68 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_LIB_DIRS_DEBUG}>)
        set_property(TARGET glibmm::giomm-2.68 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glibmm::giomm-2.68 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glibmm::glibmm-2.68 #############

        set(glibmm_glibmm_glibmm-2.68_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glibmm_glibmm_glibmm-2.68_FRAMEWORKS_FOUND_DEBUG "${glibmm_glibmm_glibmm-2.68_FRAMEWORKS_DEBUG}" "${glibmm_glibmm_glibmm-2.68_FRAMEWORK_DIRS_DEBUG}")

        set(glibmm_glibmm_glibmm-2.68_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glibmm_glibmm_glibmm-2.68_DEPS_TARGET)
            add_library(glibmm_glibmm_glibmm-2.68_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glibmm_glibmm_glibmm-2.68_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glibmm_glibmm_glibmm-2.68_DEPS_TARGET' to all of them
        conan_package_library_targets("${glibmm_glibmm_glibmm-2.68_LIBS_DEBUG}"
                              "${glibmm_glibmm_glibmm-2.68_LIB_DIRS_DEBUG}"
                              "${glibmm_glibmm_glibmm-2.68_BIN_DIRS_DEBUG}" # package_bindir
                              "${glibmm_glibmm_glibmm-2.68_LIBRARY_TYPE_DEBUG}"
                              "${glibmm_glibmm_glibmm-2.68_IS_HOST_WINDOWS_DEBUG}"
                              glibmm_glibmm_glibmm-2.68_DEPS_TARGET
                              glibmm_glibmm_glibmm-2.68_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glibmm_glibmm_glibmm-2.68"
                              "${glibmm_glibmm_glibmm-2.68_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glibmm::glibmm-2.68
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_LIBRARIES_TARGETS}>
                     )

        if("${glibmm_glibmm_glibmm-2.68_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glibmm::glibmm-2.68
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glibmm_glibmm_glibmm-2.68_DEPS_TARGET)
        endif()

        set_property(TARGET glibmm::glibmm-2.68 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glibmm::glibmm-2.68 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glibmm::glibmm-2.68 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_LIB_DIRS_DEBUG}>)
        set_property(TARGET glibmm::glibmm-2.68 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glibmm::glibmm-2.68 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_DEBUG}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET glibmm::glibmm APPEND PROPERTY INTERFACE_LINK_LIBRARIES glibmm::giomm-2.68)
    set_property(TARGET glibmm::glibmm APPEND PROPERTY INTERFACE_LINK_LIBRARIES glibmm::glibmm-2.68)

########## For the modules (FindXXX)
set(glibmm_LIBRARIES_DEBUG glibmm::glibmm)
