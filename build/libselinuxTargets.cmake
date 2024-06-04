# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/libselinux-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${libselinux_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${libselinux_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET libselinux::libselinux)
    add_library(libselinux::libselinux INTERFACE IMPORTED)
    message(${libselinux_MESSAGE_MODE} "Conan: Target declared 'libselinux::libselinux'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/libselinux-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()