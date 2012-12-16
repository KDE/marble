#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

FILE( GLOB geodata_data_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/data/*.cpp )
FILE( GLOB geodata_graphicsitem_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/graphicsitem/*.cpp )
FILE( GLOB geodata_scene_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/scene/*.cpp )

# handlers and writers sources 
FILE( GLOB geodata_handlers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/dgml/*.cpp )
FILE( GLOB geodata_handlers_kml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/kml/*.cpp )
FILE( GLOB geodata_handlers_osm_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/osm/*.cpp )
FILE( GLOB geodata_writers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/writers/dgml/*.cpp )
FILE( GLOB geodata_writers_kml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/writers/kml/*.cpp )

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

