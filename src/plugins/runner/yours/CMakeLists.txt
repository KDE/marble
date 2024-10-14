PROJECT( YoursPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(yours_SRCS YoursRunner.cpp YoursPlugin.cpp YoursRunner.h YoursPlugin.h)

set(yours_UI YoursConfigWidget.ui)

qt_wrap_ui( yours_SRCS ${yours_UI})

marble_add_plugin( YoursPlugin ${yours_SRCS})
