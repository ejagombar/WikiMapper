

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()




########## generic_system block #############
# Definition of system, platform and toolset
#############################################







string(APPEND CONAN_CXX_FLAGS " -m64")
string(APPEND CONAN_C_FLAGS " -m64")
string(APPEND CONAN_SHARED_LINKER_FLAGS " -m64")
string(APPEND CONAN_EXE_LINKER_FLAGS " -m64")



message(STATUS "Conan toolchain: C++ Standard 17 with extensions ON")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan conf flags start: 
# Conan conf flags end

foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${config} config)
    if(DEFINED CONAN_CXX_FLAGS_${config})
      string(APPEND CMAKE_CXX_FLAGS_${config}_INIT " ${CONAN_CXX_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_C_FLAGS_${config})
      string(APPEND CMAKE_C_FLAGS_${config}_INIT " ${CONAN_C_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_SHARED_LINKER_FLAGS_${config})
      string(APPEND CMAKE_SHARED_LINKER_FLAGS_${config}_INIT " ${CONAN_SHARED_LINKER_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_EXE_LINKER_FLAGS_${config})
      string(APPEND CMAKE_EXE_LINKER_FLAGS_${config}_INIT " ${CONAN_EXE_LINKER_FLAGS_${config}}")
    endif()
endforeach()

if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()


get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_LIBRARY_PATH "/home/ejago/.conan2/p/b/libxmcca057fc6e571/p/lib" "/home/ejago/.conan2/p/b/libxm47331dfc6f5fc/p/lib" "/home/ejago/.conan2/p/b/libic3a4d41bfd92d0/p/lib" "/home/ejago/.conan2/p/b/glibm534a5eb15e339/p/lib" "/home/ejago/.conan2/p/b/glib785c672db1af8/p/lib" "/home/ejago/.conan2/p/b/libff349591145e1b5/p/lib" "/home/ejago/.conan2/p/b/libel34d2133516a72/p/lib" "/home/ejago/.conan2/p/b/libmo14fec41e02336/p/lib" "/home/ejago/.conan2/p/b/libse6348ef6ba3b21/p/lib" "/home/ejago/.conan2/p/b/pcre28bfbee47083b2/p/lib" "/home/ejago/.conan2/p/b/zlib1753f82d10a89/p/lib" "/home/ejago/.conan2/p/b/bzip277add467baf54/p/lib" "/home/ejago/.conan2/p/b/libsic1112c30ab147/p/lib")
list(PREPEND CMAKE_INCLUDE_PATH "/home/ejago/.conan2/p/b/libxmcca057fc6e571/p/include/libxml++-5.0" "/home/ejago/.conan2/p/b/libxm47331dfc6f5fc/p/include" "/home/ejago/.conan2/p/b/libxm47331dfc6f5fc/p/include/libxml2" "/home/ejago/.conan2/p/b/libic3a4d41bfd92d0/p/include" "/home/ejago/.conan2/p/b/glibm534a5eb15e339/p/include/giomm-2.68" "/home/ejago/.conan2/p/b/glibm534a5eb15e339/p/include/glibmm-2.68" "/home/ejago/.conan2/p/b/glib785c672db1af8/p/include/gio-unix-2.0" "/home/ejago/.conan2/p/b/glib785c672db1af8/p/include" "/home/ejago/.conan2/p/b/glib785c672db1af8/p/include/glib-2.0" "/home/ejago/.conan2/p/b/glib785c672db1af8/p/lib/glib-2.0/include" "/home/ejago/.conan2/p/b/libff349591145e1b5/p/include" "/home/ejago/.conan2/p/b/libel34d2133516a72/p/include" "/home/ejago/.conan2/p/b/libel34d2133516a72/p/include/libelf" "/home/ejago/.conan2/p/b/libmo14fec41e02336/p/include" "/home/ejago/.conan2/p/b/libmo14fec41e02336/p/include/libmount" "/home/ejago/.conan2/p/b/libse6348ef6ba3b21/p/include" "/home/ejago/.conan2/p/b/pcre28bfbee47083b2/p/include" "/home/ejago/.conan2/p/b/zlib1753f82d10a89/p/include" "/home/ejago/.conan2/p/b/bzip277add467baf54/p/include" "/home/ejago/.conan2/p/b/libsic1112c30ab147/p/include/sigc++-3.0")



if (DEFINED ENV{PKG_CONFIG_PATH})
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR}:$ENV{PKG_CONFIG_PATH}")
else()
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR}:")
endif()




# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration


if(CMAKE_POLICY_DEFAULT_CMP0091)  # Avoid unused and not-initialized warnings
endif()
