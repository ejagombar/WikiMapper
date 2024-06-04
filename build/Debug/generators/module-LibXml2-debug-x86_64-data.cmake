########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libxml2_COMPONENT_NAMES "")
if(DEFINED libxml2_FIND_DEPENDENCY_NAMES)
  list(APPEND libxml2_FIND_DEPENDENCY_NAMES Iconv ZLIB)
  list(REMOVE_DUPLICATES libxml2_FIND_DEPENDENCY_NAMES)
else()
  set(libxml2_FIND_DEPENDENCY_NAMES Iconv ZLIB)
endif()
set(Iconv_FIND_MODE "MODULE")
set(ZLIB_FIND_MODE "MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libxml2_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libxm47331dfc6f5fc/p")
set(libxml2_BUILD_MODULES_PATHS_DEBUG "${libxml2_PACKAGE_FOLDER_DEBUG}/lib/cmake/conan-official-libxml2-variables.cmake")


set(libxml2_INCLUDE_DIRS_DEBUG )
set(libxml2_RES_DIRS_DEBUG )
set(libxml2_DEFINITIONS_DEBUG "-DLIBXML_STATIC")
set(libxml2_SHARED_LINK_FLAGS_DEBUG )
set(libxml2_EXE_LINK_FLAGS_DEBUG )
set(libxml2_OBJECTS_DEBUG )
set(libxml2_COMPILE_DEFINITIONS_DEBUG "LIBXML_STATIC")
set(libxml2_COMPILE_OPTIONS_C_DEBUG )
set(libxml2_COMPILE_OPTIONS_CXX_DEBUG )
set(libxml2_LIB_DIRS_DEBUG "${libxml2_PACKAGE_FOLDER_DEBUG}/lib")
set(libxml2_BIN_DIRS_DEBUG )
set(libxml2_LIBRARY_TYPE_DEBUG STATIC)
set(libxml2_IS_HOST_WINDOWS_DEBUG 0)
set(libxml2_LIBS_DEBUG xml2)
set(libxml2_SYSTEM_LIBS_DEBUG m pthread dl)
set(libxml2_FRAMEWORK_DIRS_DEBUG )
set(libxml2_FRAMEWORKS_DEBUG )
set(libxml2_BUILD_DIRS_DEBUG )
set(libxml2_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libxml2_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libxml2_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libxml2_COMPILE_OPTIONS_C_DEBUG}>")
set(libxml2_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libxml2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libxml2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libxml2_EXE_LINK_FLAGS_DEBUG}>")


set(libxml2_COMPONENTS_DEBUG )