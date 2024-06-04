# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/libxmlpp-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${libxmlpp_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${libxmlpp_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET libxmlpp::libxmlpp)
    add_library(libxmlpp::libxmlpp INTERFACE IMPORTED)
    message(${libxmlpp_MESSAGE_MODE} "Conan: Target declared 'libxmlpp::libxmlpp'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/libxmlpp-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()