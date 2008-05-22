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
        geodata/data/GeoDataGeometry.cpp
        geodata/data/GeoDataHotSpot.cpp
        geodata/data/GeoDataIconStyle.cpp
        geodata/data/GeoDataLabelStyle.cpp
        geodata/data/GeoDataLineStyle.cpp
        geodata/data/GeoDataPolyStyle.cpp
        geodata/data/GeoDataLatLonAltBox.cpp
        geodata/data/GeoDataLinearRing.cpp
        geodata/data/GeoDataLineString.cpp
        geodata/data/GeoDataObject.cpp
        geodata/data/GeoDataPlacemark.cpp
        geodata/data/GeoDataPoint.cpp
        geodata/data/GeoDataStyle.cpp
        geodata/data/GeoDataStyleSelector.cpp
    )

SET( geodata_scene_SRCS
        geodata/scene/GeoSceneDocument.cpp
        geodata/scene/GeoSceneGroup.cpp
        geodata/scene/GeoSceneHead.cpp
        geodata/scene/GeoSceneIcon.cpp
        geodata/scene/GeoSceneItem.cpp
        geodata/scene/GeoSceneLayer.cpp
        geodata/scene/GeoSceneLegend.cpp
        geodata/scene/GeoSceneMap.cpp
        geodata/scene/GeoSceneProperty.cpp
        geodata/scene/GeoSceneSection.cpp
        geodata/scene/GeoSceneSettings.cpp
        geodata/scene/GeoSceneTexture.cpp
        geodata/scene/GeoSceneVector.cpp
        geodata/scene/GeoSceneZoom.cpp
   )

SET( geodata_parser_SRCS
        geodata/parser/GeoDataParser.cpp
        geodata/parser/GeoDocument.cpp
        geodata/parser/GeoParser.cpp
        geodata/parser/GeoSceneParser.cpp
        geodata/parser/GeoTagHandler.cpp
   )

SET( geodata_handlers_dgml_SRCS
        geodata/handlers/dgml/DgmlAttributeDictionary.cpp
        geodata/handlers/dgml/DgmlAuxillaryDictionary.cpp
        geodata/handlers/dgml/DgmlElementDictionary.cpp
        geodata/handlers/dgml/DgmlAvailableTagHandler.cpp
        geodata/handlers/dgml/DgmlBrushTagHandler.cpp
        geodata/handlers/dgml/DgmlDescriptionTagHandler.cpp
        geodata/handlers/dgml/DgmlDiscreteTagHandler.cpp
        geodata/handlers/dgml/DgmlDocumentTagHandler.cpp
        geodata/handlers/dgml/DgmlDownloadUrlTagHandler.cpp
        geodata/handlers/dgml/DgmlGroupTagHandler.cpp
        geodata/handlers/dgml/DgmlHeadTagHandler.cpp
        geodata/handlers/dgml/DgmlHeadingTagHandler.cpp
        geodata/handlers/dgml/DgmlIconTagHandler.cpp
        geodata/handlers/dgml/DgmlInstallMapTagHandler.cpp
        geodata/handlers/dgml/DgmlItemTagHandler.cpp
        geodata/handlers/dgml/DgmlLayerTagHandler.cpp
        geodata/handlers/dgml/DgmlLegendTagHandler.cpp
        geodata/handlers/dgml/DgmlMapTagHandler.cpp
        geodata/handlers/dgml/DgmlMaximumTagHandler.cpp
        geodata/handlers/dgml/DgmlMinimumTagHandler.cpp
        geodata/handlers/dgml/DgmlNameTagHandler.cpp
        geodata/handlers/dgml/DgmlPenTagHandler.cpp
        geodata/handlers/dgml/DgmlProjectionTagHandler.cpp
        geodata/handlers/dgml/DgmlPropertyTagHandler.cpp
        geodata/handlers/dgml/DgmlSettingsTagHandler.cpp
        geodata/handlers/dgml/DgmlSectionTagHandler.cpp
        geodata/handlers/dgml/DgmlSourceDirTagHandler.cpp
        geodata/handlers/dgml/DgmlSourceFileTagHandler.cpp
        geodata/handlers/dgml/DgmlStorageLayoutTagHandler.cpp
        geodata/handlers/dgml/DgmlTargetTagHandler.cpp
        geodata/handlers/dgml/DgmlTextTagHandler.cpp
        geodata/handlers/dgml/DgmlTextureTagHandler.cpp
        geodata/handlers/dgml/DgmlThemeTagHandler.cpp
        geodata/handlers/dgml/DgmlValueTagHandler.cpp
        geodata/handlers/dgml/DgmlVectorTagHandler.cpp
        geodata/handlers/dgml/DgmlVisibleTagHandler.cpp
        geodata/handlers/dgml/DgmlZoomTagHandler.cpp
   )

SET( geodata_handlers_gpx_SRCS
        geodata/handlers/gpx/GPXElementDictionary.cpp
        geodata/handlers/gpx/GPXgpxTagHandler.cpp
   )

SET( geodata_handlers_kml_SRCS
        geodata/handlers/kml/KMLColorModeTagHandler.cpp
        geodata/handlers/kml/KMLColorTagHandler.cpp
        geodata/handlers/kml/KMLCoordinatesTagHandler.cpp
        geodata/handlers/kml/KMLDocumentTagHandler.cpp
        geodata/handlers/kml/KMLElementDictionary.cpp
        geodata/handlers/kml/KMLFillTagHandler.cpp
        geodata/handlers/kml/KMLFolderTagHandler.cpp
        geodata/handlers/kml/KMLHeadingTagHandler.cpp
        geodata/handlers/kml/KMLHotSpotTagHandler.cpp
        geodata/handlers/kml/KMLHrefTagHandler.cpp
        geodata/handlers/kml/KMLIconStyleTagHandler.cpp
        geodata/handlers/kml/KMLIconTagHandler.cpp
        geodata/handlers/kml/KMLLabelStyleTagHandler.cpp
        geodata/handlers/kml/KMLLineStyleTagHandler.cpp
        geodata/handlers/kml/KMLOutlineTagHandler.cpp
        geodata/handlers/kml/KMLPlacemarkTagHandler.cpp
        geodata/handlers/kml/KMLPointTagHandler.cpp
        geodata/handlers/kml/KMLPolyStyleTagHandler.cpp
        geodata/handlers/kml/KMLStyleTagHandler.cpp
        geodata/handlers/kml/KMLWidthTagHandler.cpp
        geodata/handlers/kml/KML_scaleTagHandler.cpp
   )

SET( geodata_SRCS
        ${geodata_data_SRCS}
        ${geodata_scene_SRCS} 
        ${geodata_parser_SRCS}
        ${geodata_handlers_gpx_SRCS}
        ${geodata_handlers_kml_SRCS}
        ${geodata_handlers_dgml_SRCS}
   )


