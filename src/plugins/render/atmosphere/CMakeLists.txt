PROJECT( AtmospherePlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( atmosphere_SRCS AtmospherePlugin.cpp )

marble_add_plugin( AtmospherePlugin ${atmosphere_SRCS} )
