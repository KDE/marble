PROJECT(Wikipedia)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/wikipedia
 ${CMAKE_BINARY_DIR}/src/plugins/render/wikipedia
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( wikipedia_SRCS WikipediaPlugin.cpp
                    WikipediaModel.cpp
                    WikipediaWidget.cpp
                    GeonamesParser.cpp )

marble_add_plugin( Wikipedia ${wikipedia_SRCS} )
target_link_libraries( Wikipedia ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
