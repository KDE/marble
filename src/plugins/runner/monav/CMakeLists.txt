PROJECT( MonavPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/monav
 ${CMAKE_BINARY_DIR}/src/plugins/runner/monav
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( monav_SRCS MonavRunner.cpp MonavPlugin.cpp )

marble_add_plugin( MonavPlugin ${monav_SRCS} )
