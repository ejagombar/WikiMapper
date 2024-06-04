########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libselinux_COMPONENT_NAMES libselinux::sepol libselinux::selinux)
list(REMOVE_DUPLICATES libselinux_COMPONENT_NAMES)
if(DEFINED libselinux_FIND_DEPENDENCY_NAMES)
  list(APPEND libselinux_FIND_DEPENDENCY_NAMES PCRE2)
  list(REMOVE_DUPLICATES libselinux_FIND_DEPENDENCY_NAMES)
else()
  set(libselinux_FIND_DEPENDENCY_NAMES PCRE2)
endif()
set(PCRE2_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libselinux_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libse6348ef6ba3b21/p")
set(libselinux_BUILD_MODULES_PATHS_DEBUG )


set(libselinux_INCLUDE_DIRS_DEBUG )
set(libselinux_RES_DIRS_DEBUG )
set(libselinux_DEFINITIONS_DEBUG )
set(libselinux_SHARED_LINK_FLAGS_DEBUG )
set(libselinux_EXE_LINK_FLAGS_DEBUG )
set(libselinux_OBJECTS_DEBUG )
set(libselinux_COMPILE_DEFINITIONS_DEBUG )
set(libselinux_COMPILE_OPTIONS_C_DEBUG )
set(libselinux_COMPILE_OPTIONS_CXX_DEBUG )
set(libselinux_LIB_DIRS_DEBUG "${libselinux_PACKAGE_FOLDER_DEBUG}/lib")
set(libselinux_BIN_DIRS_DEBUG )
set(libselinux_LIBRARY_TYPE_DEBUG STATIC)
set(libselinux_IS_HOST_WINDOWS_DEBUG 0)
set(libselinux_LIBS_DEBUG selinux sepol)
set(libselinux_SYSTEM_LIBS_DEBUG )
set(libselinux_FRAMEWORK_DIRS_DEBUG )
set(libselinux_FRAMEWORKS_DEBUG )
set(libselinux_BUILD_DIRS_DEBUG )
set(libselinux_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libselinux_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libselinux_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libselinux_COMPILE_OPTIONS_C_DEBUG}>")
set(libselinux_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libselinux_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libselinux_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libselinux_EXE_LINK_FLAGS_DEBUG}>")


set(libselinux_COMPONENTS_DEBUG libselinux::sepol libselinux::selinux)
########### COMPONENT libselinux::selinux VARIABLES ############################################

set(libselinux_libselinux_selinux_INCLUDE_DIRS_DEBUG )
set(libselinux_libselinux_selinux_LIB_DIRS_DEBUG "${libselinux_PACKAGE_FOLDER_DEBUG}/lib")
set(libselinux_libselinux_selinux_BIN_DIRS_DEBUG )
set(libselinux_libselinux_selinux_LIBRARY_TYPE_DEBUG STATIC)
set(libselinux_libselinux_selinux_IS_HOST_WINDOWS_DEBUG 0)
set(libselinux_libselinux_selinux_RES_DIRS_DEBUG )
set(libselinux_libselinux_selinux_DEFINITIONS_DEBUG )
set(libselinux_libselinux_selinux_OBJECTS_DEBUG )
set(libselinux_libselinux_selinux_COMPILE_DEFINITIONS_DEBUG )
set(libselinux_libselinux_selinux_COMPILE_OPTIONS_C_DEBUG "")
set(libselinux_libselinux_selinux_COMPILE_OPTIONS_CXX_DEBUG "")
set(libselinux_libselinux_selinux_LIBS_DEBUG selinux)
set(libselinux_libselinux_selinux_SYSTEM_LIBS_DEBUG )
set(libselinux_libselinux_selinux_FRAMEWORK_DIRS_DEBUG )
set(libselinux_libselinux_selinux_FRAMEWORKS_DEBUG )
set(libselinux_libselinux_selinux_DEPENDENCIES_DEBUG libselinux::sepol pcre2::pcre2)
set(libselinux_libselinux_selinux_SHARED_LINK_FLAGS_DEBUG )
set(libselinux_libselinux_selinux_EXE_LINK_FLAGS_DEBUG )
set(libselinux_libselinux_selinux_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(libselinux_libselinux_selinux_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libselinux_libselinux_selinux_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libselinux_libselinux_selinux_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libselinux_libselinux_selinux_EXE_LINK_FLAGS_DEBUG}>
)
set(libselinux_libselinux_selinux_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libselinux_libselinux_selinux_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libselinux_libselinux_selinux_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT libselinux::sepol VARIABLES ############################################

set(libselinux_libselinux_sepol_INCLUDE_DIRS_DEBUG )
set(libselinux_libselinux_sepol_LIB_DIRS_DEBUG "${libselinux_PACKAGE_FOLDER_DEBUG}/lib")
set(libselinux_libselinux_sepol_BIN_DIRS_DEBUG )
set(libselinux_libselinux_sepol_LIBRARY_TYPE_DEBUG STATIC)
set(libselinux_libselinux_sepol_IS_HOST_WINDOWS_DEBUG 0)
set(libselinux_libselinux_sepol_RES_DIRS_DEBUG )
set(libselinux_libselinux_sepol_DEFINITIONS_DEBUG )
set(libselinux_libselinux_sepol_OBJECTS_DEBUG )
set(libselinux_libselinux_sepol_COMPILE_DEFINITIONS_DEBUG )
set(libselinux_libselinux_sepol_COMPILE_OPTIONS_C_DEBUG "")
set(libselinux_libselinux_sepol_COMPILE_OPTIONS_CXX_DEBUG "")
set(libselinux_libselinux_sepol_LIBS_DEBUG sepol)
set(libselinux_libselinux_sepol_SYSTEM_LIBS_DEBUG )
set(libselinux_libselinux_sepol_FRAMEWORK_DIRS_DEBUG )
set(libselinux_libselinux_sepol_FRAMEWORKS_DEBUG )
set(libselinux_libselinux_sepol_DEPENDENCIES_DEBUG )
set(libselinux_libselinux_sepol_SHARED_LINK_FLAGS_DEBUG )
set(libselinux_libselinux_sepol_EXE_LINK_FLAGS_DEBUG )
set(libselinux_libselinux_sepol_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(libselinux_libselinux_sepol_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libselinux_libselinux_sepol_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libselinux_libselinux_sepol_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libselinux_libselinux_sepol_EXE_LINK_FLAGS_DEBUG}>
)
set(libselinux_libselinux_sepol_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libselinux_libselinux_sepol_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libselinux_libselinux_sepol_COMPILE_OPTIONS_C_DEBUG}>")