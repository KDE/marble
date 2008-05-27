#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

FILE( GLOB geodata_data_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/data/*.cpp )
FILE( GLOB geodata_scene_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/scene/*.cpp )
FILE( GLOB geodata_handlers_dgml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/dgml/*.cpp )
FILE( GLOB geodata_handlers_kml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} geodata/handlers/kml/*.cpp )

SET( geodata_parser_SRCS
        geodata/parser/GeoDataParser.cpp
        geodata/parser/GeoDocument.cpp
        geodata/parser/GeoParser.cpp
        geodata/parser/GeoSceneParser.cpp
        geodata/parser/GeoTagHandler.cpp
   )

SET( geodata_handlers_gpx_SRCS
        geodata/handlers/gpx/GPXElementDictionary.cpp
        geodata/handlers/gpx/GPXgpxTagHandler.cpp
   )


SET( geodata_SRCS
        ${geodata_data_SRCS}
        ${geodata_scene_SRCS} 
        ${geodata_parser_SRCS}
        ${geodata_handlers_gpx_SRCS}
        ${geodata_handlers_kml_SRCS}
        ${geodata_handlers_dgml_SRCS}
   )


