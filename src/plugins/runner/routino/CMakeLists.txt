PROJECT( RoutinoPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( routino_SRCS RoutinoRunner.cpp RoutinoPlugin.cpp )

set( routino_UI RoutinoConfigWidget.ui )

qt_wrap_ui( routino_SRCS ${routino_UI} )

marble_add_plugin( RoutinoPlugin ${routino_SRCS} )
