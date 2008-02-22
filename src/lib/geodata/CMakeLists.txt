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

add_executable(geotest ${geodata_SRCS})
target_link_libraries (geotest ${QT_QTCORE_LIBRARY} marblewidget)

install (TARGETS geotest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
