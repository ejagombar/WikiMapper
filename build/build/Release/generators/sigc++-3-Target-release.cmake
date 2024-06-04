# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libsigcpp_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libsigcpp_FRAMEWORKS_FOUND_RELEASE "${libsigcpp_FRAMEWORKS_RELEASE}" "${libsigcpp_FRAMEWORK_DIRS_RELEASE}")

set(libsigcpp_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libsigcpp_DEPS_TARGET)
    add_library(libsigcpp_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libsigcpp_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libsigcpp_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${libsigcpp_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libsigcpp_DEPS_TARGET to all of them
conan_package_library_targets("${libsigcpp_LIBS_RELEASE}"    # libraries
                              "${libsigcpp_LIB_DIRS_RELEASE}" # package_libdir
                              "${libsigcpp_BIN_DIRS_RELEASE}" # package_bindir
                              "${libsigcpp_LIBRARY_TYPE_RELEASE}"
                              "${libsigcpp_IS_HOST_WINDOWS_RELEASE}"
                              libsigcpp_DEPS_TARGET
                              libsigcpp_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "libsigcpp"    # package_name
                              "${libsigcpp_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libsigcpp_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT libsigcpp::sigc++ #############

        set(libsigcpp_libsigcpp_sigc++_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(libsigcpp_libsigcpp_sigc++_FRAMEWORKS_FOUND_RELEASE "${libsigcpp_libsigcpp_sigc++_FRAMEWORKS_RELEASE}" "${libsigcpp_libsigcpp_sigc++_FRAMEWORK_DIRS_RELEASE}")

        set(libsigcpp_libsigcpp_sigc++_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET libsigcpp_libsigcpp_sigc++_DEPS_TARGET)
            add_library(libsigcpp_libsigcpp_sigc++_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET libsigcpp_libsigcpp_sigc++_DEPS_TARGET
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_DEPENDENCIES_RELEASE}>
                     )

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'libsigcpp_libsigcpp_sigc++_DEPS_TARGET' to all of them
        conan_package_library_targets("${libsigcpp_libsigcpp_sigc++_LIBS_RELEASE}"
                              "${libsigcpp_libsigcpp_sigc++_LIB_DIRS_RELEASE}"
                              "${libsigcpp_libsigcpp_sigc++_BIN_DIRS_RELEASE}" # package_bindir
                              "${libsigcpp_libsigcpp_sigc++_LIBRARY_TYPE_RELEASE}"
                              "${libsigcpp_libsigcpp_sigc++_IS_HOST_WINDOWS_RELEASE}"
                              libsigcpp_libsigcpp_sigc++_DEPS_TARGET
                              libsigcpp_libsigcpp_sigc++_LIBRARIES_TARGETS
                              "_RELEASE"
                              "libsigcpp_libsigcpp_sigc++"
                              "${libsigcpp_libsigcpp_sigc++_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET libsigcpp::sigc++
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_LIBRARIES_TARGETS}>
                     )

        if("${libsigcpp_libsigcpp_sigc++_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET libsigcpp::sigc++
                         APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                         libsigcpp_libsigcpp_sigc++_DEPS_TARGET)
        endif()

        set_property(TARGET libsigcpp::sigc++ APPEND PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_LINKER_FLAGS_RELEASE}>)
        set_property(TARGET libsigcpp::sigc++ APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_INCLUDE_DIRS_RELEASE}>)
        set_property(TARGET libsigcpp::sigc++ APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_LIB_DIRS_RELEASE}>)
        set_property(TARGET libsigcpp::sigc++ APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_COMPILE_DEFINITIONS_RELEASE}>)
        set_property(TARGET libsigcpp::sigc++ APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_RELEASE}>)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET sigc-3.0 APPEND PROPERTY INTERFACE_LINK_LIBRARIES libsigcpp::sigc++)

########## For the modules (FindXXX)
set(libsigcpp_LIBRARIES_RELEASE sigc-3.0)
