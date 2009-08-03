#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

FILE( GLOB geodata_data_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/data/*.cpp )
FILE( GLOB geodata_graphicsitem_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/graphicsitem/*.cpp )
FILE( GLOB geodata_scene_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/scene/*.cpp )
FILE( GLOB geodata_handlers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/dgml/*.cpp )
FILE( GLOB geodata_handlers_kml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/kml/*.cpp )
FILE( GLOB geodata_writer_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/writer/*.cpp )

FILE( GLOB geodata_data_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/data/*.h )

SET( geodata_parser_SRCS
        geodata/parser/GeoDataParser.cpp
        geodata/parser/GeoDocument.cpp
        geodata/parser/GeoOnfParser.cpp
        geodata/parser/GeoParser.cpp
        geodata/parser/GeoSceneParser.cpp
        geodata/parser/GeoTagHandler.cpp
   )

SET( geodata_handlers_gpx_SRCS
        geodata/handlers/gpx/GPXElementDictionary.cpp
        geodata/handlers/gpx/GPXgpxTagHandler.cpp
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
        ${geodata_handlers_gpx_SRCS}
        ${geodata_handlers_kml_SRCS}
        ${geodata_handlers_dgml_SRCS}
        ${geodata_handlers_osmnamefinder_SRCS}
   )

#add_subdirectory(geodata/data/tests)
