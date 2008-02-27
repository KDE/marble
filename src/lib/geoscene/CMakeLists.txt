#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

SET( geoscene_data_SRCS
        geoscene/data/GeoSceneDocument.cpp
        geoscene/data/GeoSceneHead.cpp
        geoscene/data/GeoSceneMap.cpp
        geoscene/data/GeoSceneSettings.cpp
        geoscene/data/GeoSceneLegend.cpp
        geoscene/data/GeoSceneName.cpp
        geoscene/data/GeoSceneMapTheme.cpp
#        geoscene/data/GeoSceneObject.cpp
   )

SET( geoscene_parser_SRCS
        geoscene/parser/GeoSceneParser.cpp
        geoscene/parser/GeoSceneTagHandler.cpp
   )

SET( geoscene_handlers_dgml_SRCS
        geoscene/handlers/dgml/DGMLDocumentTagHandler.cpp
        geoscene/handlers/dgml/DGMLHeadTagHandler.cpp
        geoscene/handlers/dgml/DGMLMapTagHandler.cpp
        geoscene/handlers/dgml/DGMLSettingsTagHandler.cpp
        geoscene/handlers/dgml/DGMLLegendTagHandler.cpp
        geoscene/handlers/dgml/DGMLNameTagHandler.cpp
        geoscene/handlers/dgml/DGMLElementDictionary.cpp
        geoscene/handlers/dgml/DGMLAttributeDictionary.cpp
   )

SET( geoscene_SRCS
        ${geoscene_data_SRCS}
        ${geoscene_parser_SRCS}
        ${geoscene_handlers_dgml_SRCS}
        geoscene/GeoSceneTest.cpp
   )

add_executable(geoscenetest ${geoscene_SRCS})
target_link_libraries (geoscenetest ${QT_QTCORE_LIBRARY} marblewidget)

install (TARGETS geoscenetest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
