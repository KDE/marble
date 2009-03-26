PROJECT( StarsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/stars
 ${CMAKE_BINARY_DIR}/src/plugins/render/stars
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( stars_SRCS StarsPlugin.cpp )

marble_add_plugin( StarsPlugin ${stars_SRCS} )
