PROJECT( OverviewMap )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/overviewmap
 ${CMAKE_BINARY_DIR}/src/plugins/render/overviewmap
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( overviewmap_SRCS OverviewMap.cpp )
set( overviewmap_UI OverviewMapConfigWidget.ui )

qt4_wrap_ui(overviewmap_SRCS ${overviewmap_UI})
marble_add_plugin( OverviewMap ${overviewmap_SRCS} )
