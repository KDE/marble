include_directories( ${KOFFICE_INCLUDE_DIR}
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     )

########### Flake Plugin library ###############

SET (geoshape_LIB_SRCS
    GeoShape.cpp
    GeoShapeFactory.cpp
    GeoTool.cpp
    GeoToolFactory.cpp
    Plugin.cpp
)
#linker error with "undefined reference to `ViewParams::viewport()'"
kde4_add_plugin(geoshape ${geoshape_LIB_SRCS})

target_link_libraries(geoshape marblewidget ${KSTORE_LIBRARY} ${KOODF_LIBRARY} ${FLAKE_LIBRARY} ${KOGUIUTILS_LIBRARY} ${KOTEXT_LIBRARY} ${KOMAIN_LIBRARY})
install(TARGETS geoshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES geoshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})

##kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
