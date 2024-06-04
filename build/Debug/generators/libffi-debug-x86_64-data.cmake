########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libffi_COMPONENT_NAMES "")
if(DEFINED libffi_FIND_DEPENDENCY_NAMES)
  list(APPEND libffi_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libffi_FIND_DEPENDENCY_NAMES)
else()
  set(libffi_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libffi_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libff349591145e1b5/p")
set(libffi_BUILD_MODULES_PATHS_DEBUG )


set(libffi_INCLUDE_DIRS_DEBUG )
set(libffi_RES_DIRS_DEBUG )
set(libffi_DEFINITIONS_DEBUG "-DFFI_BUILDING")
set(libffi_SHARED_LINK_FLAGS_DEBUG )
set(libffi_EXE_LINK_FLAGS_DEBUG )
set(libffi_OBJECTS_DEBUG )
set(libffi_COMPILE_DEFINITIONS_DEBUG "FFI_BUILDING")
set(libffi_COMPILE_OPTIONS_C_DEBUG )
set(libffi_COMPILE_OPTIONS_CXX_DEBUG )
set(libffi_LIB_DIRS_DEBUG "${libffi_PACKAGE_FOLDER_DEBUG}/lib")
set(libffi_BIN_DIRS_DEBUG )
set(libffi_LIBRARY_TYPE_DEBUG STATIC)
set(libffi_IS_HOST_WINDOWS_DEBUG 0)
set(libffi_LIBS_DEBUG ffi)
set(libffi_SYSTEM_LIBS_DEBUG )
set(libffi_FRAMEWORK_DIRS_DEBUG )
set(libffi_FRAMEWORKS_DEBUG )
set(libffi_BUILD_DIRS_DEBUG )
set(libffi_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libffi_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libffi_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libffi_COMPILE_OPTIONS_C_DEBUG}>")
set(libffi_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libffi_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libffi_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libffi_EXE_LINK_FLAGS_DEBUG}>")


set(libffi_COMPONENTS_DEBUG )