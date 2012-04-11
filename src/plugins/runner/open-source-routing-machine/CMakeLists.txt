PROJECT( OSRMPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/open-source-routing-machine
 ${CMAKE_BINARY_DIR}/src/plugins/runner/open-source-routing-machine
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( osrm_SRCS OSRMRunner.cpp OSRMPlugin.cpp )

marble_add_plugin( OSRMPlugin ${osrm_SRCS} )
