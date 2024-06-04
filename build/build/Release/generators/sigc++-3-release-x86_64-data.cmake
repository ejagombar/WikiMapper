########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libsigcpp_COMPONENT_NAMES libsigcpp::sigc++)
list(REMOVE_DUPLICATES libsigcpp_COMPONENT_NAMES)
if(DEFINED libsigcpp_FIND_DEPENDENCY_NAMES)
  list(APPEND libsigcpp_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libsigcpp_FIND_DEPENDENCY_NAMES)
else()
  set(libsigcpp_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libsigcpp_PACKAGE_FOLDER_RELEASE "/home/ejago/.conan2/p/b/libsie7225d542d1af/p")
set(libsigcpp_BUILD_MODULES_PATHS_RELEASE )


set(libsigcpp_INCLUDE_DIRS_RELEASE )
set(libsigcpp_RES_DIRS_RELEASE )
set(libsigcpp_DEFINITIONS_RELEASE )
set(libsigcpp_SHARED_LINK_FLAGS_RELEASE )
set(libsigcpp_EXE_LINK_FLAGS_RELEASE )
set(libsigcpp_OBJECTS_RELEASE )
set(libsigcpp_COMPILE_DEFINITIONS_RELEASE )
set(libsigcpp_COMPILE_OPTIONS_C_RELEASE )
set(libsigcpp_COMPILE_OPTIONS_CXX_RELEASE )
set(libsigcpp_LIB_DIRS_RELEASE "${libsigcpp_PACKAGE_FOLDER_RELEASE}/lib")
set(libsigcpp_BIN_DIRS_RELEASE )
set(libsigcpp_LIBRARY_TYPE_RELEASE STATIC)
set(libsigcpp_IS_HOST_WINDOWS_RELEASE 0)
set(libsigcpp_LIBS_RELEASE sigc-3.0)
set(libsigcpp_SYSTEM_LIBS_RELEASE m)
set(libsigcpp_FRAMEWORK_DIRS_RELEASE )
set(libsigcpp_FRAMEWORKS_RELEASE )
set(libsigcpp_BUILD_DIRS_RELEASE )
set(libsigcpp_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libsigcpp_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libsigcpp_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libsigcpp_COMPILE_OPTIONS_C_RELEASE}>")
set(libsigcpp_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libsigcpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libsigcpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libsigcpp_EXE_LINK_FLAGS_RELEASE}>")


set(libsigcpp_COMPONENTS_RELEASE libsigcpp::sigc++)
########### COMPONENT libsigcpp::sigc++ VARIABLES ############################################

set(libsigcpp_libsigcpp_sigc++_INCLUDE_DIRS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_LIB_DIRS_RELEASE "${libsigcpp_PACKAGE_FOLDER_RELEASE}/lib")
set(libsigcpp_libsigcpp_sigc++_BIN_DIRS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_LIBRARY_TYPE_RELEASE STATIC)
set(libsigcpp_libsigcpp_sigc++_IS_HOST_WINDOWS_RELEASE 0)
set(libsigcpp_libsigcpp_sigc++_RES_DIRS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_DEFINITIONS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_OBJECTS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_COMPILE_DEFINITIONS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_C_RELEASE "")
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_CXX_RELEASE "")
set(libsigcpp_libsigcpp_sigc++_LIBS_RELEASE sigc-3.0)
set(libsigcpp_libsigcpp_sigc++_SYSTEM_LIBS_RELEASE m)
set(libsigcpp_libsigcpp_sigc++_FRAMEWORK_DIRS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_FRAMEWORKS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_DEPENDENCIES_RELEASE )
set(libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_EXE_LINK_FLAGS_RELEASE )
set(libsigcpp_libsigcpp_sigc++_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(libsigcpp_libsigcpp_sigc++_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libsigcpp_libsigcpp_sigc++_EXE_LINK_FLAGS_RELEASE}>
)
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_C_RELEASE}>")