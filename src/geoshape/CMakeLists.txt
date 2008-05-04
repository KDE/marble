include_directories( ${KOGUIUTILS_INCLUDES} )

########### Flake Plugin library ###############

SET (geoshape_LIB_SRCS
    GeoShape.cpp
    GeoShapeFactory.cpp
    GeoTool.cpp
    GeoToolFactory.cpp
    Plugin.cpp
)

kde4_add_plugin(geoshape ${geoshape_LIB_SRCS})

target_link_libraries(geoshape flake koguiutils marblewidget)
install(TARGETS geoshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES geoshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
