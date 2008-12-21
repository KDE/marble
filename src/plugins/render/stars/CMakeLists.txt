PROJECT(MarbleStarsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/stars
 ${CMAKE_BINARY_DIR}/src/plugins/marble/stars
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblestars_SRCS MarbleStarsPlugin.cpp )

marble_add_plugin( MarbleStarsPlugin ${marblestars_SRCS} )