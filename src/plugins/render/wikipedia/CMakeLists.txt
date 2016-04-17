PROJECT(Wikipedia)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE_DIRECTORIES(${Qt5WebKitWidgets_INCLUDE_DIRS})

set( wikipedia_SRCS WikipediaPlugin.cpp
                    WikipediaModel.cpp
                    WikipediaItem.cpp
                    GeonamesParser.cpp )
set( wikipedia_UI WikipediaConfigWidget.ui )

qt_wrap_ui(wikipedia_SRCS ${wikipedia_UI})

marble_add_plugin( Wikipedia ${wikipedia_SRCS} )
