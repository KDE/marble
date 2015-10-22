PROJECT( MarbleWidgetPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE_DIRECTORIES(${Qt5Designer_INCLUDE_DIRS})

set( marbleplugin_SRCS MarbleWidgetPlugin.cpp )

marble_add_designer_plugin( MarbleWidgetPlugin ${marbleplugin_SRCS} )
