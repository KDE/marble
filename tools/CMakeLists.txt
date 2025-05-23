find_package(ZLIB)

add_subdirectory(tilecreator)
add_subdirectory(tilecreator-srtm2)
add_subdirectory(dateline)
add_subdirectory(asc2kml)
add_subdirectory(constellations2kml)
add_subdirectory(dso2kml)
add_subdirectory(iau2kml)
add_subdirectory(kml2cache)
add_subdirectory(kml2kml)
add_subdirectory(mbtile-import)
add_subdirectory(poly2kml)
add_subdirectory(pnt2svg)
add_subdirectory(pntdel)
add_subdirectory(pntreplace)
add_subdirectory(shp2pn2)
add_subdirectory(svg2pnt)
add_subdirectory(maptheme-previewimage)
if(NOT WIN32)
  # doesn't build on Windows currently
  add_subdirectory(routing-instructions)
  add_subdirectory(mapreproject)
endif()
add_subdirectory(speaker-files)
add_subdirectory(stars)
add_subdirectory(sentineltile)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Tirex backend contains highly Linux-specific code
  add_subdirectory(vectorosm-tilecreator)
endif()

if(PROTOBUF_FOUND AND ZLIB_FOUND)
  # add_subdirectory(osm-addresses)
  # osm-sisyphus does not depend on zlib and protobuf, but osm-addresses
  add_subdirectory(osm-sisyphus)
endif()
