PROJECT( MapQuestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( mapquest_SRCS MapQuestRunner.cpp MapQuestPlugin.cpp )

set( mapquest_UI MapQuestConfigWidget.ui )

qt_wrap_ui( mapquest_SRCS ${mapquest_UI} )

marble_add_plugin( MapQuestPlugin ${mapquest_SRCS} )
