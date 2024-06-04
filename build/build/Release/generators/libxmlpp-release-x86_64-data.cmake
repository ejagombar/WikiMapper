########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libxmlpp_COMPONENT_NAMES "")
if(DEFINED libxmlpp_FIND_DEPENDENCY_NAMES)
  list(APPEND libxmlpp_FIND_DEPENDENCY_NAMES libxml2 glibmm)
  list(REMOVE_DUPLICATES libxmlpp_FIND_DEPENDENCY_NAMES)
else()
  set(libxmlpp_FIND_DEPENDENCY_NAMES libxml2 glibmm)
endif()
set(libxml2_FIND_MODE "NO_MODULE")
set(glibmm_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libxmlpp_PACKAGE_FOLDER_RELEASE "/home/ejago/.conan2/p/b/libxm2761441e0e153/p")
set(libxmlpp_BUILD_MODULES_PATHS_RELEASE )


set(libxmlpp_INCLUDE_DIRS_RELEASE "${libxmlpp_PACKAGE_FOLDER_RELEASE}/include/libxml++-5.0")
set(libxmlpp_RES_DIRS_RELEASE )
set(libxmlpp_DEFINITIONS_RELEASE )
set(libxmlpp_SHARED_LINK_FLAGS_RELEASE )
set(libxmlpp_EXE_LINK_FLAGS_RELEASE )
set(libxmlpp_OBJECTS_RELEASE )
set(libxmlpp_COMPILE_DEFINITIONS_RELEASE )
set(libxmlpp_COMPILE_OPTIONS_C_RELEASE )
set(libxmlpp_COMPILE_OPTIONS_CXX_RELEASE )
set(libxmlpp_LIB_DIRS_RELEASE "${libxmlpp_PACKAGE_FOLDER_RELEASE}/lib")
set(libxmlpp_BIN_DIRS_RELEASE )
set(libxmlpp_LIBRARY_TYPE_RELEASE STATIC)
set(libxmlpp_IS_HOST_WINDOWS_RELEASE 0)
set(libxmlpp_LIBS_RELEASE xml++-5.0)
set(libxmlpp_SYSTEM_LIBS_RELEASE )
set(libxmlpp_FRAMEWORK_DIRS_RELEASE )
set(libxmlpp_FRAMEWORKS_RELEASE )
set(libxmlpp_BUILD_DIRS_RELEASE )
set(libxmlpp_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libxmlpp_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libxmlpp_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libxmlpp_COMPILE_OPTIONS_C_RELEASE}>")
set(libxmlpp_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libxmlpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libxmlpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libxmlpp_EXE_LINK_FLAGS_RELEASE}>")


set(libxmlpp_COMPONENTS_RELEASE )