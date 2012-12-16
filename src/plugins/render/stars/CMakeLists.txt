PROJECT( StarsPlugin )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( stars_SRCS StarsPlugin.cpp )

if (QTONLY)
  qt4_wrap_ui(stars_SRCS StarsConfigWidget.ui)
else (QTONLY)
  include_directories (${KDE4_INCLUDES})
  kde4_add_ui_files(stars_SRCS StarsConfigWidget.ui)
endif (QTONLY)

marble_add_plugin( StarsPlugin ${stars_SRCS} )
