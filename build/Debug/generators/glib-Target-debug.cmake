# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(glib_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(glib_FRAMEWORKS_FOUND_DEBUG "${glib_FRAMEWORKS_DEBUG}" "${glib_FRAMEWORK_DIRS_DEBUG}")

set(glib_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET glib_DEPS_TARGET)
    add_library(glib_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET glib_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${glib_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${glib_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:pcre2::pcre2;glib::glib-2.0;glib::gmodule-no-export-2.0;libffi::libffi;glib::gobject-2.0;glib::gmodule-2.0;ZLIB::ZLIB;libmount::libmount;libselinux::libselinux;libelf::libelf;glib::gio-2.0>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### glib_DEPS_TARGET to all of them
conan_package_library_targets("${glib_LIBS_DEBUG}"    # libraries
                              "${glib_LIB_DIRS_DEBUG}" # package_libdir
                              "${glib_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_LIBRARY_TYPE_DEBUG}"
                              "${glib_IS_HOST_WINDOWS_DEBUG}"
                              glib_DEPS_TARGET
                              glib_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "glib"    # package_name
                              "${glib_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${glib_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT glib::gio-unix-2.0 #############

        set(glib_glib_gio-unix-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gio-unix-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gio-unix-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gio-unix-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gio-unix-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gio-unix-2.0_DEPS_TARGET)
            add_library(glib_glib_gio-unix-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gio-unix-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gio-unix-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gio-unix-2.0_LIBS_DEBUG}"
                              "${glib_glib_gio-unix-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gio-unix-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gio-unix-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gio-unix-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gio-unix-2.0_DEPS_TARGET
                              glib_glib_gio-unix-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gio-unix-2.0"
                              "${glib_glib_gio-unix-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gio-unix-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gio-unix-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gio-unix-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gio-unix-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gio-unix-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gio-unix-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gio-unix-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gio-unix-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gio-unix-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-unix-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gresource #############

        set(glib_glib_gresource_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gresource_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gresource_FRAMEWORKS_DEBUG}" "${glib_glib_gresource_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gresource_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gresource_DEPS_TARGET)
            add_library(glib_glib_gresource_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gresource_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gresource_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gresource_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gresource_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gresource_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gresource_LIBS_DEBUG}"
                              "${glib_glib_gresource_LIB_DIRS_DEBUG}"
                              "${glib_glib_gresource_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gresource_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gresource_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gresource_DEPS_TARGET
                              glib_glib_gresource_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gresource"
                              "${glib_glib_gresource_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gresource
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gresource_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gresource_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gresource_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gresource
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gresource_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gresource APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gresource_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gresource APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gresource_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gresource APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gresource_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gresource APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gresource_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gresource APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gresource_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gio-2.0 #############

        set(glib_glib_gio-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gio-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gio-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gio-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gio-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gio-2.0_DEPS_TARGET)
            add_library(glib_glib_gio-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gio-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gio-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gio-2.0_LIBS_DEBUG}"
                              "${glib_glib_gio-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gio-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gio-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gio-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gio-2.0_DEPS_TARGET
                              glib_glib_gio-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gio-2.0"
                              "${glib_glib_gio-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gio-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gio-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gio-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gio-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gio-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gio-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gio-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gio-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gio-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gio-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gthread-2.0 #############

        set(glib_glib_gthread-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gthread-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gthread-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gthread-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gthread-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gthread-2.0_DEPS_TARGET)
            add_library(glib_glib_gthread-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gthread-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gthread-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gthread-2.0_LIBS_DEBUG}"
                              "${glib_glib_gthread-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gthread-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gthread-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gthread-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gthread-2.0_DEPS_TARGET
                              glib_glib_gthread-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gthread-2.0"
                              "${glib_glib_gthread-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gthread-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gthread-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gthread-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gthread-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gthread-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gthread-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gthread-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gthread-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gthread-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gthread-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gobject-2.0 #############

        set(glib_glib_gobject-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gobject-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gobject-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gobject-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gobject-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gobject-2.0_DEPS_TARGET)
            add_library(glib_glib_gobject-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gobject-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gobject-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gobject-2.0_LIBS_DEBUG}"
                              "${glib_glib_gobject-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gobject-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gobject-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gobject-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gobject-2.0_DEPS_TARGET
                              glib_glib_gobject-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gobject-2.0"
                              "${glib_glib_gobject-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gobject-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gobject-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gobject-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gobject-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gobject-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gobject-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gobject-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gobject-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gobject-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gobject-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gmodule-2.0 #############

        set(glib_glib_gmodule-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gmodule-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gmodule-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gmodule-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gmodule-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gmodule-2.0_DEPS_TARGET)
            add_library(glib_glib_gmodule-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gmodule-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gmodule-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gmodule-2.0_LIBS_DEBUG}"
                              "${glib_glib_gmodule-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gmodule-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gmodule-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gmodule-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gmodule-2.0_DEPS_TARGET
                              glib_glib_gmodule-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gmodule-2.0"
                              "${glib_glib_gmodule-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gmodule-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gmodule-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gmodule-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gmodule-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gmodule-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gmodule-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gmodule-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gmodule-export-2.0 #############

        set(glib_glib_gmodule-export-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gmodule-export-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gmodule-export-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gmodule-export-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gmodule-export-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gmodule-export-2.0_DEPS_TARGET)
            add_library(glib_glib_gmodule-export-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gmodule-export-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gmodule-export-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gmodule-export-2.0_LIBS_DEBUG}"
                              "${glib_glib_gmodule-export-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gmodule-export-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gmodule-export-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gmodule-export-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gmodule-export-2.0_DEPS_TARGET
                              glib_glib_gmodule-export-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gmodule-export-2.0"
                              "${glib_glib_gmodule-export-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gmodule-export-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gmodule-export-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gmodule-export-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gmodule-export-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gmodule-export-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gmodule-export-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-export-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-export-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gmodule-export-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-export-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::gmodule-no-export-2.0 #############

        set(glib_glib_gmodule-no-export-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_gmodule-no-export-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_gmodule-no-export-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_gmodule-no-export-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_gmodule-no-export-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_gmodule-no-export-2.0_DEPS_TARGET)
            add_library(glib_glib_gmodule-no-export-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_gmodule-no-export-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_gmodule-no-export-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_gmodule-no-export-2.0_LIBS_DEBUG}"
                              "${glib_glib_gmodule-no-export-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_gmodule-no-export-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_gmodule-no-export-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_gmodule-no-export-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_gmodule-no-export-2.0_DEPS_TARGET
                              glib_glib_gmodule-no-export-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_gmodule-no-export-2.0"
                              "${glib_glib_gmodule-no-export-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::gmodule-no-export-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_gmodule-no-export-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::gmodule-no-export-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_gmodule-no-export-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::gmodule-no-export-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::gmodule-no-export-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-no-export-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::gmodule-no-export-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::gmodule-no-export-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_gmodule-no-export-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT glib::glib-2.0 #############

        set(glib_glib_glib-2.0_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(glib_glib_glib-2.0_FRAMEWORKS_FOUND_DEBUG "${glib_glib_glib-2.0_FRAMEWORKS_DEBUG}" "${glib_glib_glib-2.0_FRAMEWORK_DIRS_DEBUG}")

        set(glib_glib_glib-2.0_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET glib_glib_glib-2.0_DEPS_TARGET)
            add_library(glib_glib_glib-2.0_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET glib_glib_glib-2.0_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'glib_glib_glib-2.0_DEPS_TARGET' to all of them
        conan_package_library_targets("${glib_glib_glib-2.0_LIBS_DEBUG}"
                              "${glib_glib_glib-2.0_LIB_DIRS_DEBUG}"
                              "${glib_glib_glib-2.0_BIN_DIRS_DEBUG}" # package_bindir
                              "${glib_glib_glib-2.0_LIBRARY_TYPE_DEBUG}"
                              "${glib_glib_glib-2.0_IS_HOST_WINDOWS_DEBUG}"
                              glib_glib_glib-2.0_DEPS_TARGET
                              glib_glib_glib-2.0_LIBRARIES_TARGETS
                              "_DEBUG"
                              "glib_glib_glib-2.0"
                              "${glib_glib_glib-2.0_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET glib::glib-2.0
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_LIBRARIES_TARGETS}>
                     )

        if("${glib_glib_glib-2.0_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET glib::glib-2.0
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         glib_glib_glib-2.0_DEPS_TARGET)
        endif()

        set_property(TARGET glib::glib-2.0 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET glib::glib-2.0 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET glib::glib-2.0 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_LIB_DIRS_DEBUG}>)
        set_property(TARGET glib::glib-2.0 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET glib::glib-2.0 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${glib_glib_glib-2.0_COMPILE_OPTIONS_DEBUG}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gio-unix-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gresource)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gio-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gthread-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gobject-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gmodule-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gmodule-export-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::gmodule-no-export-2.0)
    set_property(TARGET glib::glib APPEND PROPERTY INTERFACE_LINK_LIBRARIES glib::glib-2.0)

########## For the modules (FindXXX)
set(glib_LIBRARIES_DEBUG glib::glib)
