PROJECT(Wikipedia)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
  INCLUDE_DIRECTORIES(${Qt5WebKitWidgets_INCLUDE_DIRS})
endif()

set( wikipedia_SRCS WikipediaPlugin.cpp
                    WikipediaModel.cpp
                    WikipediaItem.cpp
                    GeonamesParser.cpp )
set( wikipedia_UI WikipediaConfigWidget.ui )

qt_wrap_ui(wikipedia_SRCS ${wikipedia_UI})

marble_add_plugin( Wikipedia ${wikipedia_SRCS} )
target_link_libraries( Wikipedia ${QT_QTWEBKIT_LIBRARY})
