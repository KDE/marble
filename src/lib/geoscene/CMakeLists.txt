#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

SET( geoscene_data_SRCS
        geoscene/data/GeoDataMapTheme.cpp
#        geoscene/data/GeoSceneObject.cpp
   )

SET( geoscene_parser_SRCS
        geoscene/parser/GeoSceneParser.cpp
        geoscene/parser/GeoSceneTagHandler.cpp
   )

SET( geoscene_handlers_dgml_SRCS
        geoscene/handlers/dgml/DGMLDocumentTagHandler.cpp
        geoscene/handlers/dgml/DGMLFolderTagHandler.cpp
        geoscene/handlers/dgml/DGMLElementDictionary.cpp
   )

SET( geoscene_SRCS
        ${geoscene_data_SRCS}
        ${geoscene_parser_SRCS}
        ${geoscene_handlers_dgml_SRCS}
        geoscene/GeoSceneTest.cpp
   )

add_executable(geotest ${geoscene_SRCS})
target_link_libraries (geotest ${QT_QTCORE_LIBRARY} marblewidget)

install (TARGETS geotest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
