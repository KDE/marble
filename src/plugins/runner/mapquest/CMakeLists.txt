PROJECT( MapQuestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( mapquest_SRCS MapQuestRunner.cpp MapQuestPlugin.cpp )

set( mapquest_UI MapQuestConfigWidget.ui )

qt4_wrap_ui( mapquest_SRCS ${mapquest_UI} )

marble_add_plugin( MapQuestPlugin ${mapquest_SRCS} )
