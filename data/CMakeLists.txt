install (FILES
credits_authors.html
credits_data.html
legend.html
legend.css
DESTINATION ${MARBLE_DATA_INSTALL_PATH})

FILE (GLOB LEGEND_FILES *.leg)
install (FILES ${LEGEND_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH})

FILE (GLOB BITMAP_FILES bitmaps/*.png)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps)

FILE (GLOB BITMAP_FILES bitmaps/*.jpg)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps)

FILE (GLOB BITMAP_FILES bitmaps/*.xpm)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps)

install (FILES 
svg/compass.svg
svg/marble-logo.svg
svg/marble-logo.png
svg/marble-logo-72dpi.png
svg/marble-logo-inverted-72dpi.png
svg/application-x-marble.svg
svg/application-x-marble.png
svg/application-x-marble-gray.png
svg/worldmap.svg
svg/lunarmap.svg
svg/coordinate.svg
svg/wikipedia.svg
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/svg)

FILE (GLOB SVG_FILES flags/*.svg)
install (FILES ${SVG_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/flags)

install (FILES
maps/earth/citylights/citylights.jpg
maps/earth/citylights/citylights.dgml
maps/earth/citylights/citylights-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/citylights)

install (FILES
maps/earth/citylights/legend/city.png
maps/earth/citylights/legend/lightterrain.png
maps/earth/citylights/legend/darkterrain.png
maps/earth/citylights/legend/water.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/citylights/legend)

install (FILES
maps/earth/srtm/srtm.dgml
maps/earth/srtm/srtm-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm)

if(NOT TILES_AT_COMPILETIME)
install (FILES
maps/earth/srtm/srtm.jpg
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm)
endif(NOT TILES_AT_COMPILETIME)

install (FILES
maps/earth/bluemarble/bluemarble.jpg
maps/earth/bluemarble/bluemarble.dgml
maps/earth/bluemarble/bluemarble-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/bluemarble)

install (FILES
maps/earth/bluemarble/legend/ice.png
maps/earth/bluemarble/legend/mountain.png
maps/earth/bluemarble/legend/vegetation.png
maps/earth/bluemarble/legend/desert.png
maps/earth/bluemarble/legend/water.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/bluemarble/legend)

install (FILES
maps/earth/plain/plain.dgml
maps/earth/plain/plain-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/plain)

install (FILES
maps/earth/openstreetmap/openstreetmap.dgml
maps/earth/openstreetmap/openstreetmap-preview.png
maps/earth/openstreetmap/legend.html
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap)

install (FILES
maps/earth/openstreetmap/legend/airportapron.png
maps/earth/openstreetmap/legend/motorway.png
maps/earth/openstreetmap/legend/track.png
maps/earth/openstreetmap/legend/airportrunway.png
maps/earth/openstreetmap/legend/primaryroad.png
maps/earth/openstreetmap/legend/tram.png
maps/earth/openstreetmap/legend/boundary.png
maps/earth/openstreetmap/legend/railway.png
maps/earth/openstreetmap/legend/trunkroad.png
maps/earth/openstreetmap/legend/bridge.png
maps/earth/openstreetmap/legend/railwaystation.png
maps/earth/openstreetmap/legend/tunnel.png
maps/earth/openstreetmap/legend/bridleway.png
maps/earth/openstreetmap/legend/secondaryroad.png
maps/earth/openstreetmap/legend/unclassifiedroad.png
maps/earth/openstreetmap/legend/byway.png
maps/earth/openstreetmap/legend/significantbuilding.png
maps/earth/openstreetmap/legend/unsurfacedroad.png
maps/earth/openstreetmap/legend/cycleway.png
maps/earth/openstreetmap/legend/subway.png
maps/earth/openstreetmap/legend/footway.png
maps/earth/openstreetmap/legend/summitpeak.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap/legend)

install (FILES
maps/earth/openstreetmap/0/0/0.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap/0/0)

install (FILES
maps/earth/temp-july/temp-july.jpg
maps/earth/temp-july/temp-july-preview.png
maps/earth/temp-july/temp-july.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/temp-july)

install (FILES
maps/earth/temp-dec/temp-dec.jpg
maps/earth/temp-dec/temp-dec-preview.png
maps/earth/temp-dec/temp-dec.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/temp-dec)

install (FILES
maps/earth/precip-july/precip-july.jpg
maps/earth/precip-july/precip-july-preview.png
maps/earth/precip-july/precip-july.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/precip-july)

install (FILES
maps/earth/precip-dec/precip-dec.jpg
maps/earth/precip-dec/precip-dec-preview.png
maps/earth/precip-dec/precip-dec.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/precip-dec)

install (FILES
maps/earth/schagen1689/legend.html
maps/earth/schagen1689/schagen1689.jpg
maps/earth/schagen1689/schagen1689.dgml
maps/earth/schagen1689/schagen1689-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/schagen1689)

install (FILES
maps/earth/schagen1689/legend/mountain.png
maps/earth/schagen1689/legend/town.png
maps/earth/schagen1689/legend/river.png
maps/earth/schagen1689/legend/lake.png
maps/earth/schagen1689/legend/schagen_title.png
maps/earth/schagen1689/legend/schagen_orig.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/schagen1689/legend)

install (FILES
maps/moon/clementine/clementine.jpg
maps/moon/clementine/clementine-preview.png
maps/moon/clementine/legend.html 
maps/moon/clementine/clementine.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/moon/clementine)

FILE (GLOB PNT_FILES mwdbii/*.PNT)
install (FILES
  ${PNT_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/mwdbii)

FILE (GLOB STAR_FILES stars/*.dat)
install (FILES
  ${STAR_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/stars)

FILE (GLOB PLACEMARK_FILES placemarks/*.cache)
install (FILES
  ${PLACEMARK_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/placemarks)

if(NOT APPLE AND NOT WIN32)
  if(QTONLY)
    set(ICON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icons)
  endif(QTONLY)
    install (FILES icons/hi128-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128/apps/ RENAME marble.png)
    install (FILES icons/hi64-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/64x64/apps/ RENAME marble.png)
    install (FILES icons/hi48-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48/apps/ RENAME marble.png)
    install (FILES icons/hi32-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/32x32/apps/ RENAME marble.png)
    install (FILES icons/hi22-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/22x22/apps/ RENAME marble.png)
    install (FILES icons/hi16-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/16x16/apps/ RENAME marble.png)
endif(NOT APPLE AND NOT WIN32 )
