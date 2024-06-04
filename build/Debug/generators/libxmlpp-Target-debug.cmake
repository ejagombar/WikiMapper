# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libxmlpp_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libxmlpp_FRAMEWORKS_FOUND_DEBUG "${libxmlpp_FRAMEWORKS_DEBUG}" "${libxmlpp_FRAMEWORK_DIRS_DEBUG}")

set(libxmlpp_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libxmlpp_DEPS_TARGET)
    add_library(libxmlpp_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libxmlpp_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libxmlpp_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${libxmlpp_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:LibXml2::LibXml2;glibmm::glibmm>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libxmlpp_DEPS_TARGET to all of them
conan_package_library_targets("${libxmlpp_LIBS_DEBUG}"    # libraries
                              "${libxmlpp_LIB_DIRS_DEBUG}" # package_libdir
                              "${libxmlpp_BIN_DIRS_DEBUG}" # package_bindir
                              "${libxmlpp_LIBRARY_TYPE_DEBUG}"
                              "${libxmlpp_IS_HOST_WINDOWS_DEBUG}"
                              libxmlpp_DEPS_TARGET
                              libxmlpp_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "libxmlpp"    # package_name
                              "${libxmlpp_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libxmlpp_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${libxmlpp_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${libxmlpp_LIBRARIES_TARGETS}>
                 )

    if("${libxmlpp_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libxmlpp::libxmlpp
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libxmlpp_DEPS_TARGET)
    endif()

    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${libxmlpp_LINKER_FLAGS_DEBUG}>)
    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${libxmlpp_INCLUDE_DIRS_DEBUG}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${libxmlpp_LIB_DIRS_DEBUG}>)
    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${libxmlpp_COMPILE_DEFINITIONS_DEBUG}>)
    set_property(TARGET libxmlpp::libxmlpp
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${libxmlpp_COMPILE_OPTIONS_DEBUG}>)

########## For the modules (FindXXX)
set(libxmlpp_LIBRARIES_DEBUG libxmlpp::libxmlpp)
