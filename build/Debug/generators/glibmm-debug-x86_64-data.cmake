########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND glibmm_COMPONENT_NAMES glibmm::glibmm-2.68 glibmm::giomm-2.68)
list(REMOVE_DUPLICATES glibmm_COMPONENT_NAMES)
if(DEFINED glibmm_FIND_DEPENDENCY_NAMES)
  list(APPEND glibmm_FIND_DEPENDENCY_NAMES glib sigc++-3)
  list(REMOVE_DUPLICATES glibmm_FIND_DEPENDENCY_NAMES)
else()
  set(glibmm_FIND_DEPENDENCY_NAMES glib sigc++-3)
endif()
set(glib_FIND_MODE "NO_MODULE")
set(sigc++-3_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(glibmm_PACKAGE_FOLDER_DEBUG "/home/ejago/.conan2/p/b/glibm534a5eb15e339/p")
set(glibmm_BUILD_MODULES_PATHS_DEBUG )


set(glibmm_INCLUDE_DIRS_DEBUG )
set(glibmm_RES_DIRS_DEBUG )
set(glibmm_DEFINITIONS_DEBUG )
set(glibmm_SHARED_LINK_FLAGS_DEBUG )
set(glibmm_EXE_LINK_FLAGS_DEBUG )
set(glibmm_OBJECTS_DEBUG )
set(glibmm_COMPILE_DEFINITIONS_DEBUG )
set(glibmm_COMPILE_OPTIONS_C_DEBUG )
set(glibmm_COMPILE_OPTIONS_CXX_DEBUG )
set(glibmm_LIB_DIRS_DEBUG "${glibmm_PACKAGE_FOLDER_DEBUG}/lib")
set(glibmm_BIN_DIRS_DEBUG )
set(glibmm_LIBRARY_TYPE_DEBUG STATIC)
set(glibmm_IS_HOST_WINDOWS_DEBUG 0)
set(glibmm_LIBS_DEBUG giomm-2.68 glibmm-2.68)
set(glibmm_SYSTEM_LIBS_DEBUG )
set(glibmm_FRAMEWORK_DIRS_DEBUG )
set(glibmm_FRAMEWORKS_DEBUG )
set(glibmm_BUILD_DIRS_DEBUG )
set(glibmm_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(glibmm_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${glibmm_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${glibmm_COMPILE_OPTIONS_C_DEBUG}>")
set(glibmm_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${glibmm_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${glibmm_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${glibmm_EXE_LINK_FLAGS_DEBUG}>")


set(glibmm_COMPONENTS_DEBUG glibmm::glibmm-2.68 glibmm::giomm-2.68)
########### COMPONENT glibmm::giomm-2.68 VARIABLES ############################################

set(glibmm_glibmm_giomm-2.68_INCLUDE_DIRS_DEBUG )
set(glibmm_glibmm_giomm-2.68_LIB_DIRS_DEBUG "${glibmm_PACKAGE_FOLDER_DEBUG}/lib")
set(glibmm_glibmm_giomm-2.68_BIN_DIRS_DEBUG )
set(glibmm_glibmm_giomm-2.68_LIBRARY_TYPE_DEBUG STATIC)
set(glibmm_glibmm_giomm-2.68_IS_HOST_WINDOWS_DEBUG 0)
set(glibmm_glibmm_giomm-2.68_RES_DIRS_DEBUG )
set(glibmm_glibmm_giomm-2.68_DEFINITIONS_DEBUG )
set(glibmm_glibmm_giomm-2.68_OBJECTS_DEBUG )
set(glibmm_glibmm_giomm-2.68_COMPILE_DEFINITIONS_DEBUG )
set(glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_C_DEBUG "")
set(glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_CXX_DEBUG "")
set(glibmm_glibmm_giomm-2.68_LIBS_DEBUG giomm-2.68)
set(glibmm_glibmm_giomm-2.68_SYSTEM_LIBS_DEBUG )
set(glibmm_glibmm_giomm-2.68_FRAMEWORK_DIRS_DEBUG )
set(glibmm_glibmm_giomm-2.68_FRAMEWORKS_DEBUG )
set(glibmm_glibmm_giomm-2.68_DEPENDENCIES_DEBUG glibmm::glibmm-2.68 glib::gio-2.0)
set(glibmm_glibmm_giomm-2.68_SHARED_LINK_FLAGS_DEBUG )
set(glibmm_glibmm_giomm-2.68_EXE_LINK_FLAGS_DEBUG )
set(glibmm_glibmm_giomm-2.68_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(glibmm_glibmm_giomm-2.68_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${glibmm_glibmm_giomm-2.68_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${glibmm_glibmm_giomm-2.68_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${glibmm_glibmm_giomm-2.68_EXE_LINK_FLAGS_DEBUG}>
)
set(glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${glibmm_glibmm_giomm-2.68_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT glibmm::glibmm-2.68 VARIABLES ############################################

set(glibmm_glibmm_glibmm-2.68_INCLUDE_DIRS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_LIB_DIRS_DEBUG "${glibmm_PACKAGE_FOLDER_DEBUG}/lib")
set(glibmm_glibmm_glibmm-2.68_BIN_DIRS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_LIBRARY_TYPE_DEBUG STATIC)
set(glibmm_glibmm_glibmm-2.68_IS_HOST_WINDOWS_DEBUG 0)
set(glibmm_glibmm_glibmm-2.68_RES_DIRS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_DEFINITIONS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_OBJECTS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_COMPILE_DEFINITIONS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_C_DEBUG "")
set(glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_CXX_DEBUG "")
set(glibmm_glibmm_glibmm-2.68_LIBS_DEBUG glibmm-2.68)
set(glibmm_glibmm_glibmm-2.68_SYSTEM_LIBS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_FRAMEWORK_DIRS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_FRAMEWORKS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_DEPENDENCIES_DEBUG glib::gobject-2.0 sigc-3.0)
set(glibmm_glibmm_glibmm-2.68_SHARED_LINK_FLAGS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_EXE_LINK_FLAGS_DEBUG )
set(glibmm_glibmm_glibmm-2.68_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(glibmm_glibmm_glibmm-2.68_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${glibmm_glibmm_glibmm-2.68_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${glibmm_glibmm_glibmm-2.68_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${glibmm_glibmm_glibmm-2.68_EXE_LINK_FLAGS_DEBUG}>
)
set(glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${glibmm_glibmm_glibmm-2.68_COMPILE_OPTIONS_C_DEBUG}>")