PROJECT( OpenDesktopPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(my_SRCS
  OpenDesktopPlugin.cpp
  OpenDesktopItem.cpp
  OpenDesktopModel.cpp
  OpenDesktopPlugin.h
  OpenDesktopItem.h
  OpenDesktopModel.h
)

set(opendesktop_UI OpenDesktopConfigWidget.ui)
qt_wrap_ui( my_SRCS ${opendesktop_UI})

marble_add_plugin( OpenDesktopPlugin ${my_SRCS})
