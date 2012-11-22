PROJECT(NavigationFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( navigation_SRCS NavigationFloatItem.cpp )
set (navigation_UI navigation.ui navigation_small.ui)

qt4_add_resources(navigation_SRCS navigation.qrc)

if (QTONLY)
  qt4_wrap_ui(navigation_SRCS ${navigation_UI})
  qt4_automoc(${navigation_SRCS})
else (QTONLY)
  include_directories (${KDE4_INCLUDES})
  kde4_add_ui_files(navigation_SRCS ${navigation_UI})
endif (QTONLY)

marble_add_plugin( NavigationFloatItem ${navigation_SRCS} )
