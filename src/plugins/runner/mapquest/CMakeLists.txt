PROJECT( MapQuestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
else()
INCLUDE_DIRECTORIES(
  ${Qt5WebKitWidgets_INCLUDE_DIRS}
  ${Qt5Xml_INCLUDE_DIRS}
)
endif()

set( mapquest_SRCS MapQuestRunner.cpp MapQuestPlugin.cpp )

set( mapquest_UI MapQuestConfigWidget.ui )

qt_wrap_ui( mapquest_SRCS ${mapquest_UI} )

marble_add_plugin( MapQuestPlugin ${mapquest_SRCS} )
