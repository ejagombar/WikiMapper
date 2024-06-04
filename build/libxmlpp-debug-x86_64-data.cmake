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
set(libxmlpp_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libxmcca057fc6e571/p")
set(libxmlpp_BUILD_MODULES_PATHS_DEBUG )


set(libxmlpp_INCLUDE_DIRS_DEBUG "${libxmlpp_PACKAGE_FOLDER_DEBUG}/include/libxml++-5.0")
set(libxmlpp_RES_DIRS_DEBUG )
set(libxmlpp_DEFINITIONS_DEBUG )
set(libxmlpp_SHARED_LINK_FLAGS_DEBUG )
set(libxmlpp_EXE_LINK_FLAGS_DEBUG )
set(libxmlpp_OBJECTS_DEBUG )
set(libxmlpp_COMPILE_DEFINITIONS_DEBUG )
set(libxmlpp_COMPILE_OPTIONS_C_DEBUG )
set(libxmlpp_COMPILE_OPTIONS_CXX_DEBUG )
set(libxmlpp_LIB_DIRS_DEBUG "${libxmlpp_PACKAGE_FOLDER_DEBUG}/lib")
set(libxmlpp_BIN_DIRS_DEBUG )
set(libxmlpp_LIBRARY_TYPE_DEBUG STATIC)
set(libxmlpp_IS_HOST_WINDOWS_DEBUG 0)
set(libxmlpp_LIBS_DEBUG xml++-5.0)
set(libxmlpp_SYSTEM_LIBS_DEBUG )
set(libxmlpp_FRAMEWORK_DIRS_DEBUG )
set(libxmlpp_FRAMEWORKS_DEBUG )
set(libxmlpp_BUILD_DIRS_DEBUG )
set(libxmlpp_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libxmlpp_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libxmlpp_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libxmlpp_COMPILE_OPTIONS_C_DEBUG}>")
set(libxmlpp_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libxmlpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libxmlpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libxmlpp_EXE_LINK_FLAGS_DEBUG}>")


set(libxmlpp_COMPONENTS_DEBUG )