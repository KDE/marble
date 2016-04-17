PROJECT(Panoramio)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( panoramio_SRCS PanoramioPlugin.cpp
                    PanoramioModel.cpp
                    PanoramioItem.cpp
                    PanoramioParser.cpp )

qt_add_resources( panoramio_SRCS panoramio.qrc )

marble_add_plugin( Panoramio ${panoramio_SRCS} )
