PROJECT( OverviewMap)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(overviewmap_SRCS OverviewMap.cpp OverviewMap.h)
set(overviewmap_UI OverviewMapConfigWidget.ui)

qt_wrap_ui(overviewmap_SRCS ${overviewmap_UI})

set(OverviewMap_LIBS Qt6::Svg Qt6::SvgWidgets)

marble_add_plugin( OverviewMap ${overviewmap_SRCS})
