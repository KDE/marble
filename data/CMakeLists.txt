install (FILES
credits_authors.html
credits_data.html
legend.html
legend.css
DESTINATION ${MARBLE_DATA_PATH})

FILE (GLOB LEGEND_FILES *.leg)
install (FILES ${LEGEND_FILES}
DESTINATION ${MARBLE_DATA_PATH})

FILE (GLOB BITMAP_FILES bitmaps/*.png)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_PATH}/bitmaps)

FILE (GLOB BITMAP_FILES bitmaps/*.xpm)
install (FILES ${BITMAP_FILES}
DESTINATION ${MARBLE_DATA_PATH}/bitmaps)

install (FILES 
svg/compass.svg
svg/marble-logo.svg
svg/marble-logo.png
svg/marble-logo-72dpi.png
svg/application-x-marble.svg
svg/application-x-marble.png
svg/application-x-marble-gray.png
DESTINATION ${MARBLE_DATA_PATH}/svg)

FILE (GLOB SVG_FILES flags/*.svg)
install (FILES ${SVG_FILES}
DESTINATION ${MARBLE_DATA_PATH}/flags)

install (FILES
maps/earth/citylights/citylights.jpg
maps/earth/citylights/citylights.dgml
maps/earth/citylights/citylights-preview.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/citylights)

install (FILES
maps/earth/citylights/legend/city.png
maps/earth/citylights/legend/lightterrain.png
maps/earth/citylights/legend/darkterrain.png
maps/earth/citylights/legend/water.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/citylights/legend)

install (FILES
maps/earth/srtm/srtm.jpg
maps/earth/srtm/srtm.dgml
maps/earth/srtm/srtm.dgm2
maps/earth/srtm/srtm-preview.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/srtm)

install (FILES
maps/earth/bluemarble/bluemarble.jpg
maps/earth/bluemarble/bluemarble.dgml
maps/earth/bluemarble/bluemarble-preview.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/bluemarble)

install (FILES
maps/earth/bluemarble/legend/ice.png
maps/earth/bluemarble/legend/mountain.png
maps/earth/bluemarble/legend/vegetation.png
maps/earth/bluemarble/legend/desert.png
maps/earth/bluemarble/legend/water.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/bluemarble/legend)

install (FILES
maps/earth/plain/plain.dgml
maps/earth/plain/plain-preview.png
DESTINATION ${MARBLE_DATA_PATH}/maps/earth/plain)

FILE (GLOB PNT_FILES mwdbii/*.PNT)
install (FILES
  ${PNT_FILES}
DESTINATION ${MARBLE_DATA_PATH}/mwdbii)

FILE (GLOB PLACEMARK_FILES placemarks/*.cache)
install (FILES
  ${PLACEMARK_FILES}
DESTINATION ${MARBLE_DATA_PATH}/placemarks)

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
