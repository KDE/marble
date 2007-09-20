#
# Dummy CMakeLists.txt file.
# For now we just collect all the sources here and link
# them against the main application
#

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/marble/src/lib/geodata
 ${CMAKE_SOURCE_DIR}/marble/src/lib/geodata/data
 ${CMAKE_SOURCE_DIR}/marble/src/lib/geodata/handlers/gpx
 ${CMAKE_SOURCE_DIR}/marble/src/lib/geodata/handlers/kml
 ${CMAKE_SOURCE_DIR}/marble/src/lib/geodata/parser
)

SET( geodata_data_SRCS
          geodata/data/GeoDataDocument.cpp
          geodata/data/GeoDataFolder.cpp
   )

SET( geodata_parser_SRCS
          geodata/parser/GeoDataParser.cpp
          geodata/parser/GeoDataTagHandler.cpp
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
          ${geodata_parser_SRCS}
          ${geodata_handlers_gpx_SRCS}
          ${geodata_handlers_kml_SRCS}
          geodata/GeoDataTest.cpp
   )
