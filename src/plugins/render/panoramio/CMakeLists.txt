CPROJECT(Panoramio)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( panoramio_SRCS PanoramioPlugin.cpp
                    PanoramioModel.cpp
                    PanoramioWidget.cpp
                    jsonparser.cpp )

marble_add_plugin( Panoramio ${panoramio_SRCS} )
target_link_libraries( Panoramio ${QT_QTSCRIPT_LIBRARY} )
