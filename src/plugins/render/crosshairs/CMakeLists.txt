PROJECT(MarbleCrosshairsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/crosshairs
 ${CMAKE_BINARY_DIR}/src/plugins/render/crosshairs
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblecrosshairs_SRCS MarbleCrosshairsPlugin.cpp )

marble_add_plugin( MarbleCrosshairsPlugin ${marblecrosshairs_SRCS} )
