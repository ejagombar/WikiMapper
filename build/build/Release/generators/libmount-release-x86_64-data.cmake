########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libmount_COMPONENT_NAMES libmount::libblkid libmount::libmount)
list(REMOVE_DUPLICATES libmount_COMPONENT_NAMES)
if(DEFINED libmount_FIND_DEPENDENCY_NAMES)
  list(APPEND libmount_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libmount_FIND_DEPENDENCY_NAMES)
else()
  set(libmount_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libmount_PACKAGE_FOLDER_RELEASE "/home/ejago/.conan2/p/b/libmo0274f8c68372f/p")
set(libmount_BUILD_MODULES_PATHS_RELEASE )


set(libmount_INCLUDE_DIRS_RELEASE )
set(libmount_RES_DIRS_RELEASE )
set(libmount_DEFINITIONS_RELEASE )
set(libmount_SHARED_LINK_FLAGS_RELEASE )
set(libmount_EXE_LINK_FLAGS_RELEASE )
set(libmount_OBJECTS_RELEASE )
set(libmount_COMPILE_DEFINITIONS_RELEASE )
set(libmount_COMPILE_OPTIONS_C_RELEASE )
set(libmount_COMPILE_OPTIONS_CXX_RELEASE )
set(libmount_LIB_DIRS_RELEASE "${libmount_PACKAGE_FOLDER_RELEASE}/lib")
set(libmount_BIN_DIRS_RELEASE )
set(libmount_LIBRARY_TYPE_RELEASE STATIC)
set(libmount_IS_HOST_WINDOWS_RELEASE 0)
set(libmount_LIBS_RELEASE mount blkid)
set(libmount_SYSTEM_LIBS_RELEASE rt)
set(libmount_FRAMEWORK_DIRS_RELEASE )
set(libmount_FRAMEWORKS_RELEASE )
set(libmount_BUILD_DIRS_RELEASE )
set(libmount_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libmount_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libmount_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libmount_COMPILE_OPTIONS_C_RELEASE}>")
set(libmount_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libmount_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libmount_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libmount_EXE_LINK_FLAGS_RELEASE}>")


set(libmount_COMPONENTS_RELEASE libmount::libblkid libmount::libmount)
########### COMPONENT libmount::libmount VARIABLES ############################################

set(libmount_libmount_libmount_INCLUDE_DIRS_RELEASE )
set(libmount_libmount_libmount_LIB_DIRS_RELEASE "${libmount_PACKAGE_FOLDER_RELEASE}/lib")
set(libmount_libmount_libmount_BIN_DIRS_RELEASE )
set(libmount_libmount_libmount_LIBRARY_TYPE_RELEASE STATIC)
set(libmount_libmount_libmount_IS_HOST_WINDOWS_RELEASE 0)
set(libmount_libmount_libmount_RES_DIRS_RELEASE )
set(libmount_libmount_libmount_DEFINITIONS_RELEASE )
set(libmount_libmount_libmount_OBJECTS_RELEASE )
set(libmount_libmount_libmount_COMPILE_DEFINITIONS_RELEASE )
set(libmount_libmount_libmount_COMPILE_OPTIONS_C_RELEASE "")
set(libmount_libmount_libmount_COMPILE_OPTIONS_CXX_RELEASE "")
set(libmount_libmount_libmount_LIBS_RELEASE mount)
set(libmount_libmount_libmount_SYSTEM_LIBS_RELEASE rt)
set(libmount_libmount_libmount_FRAMEWORK_DIRS_RELEASE )
set(libmount_libmount_libmount_FRAMEWORKS_RELEASE )
set(libmount_libmount_libmount_DEPENDENCIES_RELEASE libmount::libblkid)
set(libmount_libmount_libmount_SHARED_LINK_FLAGS_RELEASE )
set(libmount_libmount_libmount_EXE_LINK_FLAGS_RELEASE )
set(libmount_libmount_libmount_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(libmount_libmount_libmount_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libmount_libmount_libmount_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libmount_libmount_libmount_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libmount_libmount_libmount_EXE_LINK_FLAGS_RELEASE}>
)
set(libmount_libmount_libmount_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libmount_libmount_libmount_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libmount_libmount_libmount_COMPILE_OPTIONS_C_RELEASE}>")
########### COMPONENT libmount::libblkid VARIABLES ############################################

set(libmount_libmount_libblkid_INCLUDE_DIRS_RELEASE )
set(libmount_libmount_libblkid_LIB_DIRS_RELEASE "${libmount_PACKAGE_FOLDER_RELEASE}/lib")
set(libmount_libmount_libblkid_BIN_DIRS_RELEASE )
set(libmount_libmount_libblkid_LIBRARY_TYPE_RELEASE STATIC)
set(libmount_libmount_libblkid_IS_HOST_WINDOWS_RELEASE 0)
set(libmount_libmount_libblkid_RES_DIRS_RELEASE )
set(libmount_libmount_libblkid_DEFINITIONS_RELEASE )
set(libmount_libmount_libblkid_OBJECTS_RELEASE )
set(libmount_libmount_libblkid_COMPILE_DEFINITIONS_RELEASE )
set(libmount_libmount_libblkid_COMPILE_OPTIONS_C_RELEASE "")
set(libmount_libmount_libblkid_COMPILE_OPTIONS_CXX_RELEASE "")
set(libmount_libmount_libblkid_LIBS_RELEASE blkid)
set(libmount_libmount_libblkid_SYSTEM_LIBS_RELEASE )
set(libmount_libmount_libblkid_FRAMEWORK_DIRS_RELEASE )
set(libmount_libmount_libblkid_FRAMEWORKS_RELEASE )
set(libmount_libmount_libblkid_DEPENDENCIES_RELEASE )
set(libmount_libmount_libblkid_SHARED_LINK_FLAGS_RELEASE )
set(libmount_libmount_libblkid_EXE_LINK_FLAGS_RELEASE )
set(libmount_libmount_libblkid_NO_SONAME_MODE_RELEASE FALSE)

# COMPOUND VARIABLES
set(libmount_libmount_libblkid_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libmount_libmount_libblkid_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libmount_libmount_libblkid_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libmount_libmount_libblkid_EXE_LINK_FLAGS_RELEASE}>
)
set(libmount_libmount_libblkid_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libmount_libmount_libblkid_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libmount_libmount_libblkid_COMPILE_OPTIONS_C_RELEASE}>")