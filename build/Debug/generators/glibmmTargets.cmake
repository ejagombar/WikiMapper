# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/glibmm-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${glibmm_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${glibmm_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET glibmm::glibmm)
    add_library(glibmm::glibmm INTERFACE IMPORTED)
    message(${glibmm_MESSAGE_MODE} "Conan: Target declared 'glibmm::glibmm'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/glibmm-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()