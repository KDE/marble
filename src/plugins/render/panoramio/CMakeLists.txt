PROJECT(Panoramio)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

set( panoramio_SRCS PanoramioPlugin.cpp
                    PanoramioModel.cpp
                    PanoramioWidget.cpp
                    jsonparser.cpp )

marble_add_plugin( Panoramio ${panoramio_SRCS} )
