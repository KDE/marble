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

INSTALL (DIRECTORY svg/osmcarto/svg
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/svg/osmcarto
FILES_MATCHING PATTERN "*.svg"
PATTERN "svgorig" EXCLUDE)

INSTALL (DIRECTORY svg/material svg/thenounproject
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/svg
FILES_MATCHING PATTERN "*.svg")

FILE (GLOB BITMAP_EDITARROW_FILES bitmaps/editarrows/*.png)
install (FILES ${BITMAP_EDITARROW_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps/editarrows)

install (DIRECTORY bitmaps/osmcarto
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps
PATTERN "LICENSE.txt" EXCLUDE)

FILE (GLOB BITMAP_STAR_FILES bitmaps/stars/*.png)
install (FILES ${BITMAP_STAR_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/bitmaps/stars)

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
svg/marsmap.svg
svg/coordinate.svg
svg/wikipedia.svg
svg/wikipedia_shadow.svg
svg/sunshine.png
svg/sun.png
svg/moon.png
svg/glow.png
svg/track_turtle.svg
svg/bookmark.svg
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

install (DIRECTORY maps/earth/srtm DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth)

install (FILES
maps/earth/srtm2/srtm2.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm2)

install (FILES
maps/earth/srtm2/0/000000/000000_000000.png
maps/earth/srtm2/0/000000/000000_000001.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/srtm2/0/000000)

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

install (DIRECTORY maps/earth/behaim1492
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth
)

install (DIRECTORY maps/earth/openstreetmap
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth
)

install (FILES
maps/earth/humanitarian/0/0/0.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/humanitarian/0/0)

install (FILES
maps/earth/hillshading/0/0/0.png
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/hillshading/0/0)

OPTION(EXPERIMENTAL_SENTINEL2_MAP "Install an experimental satellite map theme" ON)
if(EXPERIMENTAL_SENTINEL2_MAP)
    install (DIRECTORY maps/earth/sentinel2 DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth)
endif()

install (DIRECTORY maps/earth/vectorosm DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth)
OPTION(MARBLE_DEV_VECTOR_TILING "Install the development version of OSM vector tiles" OFF)
if(MARBLE_DEV_VECTOR_TILING)
    install (DIRECTORY maps/earth/vectorosm-dev DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth)
endif()

OPTION(MARBLE_EMPTY_MAPTHEME "Install an empty map theme for developmental purposes")
if(MARBLE_EMPTY_MAPTHEME)
    install (DIRECTORY maps/earth/empty DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth)
endif()

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
maps/earth/political/political-preview.png
maps/earth/political/political.dgml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/earth/political)

install (FILES
maps/moon/clementine/clementine.jpg
maps/moon/clementine/clementine-preview.png
maps/moon/clementine/legend.html 
maps/moon/clementine/clementine.dgml
maps/moon/clementine/tour.kml
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/maps/moon/clementine)

FILE (GLOB_RECURSE DGML_FILES maps/*.dgml)
MARBLE_ADD_PROJECT_RESOURCES(${DGML_FILES})

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

FILE (GLOB PN2_FILES naturalearth/*.pn2)
install (FILES ${PN2_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/naturalearth)

FILE (GLOB STAR_FILES stars/*.dat)
install (FILES ${STAR_FILES} stars/deepsky.png stars/names.csv
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
  install (FILES icons/128-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128/apps/ RENAME marble.png)
  install (FILES icons/64-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/64x64/apps/ RENAME marble.png)
  install (FILES icons/48-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48/apps/ RENAME marble.png)
  install (FILES icons/32-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/32x32/apps/ RENAME marble.png)
  install (FILES icons/22-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/22x22/apps/ RENAME marble.png)
  install (FILES icons/16-apps-marble.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/16x16/apps/ RENAME marble.png)
endif(NOT APPLE AND NOT WIN32 )

FILE (GLOB AUDIO_FILES audio/*.ogg)
install (FILES
  ${AUDIO_FILES}
DESTINATION ${MARBLE_DATA_INSTALL_PATH}/audio)

add_subdirectory(lang)

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET) # for FindSharedMimeInfo
if(NOT ECM_FOUND)
    return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs) # for KDE_INSTALL_MIMEDIR
# shared-mime-info 0.40 is mandatory for generic-icon
set( SHARED_MIME_INFO_MINIMUM_VERSION "0.40" )
find_package( SharedMimeInfo QUIET )
if(SharedMimeInfo_FOUND)
    install(FILES mimetypes/geo.xml  DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes( ${KDE_INSTALL_MIMEDIR} )
endif()
