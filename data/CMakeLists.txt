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

FILE (GLOB BITMAP_POI_FILES bitmaps/poi/*.png)
install (FILES ${BITMAP_POI_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps/poi)

FILE (GLOB BITMAP_FILES bitmaps/*.jpg)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps)

install (FILES 
svg/marble-logo.svg
svg/marble-logo.png
svg/marble-logo-32dpi.png
svg/marble-logo-72dpi.png
svg/marble-logo-inverted-32dpi.png
svg/marble-logo-inverted-72dpi.png
svg/application-x-marble.svg
svg/application-x-marble.png
svg/application-x-marble-gray.png
svg/worldmap.svg
svg/lunarmap.svg
svg/coordinate.svg
svg/wikipedia.svg
svg/wikipedia_shadow.svg
svg/sunshine.png
svg/sun.png
svg/track_turtle.svg
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
maps/earth/clouds/0/000000/000000_000000.jpg
maps/earth/clouds/0/000000/000000_000001.jpg
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/clouds/0/000000)

install (FILES
maps/earth/srtm/srtm.dgml
maps/earth/srtm/srtm-preview.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm)

install (FILES
maps/earth/srtm2/srtm2.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm2)

install (FILES
maps/earth/srtm2/0/000000/000000_000000.png
maps/earth/srtm2/0/000000/000000_000001.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm2/0/000000)

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

FILE(GLOB OSM_LEGEND_ICONS maps/earth/openstreetmap/legend/*.png)
install (FILES ${OSM_LEGEND_ICONS} DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap/legend)
FILE(GLOB OSM_LEGEND_ICONS maps/earth/openstreetmap/legend/mapicons/*.png)
install (FILES ${OSM_LEGEND_ICONS} DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap/legend/mapicons)

install (FILES
maps/earth/openstreetmap/0/0/0.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/openstreetmap/0/0)

install (FILES
maps/earth/hillshading/0/0/0.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/hillshading/0/0)

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

install (FILES
weather/bbc-stations.xml
weather/weather-clear-night.png
weather/weather-clear.png
weather/weather-clouds-night.png
weather/weather-clouds.png
weather/weather-few-clouds-night.png
weather/weather-few-clouds.png
weather/weather-hail.png
weather/weather-many-clouds.png
weather/weather-mist.png
weather/weather-none-available.png
weather/weather-showers-day.png
weather/weather-showers-night.png
weather/weather-showers-scattered-day.png
weather/weather-showers-scattered-night.png
weather/weather-showers-scattered.png
weather/weather-showers.png
weather/weather-snow-rain.png
weather/weather-snow-scattered-day.png
weather/weather-snow-scattered-night.png
weather/weather-snow-scattered.png
weather/weather-snow.png
weather/weather-storm-day.png
weather/weather-storm-night.png
weather/weather-storm.png
weather/wind-arrows.svgz
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/weather)

install (FILES
licenses/lgpl2.txt
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/licenses)

install (FILES
mwdbii/DATELINE.PNT
mwdbii/PCOAST.PNT
mwdbii/PDIFFBORDER.PNT
mwdbii/PGLACIER.PNT
mwdbii/PISLAND.PNT
mwdbii/PLAKEISLAND.PNT
mwdbii/PLAKE.PNT
mwdbii/PUSA48.DIFF.PNT
mwdbii/RIVER.PNT
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/mwdbii)

FILE (GLOB STAR_FILES stars/*.dat)
install (FILES ${STAR_FILES} stars/deepsky.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/stars)

install (FILES
placemarks/elevplacemarks.cache
placemarks/otherplacemarks.cache
placemarks/baseplacemarks.cache
placemarks/moonlandingsites.cache
placemarks/boundaryplacemarks.cache
placemarks/moonterrain.cache
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/placemarks)

if(MOBILE)
    install (FILES placemarks/cityplacemarks_large_population.cache DESTINATION ${MARBLE_DATA_INSTALL_PATH}/placemarks RENAME placemarks/cityplacemarks.cache)
else(MOBILE)
    install (FILES placemarks/cityplacemarks.cache DESTINATION ${MARBLE_DATA_INSTALL_PATH}/placemarks)
endif(MOBILE)

if(NOT APPLE AND NOT WIN32)
  install (FILES icons/hi128-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128/apps/ RENAME marble.png)
  install (FILES icons/hi64-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/64x64/apps/ RENAME marble.png)
  install (FILES icons/hi48-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48/apps/ RENAME marble.png)
  install (FILES icons/hi32-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/32x32/apps/ RENAME marble.png)
  install (FILES icons/hi22-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/22x22/apps/ RENAME marble.png)
  install (FILES icons/hi16-app-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/16x16/apps/ RENAME marble.png)
endif(NOT APPLE AND NOT WIN32 )

FILE (GLOB AUDIO_FILES audio/*.ogg)
install (FILES
  ${AUDIO_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/audio)

FILE (GLOB_RECURSE DGML_FILES maps/*.dgml)
MARBLE_ADD_PROJECT_RESOURCES(${DGML_FILES})

# shared-mime-info 0.40 is mandatory for generic-icon
set( SHARED_MIME_INFO_MINIMUM_VERSION "0.40" )
find_package( SharedMimeInfo QUIET )
IF( SHAREDMIMEINFO_FOUND )
  install( FILES mimetypes/geo.xml  DESTINATION ${XDG_MIME_INSTALL_DIR} )
  update_xdg_mimetypes( ${XDG_MIME_INSTALL_DIR} )
ENDIF()
