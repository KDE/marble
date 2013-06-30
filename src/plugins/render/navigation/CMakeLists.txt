PROJECT(NavigationFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( navigation_SRCS NavigationFloatItem.cpp ArrowDiscWidget.cpp NavigationButton.cpp NavigationSlider.cpp )
set (navigation_UI navigation.ui)

qt4_add_resources(navigation_SRCS navigation.qrc)
qt4_wrap_ui(navigation_SRCS ${navigation_UI})
qt4_automoc(${navigation_SRCS})

marble_add_plugin( NavigationFloatItem ${navigation_SRCS} )
