PROJECT(Wikipedia)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/wikipedia
 ${CMAKE_CURRENT_BUILD_DIR}/src/plugins/render/wikipedia
 ${CMAKE_BINARY_DIR}/src/plugins/render/wikipedia
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( wikipedia_SRCS WikipediaPlugin.cpp
                    WikipediaModel.cpp
                    WikipediaItem.cpp
                    GeonamesParser.cpp )
set( wikipedia_UI WikipediaConfigWidget.ui )

qt4_wrap_ui(wikipedia_SRCS ${wikipedia_UI})

marble_add_plugin( Wikipedia ${wikipedia_SRCS} )
target_link_libraries( Wikipedia ${QT_QTWEBKIT_LIBRARY})
