PROJECT( GosmorePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/gosmore
 ${CMAKE_BINARY_DIR}/src/plugins/runner/gosmore
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( gosmore_SRCS GosmoreRunner.cpp GosmorePlugin.cpp )

marble_add_plugin( GosmorePlugin ${gosmore_SRCS} )
