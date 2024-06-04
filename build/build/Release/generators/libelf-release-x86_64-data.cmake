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
set(libelf_PACKAGE_FOLDER_RELEASE "/home/ejago/.conan2/p/b/libel84a8652a714a4/p")
set(libelf_BUILD_MODULES_PATHS_RELEASE )


set(libelf_INCLUDE_DIRS_RELEASE )
set(libelf_RES_DIRS_RELEASE )
set(libelf_DEFINITIONS_RELEASE )
set(libelf_SHARED_LINK_FLAGS_RELEASE )
set(libelf_EXE_LINK_FLAGS_RELEASE )
set(libelf_OBJECTS_RELEASE )
set(libelf_COMPILE_DEFINITIONS_RELEASE )
set(libelf_COMPILE_OPTIONS_C_RELEASE )
set(libelf_COMPILE_OPTIONS_CXX_RELEASE )
set(libelf_LIB_DIRS_RELEASE "${libelf_PACKAGE_FOLDER_RELEASE}/lib")
set(libelf_BIN_DIRS_RELEASE )
set(libelf_LIBRARY_TYPE_RELEASE STATIC)
set(libelf_IS_HOST_WINDOWS_RELEASE 0)
set(libelf_LIBS_RELEASE elf)
set(libelf_SYSTEM_LIBS_RELEASE )
set(libelf_FRAMEWORK_DIRS_RELEASE )
set(libelf_FRAMEWORKS_RELEASE )
set(libelf_BUILD_DIRS_RELEASE )
set(libelf_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(libelf_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libelf_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libelf_COMPILE_OPTIONS_C_RELEASE}>")
set(libelf_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libelf_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libelf_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libelf_EXE_LINK_FLAGS_RELEASE}>")


set(libelf_COMPONENTS_RELEASE )