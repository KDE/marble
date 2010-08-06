PROJECT( RoutinoPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/routino
 ${CMAKE_BINARY_DIR}/src/plugins/runner/routino
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( routino_SRCS RoutinoRunner.cpp RoutinoPlugin.cpp )

marble_add_plugin( RoutinoPlugin ${routino_SRCS} )
