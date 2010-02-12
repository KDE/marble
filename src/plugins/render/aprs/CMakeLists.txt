PROJECT(AprsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/aprs
 ${CMAKE_BINARY_DIR}/src/plugins/render/aprs
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( aprs_SRCS AprsPlugin.cpp )

marble_add_plugin( AprsPlugin ${aprs_SRCS} )
