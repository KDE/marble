PROJECT( SunPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( sun_SRCS SunPlugin.cpp )

marble_add_plugin( SunPlugin ${sun_SRCS} )
