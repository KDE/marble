PROJECT( RoutinoPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/runner/routino
 ${CMAKE_BINARY_DIR}/src/plugins/runner/routino
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( routino_SRCS RoutinoRunner.cpp RoutinoPlugin.cpp )

set( routino_UI RoutinoConfigWidget.ui )

qt4_wrap_ui( routino_SRCS ${routino_UI} )

marble_add_plugin( RoutinoPlugin ${routino_SRCS} )
