# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libmount_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libmount_FRAMEWORKS_FOUND_DEBUG "${libmount_FRAMEWORKS_DEBUG}" "${libmount_FRAMEWORK_DIRS_DEBUG}")

set(libmount_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libmount_DEPS_TARGET)
    add_library(libmount_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libmount_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libmount_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${libmount_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:libmount::libblkid>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libmount_DEPS_TARGET to all of them
conan_package_library_targets("${libmount_LIBS_DEBUG}"    # libraries
                              "${libmount_LIB_DIRS_DEBUG}" # package_libdir
                              "${libmount_BIN_DIRS_DEBUG}" # package_bindir
                              "${libmount_LIBRARY_TYPE_DEBUG}"
                              "${libmount_IS_HOST_WINDOWS_DEBUG}"
                              libmount_DEPS_TARGET
                              libmount_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "libmount"    # package_name
                              "${libmount_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libmount_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT libmount::libmount #############

        set(libmount_libmount_libmount_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(libmount_libmount_libmount_FRAMEWORKS_FOUND_DEBUG "${libmount_libmount_libmount_FRAMEWORKS_DEBUG}" "${libmount_libmount_libmount_FRAMEWORK_DIRS_DEBUG}")

        set(libmount_libmount_libmount_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET libmount_libmount_libmount_DEPS_TARGET)
            add_library(libmount_libmount_libmount_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET libmount_libmount_libmount_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'libmount_libmount_libmount_DEPS_TARGET' to all of them
        conan_package_library_targets("${libmount_libmount_libmount_LIBS_DEBUG}"
                              "${libmount_libmount_libmount_LIB_DIRS_DEBUG}"
                              "${libmount_libmount_libmount_BIN_DIRS_DEBUG}" # package_bindir
                              "${libmount_libmount_libmount_LIBRARY_TYPE_DEBUG}"
                              "${libmount_libmount_libmount_IS_HOST_WINDOWS_DEBUG}"
                              libmount_libmount_libmount_DEPS_TARGET
                              libmount_libmount_libmount_LIBRARIES_TARGETS
                              "_DEBUG"
                              "libmount_libmount_libmount"
                              "${libmount_libmount_libmount_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET libmount::libmount
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_LIBRARIES_TARGETS}>
                     )

        if("${libmount_libmount_libmount_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET libmount::libmount
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         libmount_libmount_libmount_DEPS_TARGET)
        endif()

        set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_LIB_DIRS_DEBUG}>)
        set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libmount_COMPILE_OPTIONS_DEBUG}>)

    ########## COMPONENT libmount::libblkid #############

        set(libmount_libmount_libblkid_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(libmount_libmount_libblkid_FRAMEWORKS_FOUND_DEBUG "${libmount_libmount_libblkid_FRAMEWORKS_DEBUG}" "${libmount_libmount_libblkid_FRAMEWORK_DIRS_DEBUG}")

        set(libmount_libmount_libblkid_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET libmount_libmount_libblkid_DEPS_TARGET)
            add_library(libmount_libmount_libblkid_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET libmount_libmount_libblkid_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_DEPENDENCIES_DEBUG}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'libmount_libmount_libblkid_DEPS_TARGET' to all of them
        conan_package_library_targets("${libmount_libmount_libblkid_LIBS_DEBUG}"
                              "${libmount_libmount_libblkid_LIB_DIRS_DEBUG}"
                              "${libmount_libmount_libblkid_BIN_DIRS_DEBUG}" # package_bindir
                              "${libmount_libmount_libblkid_LIBRARY_TYPE_DEBUG}"
                              "${libmount_libmount_libblkid_IS_HOST_WINDOWS_DEBUG}"
                              libmount_libmount_libblkid_DEPS_TARGET
                              libmount_libmount_libblkid_LIBRARIES_TARGETS
                              "_DEBUG"
                              "libmount_libmount_libblkid"
                              "${libmount_libmount_libblkid_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET libmount::libblkid
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_LIBRARIES_TARGETS}>
                     )

        if("${libmount_libmount_libblkid_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET libmount::libblkid
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         libmount_libmount_libblkid_DEPS_TARGET)
        endif()

        set_property(TARGET libmount::libblkid APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_LINKER_FLAGS_DEBUG}>)
        set_property(TARGET libmount::libblkid APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_INCLUDE_DIRS_DEBUG}>)
        set_property(TARGET libmount::libblkid APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_LIB_DIRS_DEBUG}>)
        set_property(TARGET libmount::libblkid APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_COMPILE_DEFINITIONS_DEBUG}>)
        set_property(TARGET libmount::libblkid APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${libmount_libmount_libblkid_COMPILE_OPTIONS_DEBUG}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_LINK_LIBRARIES libmount::libmount)
    set_property(TARGET libmount::libmount APPEND PROPERTY INTERFACE_LINK_LIBRARIES libmount::libblkid)

########## For the modules (FindXXX)
set(libmount_LIBRARIES_DEBUG libmount::libmount)
