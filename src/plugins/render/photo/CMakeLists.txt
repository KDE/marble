PROJECT(Photo)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( photo_SRCS PhotoPlugin.cpp
                PhotoPluginModel.cpp
                PhotoPluginItem.cpp
                FlickrParser.cpp
                CoordinatesParser.cpp )
set( photo_UI PhotoConfigWidget.ui )

qt4_wrap_ui(photo_SRCS ${photo_UI})

marble_add_plugin( Photo ${photo_SRCS} )
target_link_libraries( Photo ${QT_QTGUI_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
