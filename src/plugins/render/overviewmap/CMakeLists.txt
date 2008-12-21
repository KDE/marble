PROJECT(MarbleOverviewMap)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/render/overviewmap
 ${CMAKE_BINARY_DIR}/src/plugins/render/overviewmap
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marbleoverviewmap_SRCS MarbleOverviewMap.cpp )

marble_add_plugin( MarbleOverviewMap ${marbleoverviewmap_SRCS} )
