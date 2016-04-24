PROJECT(Wikipedia)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( wikipedia_SRCS WikipediaPlugin.cpp
                    WikipediaModel.cpp
                    WikipediaItem.cpp
                    GeonamesParser.cpp )
set( wikipedia_UI WikipediaConfigWidget.ui )

qt_wrap_ui(wikipedia_SRCS ${wikipedia_UI})

set( Wikipedia_LIBS Qt5::Svg )

marble_add_plugin( Wikipedia ${wikipedia_SRCS} )
