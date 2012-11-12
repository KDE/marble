PROJECT( StarsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( stars_SRCS StarsPlugin.cpp )

marble_add_plugin( StarsPlugin ${stars_SRCS} )
