#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

SET( geodata_data_SRCS
        geodata/data/GeoDataColorStyle.cpp
        geodata/data/GeoDataContainer.cpp
        geodata/data/GeoDataDocument.cpp
        geodata/data/GeoDataFeature.cpp
        geodata/data/GeoDataFolder.cpp
        geodata/data/GeoDataHotSpot.cpp
        geodata/data/GeoDataIconStyle.cpp
        geodata/data/GeoDataLabelStyle.cpp
        geodata/data/GeoDataObject.cpp
        geodata/data/GeoDataPlacemark.cpp
        geodata/data/GeoDataPoint.cpp
        geodata/data/GeoDataStyle.cpp
        geodata/data/GeoDataStyleSelector.cpp
    )

SET( geodata_scene_SRCS
        geodata/scene/GeoSceneDocument.cpp
        geodata/scene/GeoSceneHead.cpp
        geodata/scene/GeoSceneIcon.cpp
        geodata/scene/GeoSceneZoom.cpp
        geodata/scene/GeoSceneMap.cpp
        geodata/scene/GeoSceneSettings.cpp
        geodata/scene/GeoSceneProperty.cpp
        geodata/scene/GeoSceneLegend.cpp
        geodata/scene/GeoSceneSection.cpp
        geodata/scene/GeoSceneItem.cpp
   )

SET( geodata_parser_SRCS
        geodata/parser/GeoParser.cpp
        geodata/parser/GeoDataParser.cpp
        geodata/parser/GeoSceneParser.cpp
        geodata/parser/GeoTagHandler.cpp
   )

SET( geodata_handlers_dgml_SRCS
        geodata/handlers/dgml/DGMLDocumentTagHandler.cpp
        geodata/handlers/dgml/DGMLHeadTagHandler.cpp
        geodata/handlers/dgml/DGMLMapTagHandler.cpp
        geodata/handlers/dgml/DGMLSettingsTagHandler.cpp
        geodata/handlers/dgml/DGMLLegendTagHandler.cpp
        geodata/handlers/dgml/DGMLSectionTagHandler.cpp
        geodata/handlers/dgml/DGMLItemTagHandler.cpp
        geodata/handlers/dgml/DGMLNameTagHandler.cpp
        geodata/handlers/dgml/DGMLTargetTagHandler.cpp
        geodata/handlers/dgml/DGMLThemeTagHandler.cpp
        geodata/handlers/dgml/DGMLIconTagHandler.cpp
        geodata/handlers/dgml/DGMLDescriptionTagHandler.cpp
        geodata/handlers/dgml/DGMLVisibleTagHandler.cpp
        geodata/handlers/dgml/DGMLZoomTagHandler.cpp
        geodata/handlers/dgml/DGMLDiscreteTagHandler.cpp
        geodata/handlers/dgml/DGMLMinimumTagHandler.cpp
        geodata/handlers/dgml/DGMLMaximumTagHandler.cpp
        geodata/handlers/dgml/DGMLPropertyTagHandler.cpp
        geodata/handlers/dgml/DGMLAvailableTagHandler.cpp
        geodata/handlers/dgml/DGMLEnabledTagHandler.cpp
        geodata/handlers/dgml/DGMLElementDictionary.cpp
        geodata/handlers/dgml/DGMLAttributeDictionary.cpp
        geodata/handlers/dgml/DGMLAuxillaryDictionary.cpp
   )

SET( geodata_handlers_gpx_SRCS
        geodata/handlers/gpx/GPXgpxTagHandler.cpp
        geodata/handlers/gpx/GPXElementDictionary.cpp
   )

SET( geodata_handlers_kml_SRCS
        geodata/handlers/kml/KMLDocumentTagHandler.cpp
        geodata/handlers/kml/KMLFolderTagHandler.cpp
        geodata/handlers/kml/KMLElementDictionary.cpp
   )

SET( geodata_SRCS
        ${geodata_data_SRCS}
        ${geodata_scene_SRCS} 
        ${geodata_parser_SRCS}
        ${geodata_handlers_gpx_SRCS}
        ${geodata_handlers_kml_SRCS}
        ${geodata_handlers_dgml_SRCS}
   )


