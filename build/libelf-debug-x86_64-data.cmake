########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(libelf_COMPONENT_NAMES "")
if(DEFINED libelf_FIND_DEPENDENCY_NAMES)
  list(APPEND libelf_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES libelf_FIND_DEPENDENCY_NAMES)
else()
  set(libelf_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(libelf_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/libel34d2133516a72/p")
set(libelf_BUILD_MODULES_PATHS_DEBUG )


set(libelf_INCLUDE_DIRS_DEBUG )
set(libelf_RES_DIRS_DEBUG )
set(libelf_DEFINITIONS_DEBUG )
set(libelf_SHARED_LINK_FLAGS_DEBUG )
set(libelf_EXE_LINK_FLAGS_DEBUG )
set(libelf_OBJECTS_DEBUG )
set(libelf_COMPILE_DEFINITIONS_DEBUG )
set(libelf_COMPILE_OPTIONS_C_DEBUG )
set(libelf_COMPILE_OPTIONS_CXX_DEBUG )
set(libelf_LIB_DIRS_DEBUG "${libelf_PACKAGE_FOLDER_DEBUG}/lib")
set(libelf_BIN_DIRS_DEBUG )
set(libelf_LIBRARY_TYPE_DEBUG STATIC)
set(libelf_IS_HOST_WINDOWS_DEBUG 0)
set(libelf_LIBS_DEBUG elf)
set(libelf_SYSTEM_LIBS_DEBUG )
set(libelf_FRAMEWORK_DIRS_DEBUG )
set(libelf_FRAMEWORKS_DEBUG )
set(libelf_BUILD_DIRS_DEBUG )
set(libelf_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(libelf_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libelf_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libelf_COMPILE_OPTIONS_C_DEBUG}>")
set(libelf_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libelf_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libelf_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libelf_EXE_LINK_FLAGS_DEBUG}>")


set(libelf_COMPONENTS_DEBUG )