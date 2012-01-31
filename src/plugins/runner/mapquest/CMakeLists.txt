PROJECT( MapQuestPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/mapquest
 ${CMAKE_BINARY_DIR}/src/plugins/runner/mapquest
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( mapquest_SRCS MapQuestRunner.cpp MapQuestPlugin.cpp )

set( mapquest_UI MapQuestConfigWidget.ui )

qt4_wrap_ui( mapquest_SRCS ${mapquest_UI} )

marble_add_plugin( MapQuestPlugin ${mapquest_SRCS} )
