FILE( GLOB geodata_data_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/data/*.cpp )
FILE( GLOB geodata_graphicsitem_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/graphicsitem/*.cpp )
FILE( GLOB geodata_scene_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/scene/*.cpp )

# handlers and writers sources 
FILE( GLOB geodata_handlers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/dgml/*.cpp )
FILE( GLOB geodata_handlers_osm_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/osm/*.cpp )
FILE( GLOB geodata_writers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/writers/dgml/*.cpp )

SET ( geodata_handlers_kml_SRCS
geodata/handlers/kml/KmlAltitudeModeTagHandler.cpp
geodata/handlers/kml/KmlAltitudeTagHandler.cpp
geodata/handlers/kml/KmlAreaTagHandler.cpp
geodata/handlers/kml/KmlBalloonStyleTagHandler.cpp
geodata/handlers/kml/KmlBeginTagHandler.cpp
geodata/handlers/kml/KmlBgColorTagHandler.cpp
geodata/handlers/kml/KmlColorModeTagHandler.cpp
geodata/handlers/kml/KmlColorTagHandler.cpp
geodata/handlers/kml/KmlCoordinatesTagHandler.cpp
geodata/handlers/kml/KmlCountrycodeTagHandler.cpp
geodata/handlers/kml/KmlCountryNameCodeTagHandler.cpp
geodata/handlers/kml/KmlDataTagHandler.cpp
geodata/handlers/kml/KmlDescriptionTagHandler.cpp
geodata/handlers/kml/KmlDisplayModeTagHandler.cpp
geodata/handlers/kml/KmlDisplayNameTagHandler.cpp
geodata/handlers/kml/KmlDocumentTagHandler.cpp
geodata/handlers/kml/KmlEastTagHandler.cpp
geodata/handlers/kml/KmlElementDictionary.cpp
geodata/handlers/kml/KmlEndTagHandler.cpp
geodata/handlers/kml/KmlExtendedDataTagHandler.cpp
geodata/handlers/kml/KmlExtrudeTagHandler.cpp
geodata/handlers/kml/KmlFillTagHandler.cpp
geodata/handlers/kml/KmlFolderTagHandler.cpp
geodata/handlers/kml/KmlGroundOverlayTagHandler.cpp
geodata/handlers/kml/KmlHeadingTagHandler.cpp
geodata/handlers/kml/KmlHotSpotTagHandler.cpp
geodata/handlers/kml/KmlHrefTagHandler.cpp
geodata/handlers/kml/KmlIconStyleTagHandler.cpp
geodata/handlers/kml/KmlIconTagHandler.cpp
geodata/handlers/kml/KmlInnerBoundaryIsTagHandler.cpp
geodata/handlers/kml/KmlItemIconTagHandler.cpp
geodata/handlers/kml/KmlKeyTagHandler.cpp
geodata/handlers/kml/KmlLabelStyleTagHandler.cpp
geodata/handlers/kml/KmlLatitudeTagHandler.cpp
geodata/handlers/kml/KmlLatLonAltBoxTagHandler.cpp
geodata/handlers/kml/KmlLatLonBoxTagHandler.cpp
geodata/handlers/kml/KmlLinearRingTagHandler.cpp
geodata/handlers/kml/KmlLineStringTagHandler.cpp
geodata/handlers/kml/KmlLineStyleTagHandler.cpp
geodata/handlers/kml/KmlListItemTypeTagHandler.cpp
geodata/handlers/kml/KmlListStyleTagHandler.cpp
geodata/handlers/kml/KmlLodTagHandler.cpp
geodata/handlers/kml/KmlLongitudeTagHandler.cpp
geodata/handlers/kml/KmlLookAtTagHandler.cpp
geodata/handlers/kml/KmlMarblePlacemarkTagHandler.cpp
geodata/handlers/kml/KmlMaxAltitudeTagHandler.cpp
geodata/handlers/kml/KmlMaxFadeExtentTagHandler.cpp
geodata/handlers/kml/KmlMaxLodPixelsTagHandler.cpp
geodata/handlers/kml/KmlMinAltitudeTagHandler.cpp
geodata/handlers/kml/KmlMinFadeExtentTagHandler.cpp
geodata/handlers/kml/KmlMinLodPixelsTagHandler.cpp
geodata/handlers/kml/KmlMultiGeometryTagHandler.cpp
geodata/handlers/kml/KmlMultiTrackTagHandler.cpp
geodata/handlers/kml/KmlNameTagHandler.cpp
geodata/handlers/kml/KmlNorthTagHandler.cpp
geodata/handlers/kml/KmlOpenTagHandler.cpp
geodata/handlers/kml/KmlOuterBoundaryIsTagHandler.cpp
geodata/handlers/kml/KmlOutlineTagHandler.cpp
geodata/handlers/kml/KmlOverlayXYTagHandler.cpp
geodata/handlers/kml/KmlPairTagHandler.cpp
geodata/handlers/kml/KmlPhotoOverlayTagHandler.cpp
geodata/handlers/kml/KmlPlacemarkTagHandler.cpp
geodata/handlers/kml/KmlPointTagHandler.cpp
geodata/handlers/kml/KmlPolygonTagHandler.cpp
geodata/handlers/kml/KmlPolyStyleTagHandler.cpp
geodata/handlers/kml/KmlPopTagHandler.cpp
geodata/handlers/kml/KmlRangeTagHandler.cpp
geodata/handlers/kml/KmlRegionTagHandler.cpp
geodata/handlers/kml/KmlRoleTagHandler.cpp
geodata/handlers/kml/KmlRotationTagHandler.cpp
geodata/handlers/kml/KmlRotationXYTagHandler.cpp
geodata/handlers/kml/Kml_scaleTagHandler.cpp
geodata/handlers/kml/KmlSchemaDataTagHandler.cpp
geodata/handlers/kml/KmlSchemaTagHandler.cpp
geodata/handlers/kml/KmlScreenOverlayTagHandler.cpp
geodata/handlers/kml/KmlScreenXYTagHandler.cpp
geodata/handlers/kml/KmlSimpleArrayDataTagHandler.cpp
geodata/handlers/kml/KmlSimpleFieldTagHandler.cpp
geodata/handlers/kml/KmlSizeTagHandler.cpp
geodata/handlers/kml/KmlSouthTagHandler.cpp
geodata/handlers/kml/KmlStateTagHandler.cpp
geodata/handlers/kml/KmlStyleMapTagHandler.cpp
geodata/handlers/kml/KmlStyleTagHandler.cpp
geodata/handlers/kml/KmlStyleUrlTagHandler.cpp
geodata/handlers/kml/KmlTessellateTagHandler.cpp
geodata/handlers/kml/KmlTextColorTagHandler.cpp
geodata/handlers/kml/KmlTextTagHandler.cpp
geodata/handlers/kml/KmlTimeSpanTagHandler.cpp
geodata/handlers/kml/KmlTimeStampTagHandler.cpp
geodata/handlers/kml/KmlTrackTagHandler.cpp
geodata/handlers/kml/KmlValueTagHandler.cpp
geodata/handlers/kml/KmlVisibilityTagHandler.cpp
geodata/handlers/kml/KmlWestTagHandler.cpp
geodata/handlers/kml/KmlWhenTagHandler.cpp
geodata/handlers/kml/KmlWidthTagHandler.cpp
)

SET ( geodata_writers_kml_SRCS 
geodata/writers/kml/KmlColorStyleTagWriter.cpp
geodata/writers/kml/KmlDataTagWriter.cpp
geodata/writers/kml/KmlDocumentTagWriter.cpp
geodata/writers/kml/KmlExtendedDataTagWriter.cpp
geodata/writers/kml/KmlFolderTagWriter.cpp
geodata/writers/kml/KmlGroundOverlayWriter.cpp
geodata/writers/kml/KmlIconStyleTagWriter.cpp
geodata/writers/kml/KmlLatLonBoxWriter.cpp
geodata/writers/kml/KmlLinearRingTagWriter.cpp
geodata/writers/kml/KmlLineStringTagWriter.cpp
geodata/writers/kml/KmlLineStyleTagWriter.cpp
geodata/writers/kml/KmlLookAtTagWriter.cpp
geodata/writers/kml/KmlMultiGeometryTagWriter.cpp
geodata/writers/kml/KmlMultiTrackTagWriter.cpp
geodata/writers/kml/KmlOverlayTagWriter.cpp
geodata/writers/kml/KmlPhotoOverlayWriter.cpp
geodata/writers/kml/KmlPlacemarkTagWriter.cpp
geodata/writers/kml/KmlPointTagWriter.cpp
geodata/writers/kml/KmlPolygonTagWriter.cpp
geodata/writers/kml/KmlStyleMapTagWriter.cpp
geodata/writers/kml/KmlStyleTagWriter.cpp
geodata/writers/kml/KmlTagWriter.cpp
geodata/writers/kml/KmlTimeSpanWriter.cpp
geodata/writers/kml/KmlTimeStampTagWriter.cpp
geodata/writers/kml/KmlTrackWriter.cpp
)

# writer and the parser sources 
SET ( geodata_parser_SRCS
        geodata/parser/GeoDataParser.cpp
        geodata/parser/GeoDataTypes.cpp
        geodata/parser/GeoDocument.cpp
        geodata/parser/GeoOnfParser.cpp
        geodata/parser/GeoParser.cpp
        geodata/parser/GeoSceneParser.cpp
        geodata/parser/GeoSceneTypes.cpp
        geodata/parser/GeoTagHandler.cpp
   )

SET( geodata_writer_SRCS
        geodata/writer/GeoTagWriter.cpp
        geodata/writer/GeoWriter.cpp
   )

SET( geodata_handlers_osmnamefinder_SRCS
        geodata/handlers/osm-namefinder/AttributeDictionary.cpp
        geodata/handlers/osm-namefinder/ElementDictionary.cpp
        geodata/handlers/osm-namefinder/DescriptionTagHandler.cpp
        geodata/handlers/osm-namefinder/NamedTagHandler.cpp
        geodata/handlers/osm-namefinder/NearestPlacesTagHandler.cpp
        geodata/handlers/osm-namefinder/PlaceTagHandler.cpp
        geodata/handlers/osm-namefinder/SearchResultsTagHandler.cpp
   )

SET( geodata_SRCS
        ${geodata_data_SRCS}
        ${geodata_graphicsitem_SRCS}
        ${geodata_scene_SRCS} 
        ${geodata_parser_SRCS}
        ${geodata_writer_SRCS}
        ${geodata_handlers_kml_SRCS}
        ${geodata_handlers_dgml_SRCS}
        ${geodata_handlers_osm_SRCS}
        ${geodata_handlers_osmnamefinder_SRCS}
        ${geodata_writers_kml_SRCS}
        ${geodata_writers_dgml_SRCS}
   )

#add_subdirectory(geodata/data/tests)

