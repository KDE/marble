PROJECT( MarbleWidgetPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE( ${QT_USE_FILE} )

set( marbleplugin_SRCS MarbleWidgetPlugin.cpp )

marble_add_designer_plugin( MarbleWidgetPlugin ${marbleplugin_SRCS} )
