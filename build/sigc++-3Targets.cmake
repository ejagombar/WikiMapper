# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/sigc++-3-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${libsigcpp_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${sigc++-3_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET sigc-3.0)
    add_library(sigc-3.0 INTERFACE IMPORTED)
    message(${sigc++-3_MESSAGE_MODE} "Conan: Target declared 'sigc-3.0'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/sigc++-3-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()