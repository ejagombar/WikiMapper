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
set(libsigcpp_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libsic1112c30ab147/p")
set(libsigcpp_BUILD_MODULES_PATHS_DEBUG )


set(libsigcpp_INCLUDE_DIRS_DEBUG )
set(libsigcpp_RES_DIRS_DEBUG )
set(libsigcpp_DEFINITIONS_DEBUG )
set(libsigcpp_SHARED_LINK_FLAGS_DEBUG )
set(libsigcpp_EXE_LINK_FLAGS_DEBUG )
set(libsigcpp_OBJECTS_DEBUG )
set(libsigcpp_COMPILE_DEFINITIONS_DEBUG )
set(libsigcpp_COMPILE_OPTIONS_C_DEBUG )
set(libsigcpp_COMPILE_OPTIONS_CXX_DEBUG )
set(libsigcpp_LIB_DIRS_DEBUG "${libsigcpp_PACKAGE_FOLDER_DEBUG}/lib")
set(libsigcpp_BIN_DIRS_DEBUG )
set(libsigcpp_LIBRARY_TYPE_DEBUG STATIC)
set(libsigcpp_IS_HOST_WINDOWS_DEBUG 0)
set(libsigcpp_LIBS_DEBUG sigc-3.0)
set(libsigcpp_SYSTEM_LIBS_DEBUG m)
set(libsigcpp_FRAMEWORK_DIRS_DEBUG )
set(libsigcpp_FRAMEWORKS_DEBUG )
set(libsigcpp_BUILD_DIRS_DEBUG )
set(libsigcpp_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libsigcpp_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libsigcpp_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libsigcpp_COMPILE_OPTIONS_C_DEBUG}>")
set(libsigcpp_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libsigcpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libsigcpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libsigcpp_EXE_LINK_FLAGS_DEBUG}>")


set(libsigcpp_COMPONENTS_DEBUG libsigcpp::sigc++)
########### COMPONENT libsigcpp::sigc++ VARIABLES ############################################

set(libsigcpp_libsigcpp_sigc++_INCLUDE_DIRS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_LIB_DIRS_DEBUG "${libsigcpp_PACKAGE_FOLDER_DEBUG}/lib")
set(libsigcpp_libsigcpp_sigc++_BIN_DIRS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_LIBRARY_TYPE_DEBUG STATIC)
set(libsigcpp_libsigcpp_sigc++_IS_HOST_WINDOWS_DEBUG 0)
set(libsigcpp_libsigcpp_sigc++_RES_DIRS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_DEFINITIONS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_OBJECTS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_COMPILE_DEFINITIONS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_C_DEBUG "")
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_CXX_DEBUG "")
set(libsigcpp_libsigcpp_sigc++_LIBS_DEBUG sigc-3.0)
set(libsigcpp_libsigcpp_sigc++_SYSTEM_LIBS_DEBUG m)
set(libsigcpp_libsigcpp_sigc++_FRAMEWORK_DIRS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_FRAMEWORKS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_DEPENDENCIES_DEBUG )
set(libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_EXE_LINK_FLAGS_DEBUG )
set(libsigcpp_libsigcpp_sigc++_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(libsigcpp_libsigcpp_sigc++_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libsigcpp_libsigcpp_sigc++_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libsigcpp_libsigcpp_sigc++_EXE_LINK_FLAGS_DEBUG}>
)
set(libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libsigcpp_libsigcpp_sigc++_COMPILE_OPTIONS_C_DEBUG}>")