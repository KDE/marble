PROJECT( OverviewMap )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( overviewmap_SRCS OverviewMap.cpp )
set( overviewmap_UI OverviewMapConfigWidget.ui )

qt_wrap_ui(overviewmap_SRCS ${overviewmap_UI})

set( OverviewMap_LIBS Qt5::Svg )

marble_add_plugin( OverviewMap ${overviewmap_SRCS} )
