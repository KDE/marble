IF(BUILD_WITH_GEOSHAPE)
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

target_link_libraries(geoshape marblewidget ${KOFFICE_LIBRARIES} kstore flake koguiutils koodf komain)
install(TARGETS geoshape DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES geoshape.desktop DESTINATION ${SERVICES_INSTALL_DIR})

##kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons )
ENDIF(BUILD_WITH_GEOSHAPE)