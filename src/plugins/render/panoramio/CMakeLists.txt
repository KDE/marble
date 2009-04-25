PROJECT(Panoramio)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/panoramio
 ${CMAKE_BINARY_DIR}/src/plugins/render/panoramio
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( panoramio_SRCS PanoramioPlugin.cpp
                    PanoramioModel.cpp
                    PanoramioWidget.cpp
                    jsonparser.cpp
#                    imagewidget.cpp
                    AbstractDataPluginWidget.cpp
                    AbstractDataPluginModel.cpp
                    AbstractDataPlugin.cpp )

marble_add_plugin( Panoramio ${panoramio_SRCS} )
target_link_libraries( Panoramio ${QT_QTSCRIPT_LIBRARY} )
