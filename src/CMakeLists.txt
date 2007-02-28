project(marble)

option(QTONLY  "Build Qt only")

find_package(Qt4 REQUIRED)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

if (NOT QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (NOT QTONLY)


if (QTONLY)
  add_definitions (${QT_DEFINITIONS} -msse)
  include_directories (${CMAKE_BUILD_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDE_DIR})
else (QTONLY)
  add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -msse -DKDEBUILD)
  include_directories (${CMAKE_BUILD_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
endif (QTONLY)

link_directories (${QT_LIBRARY_DIR})

set(marble_SRCS
           katlascontrol.cpp
           katlasview.cpp
           katlasviewinputhandler.cpp
           main.cpp
           texcolorizer.cpp
           texloader.cpp
           texmapper.cpp
           quaternion.cpp
           katlasglobe.cpp
           katlastoolbox.cpp
           pntmap.cpp
           vectorcomposer.cpp
           vectormap.cpp
           clippainter.cpp
           geopoint.cpp
           placemarkmanager.cpp
           placecontainer.cpp
           placemarkstorage.cpp
           placefolder.cpp
           placemark.cpp
           searchlistview.cpp
           katlasmapscale.cpp
           placemarkmodel.cpp
           placemarkpainter.cpp
           maptheme.cpp
           tilescissor.cpp
           katlastilecreatordialog.cpp
           katlasthemeselectview.cpp
           katlaswindrose.cpp
           katlasviewpopupmenu.cpp
           katlascrosshair.cpp
           xmlhandler.cpp
           placemarkinfodialog.cpp
           katlasflag.cpp
           gridmap.cpp
           tinywebbrowser.cpp
           httpfetchfile.cpp
)

set (marble_UI katlastilecreatordialog.ui
	   katlastoolbox.ui placemarkinfodialog.ui)

if (QTONLY)
  qt4_wrap_ui(marble_SRCS ${marble_UI})
  qt4_automoc(${marble_SRCS})
else (QTONLY)
  kde4_add_ui_files(marble_SRCS ${marble_UI})
  kde4_automoc(${marble_SRCS})
endif (QTONLY)

#kde4_add_library (marble SHARED ${marble_SRCS})
if (QTONLY)
  add_executable (marble ${marble_SRCS})
else (QTONLY)
  kde4_add_executable (marble ${marble_SRCS})
endif (QTONLY)

if (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY})
else (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY})
endif (QTONLY)

#set_target_properties(marble PROPERTIES VERSION 1.0.0 SOVERSION 1)
install (TARGETS marble DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
#install (TARGETS marble DESTINATION ${LIB_INSTALL_DIR} )

install (FILES
../data/bitmaps/city_1_yellow.png
../data/bitmaps/cursor_tl.xpm
../data/bitmaps/cursor_bc.xpm
../data/bitmaps/cursor_tr.xpm
../data/bitmaps/cursor_bl.xpm
../data/bitmaps/cursor_cl.xpm
../data/bitmaps/cursor_br.xpm
../data/bitmaps/cursor_cr.xpm
../data/bitmaps/city_1_red.png
../data/bitmaps/city_2_red.png
../data/bitmaps/city_1_orange.png
../data/bitmaps/cursor_tc.xpm
../data/bitmaps/city_3_red.png
../data/bitmaps/city_2_yellow.png
../data/bitmaps/city_2_orange.png
../data/bitmaps/city_4_red.png
../data/bitmaps/city_3_yellow.png
../data/bitmaps/city_3_orange.png
../data/bitmaps/city_4_yellow.png
../data/bitmaps/city_4_orange.png
../data/bitmaps/city_1_white.png
../data/bitmaps/city_2_white.png
../data/bitmaps/city_3_white.png
../data/bitmaps/city_4_white.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/bitmaps)

install (FILES 
../data/svg/windrose.svg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/svg)

install (FILES
../data/maps/citylights/citylights.jpg
../data/maps/citylights/citylights.dgml
../data/maps/citylights/citylights-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/citylights)

install (FILES
../data/maps/etopo2/etopo.jpg
../data/maps/etopo2/etopo2.dgml
../data/maps/etopo2/etopo-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/etopo2)

install (FILES
../data/maps/bluemarble/bluemarble.jpg
../data/maps/bluemarble/bluemarble.dgml
../data/maps/bluemarble/bluemarble-preview.png
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/maps/bluemarble)

install (FILES
../data/vectorsource/glaciers.svg
../data/vectorsource/islands.svg
../data/vectorsource/lakes.svg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/vectorsource)

install (FILES
../data/mwdbii/PDIFFBORDER.PNT
../data/mwdbii/PUSA48.PNT
../data/mwdbii/PGLACIER.PNT
../data/mwdbii/PISLAND.PNT.orig
../data/mwdbii/RIVER.PNT
../data/mwdbii/PCANPROV.PNT
../data/mwdbii/PUSA48.DIFF.PNT
../data/mwdbii/PCOAST.PNT
../data/mwdbii/PISLAND.PNT
../data/mwdbii/PLAKE.PNT.orig
../data/mwdbii/Pntinfo.txt
../data/mwdbii/PLAKE.PNT
../data/mwdbii/PAUST.PNT
../data/mwdbii/PBORDER.PNT
../data/mwdbii/PMEXICO.PNT
../data/mwdbii/GOODEIHS.BAS
../data/mwdbii/PBORDER.TXT
../data/mwdbii/MWDBPOLY.TXT
../data/mwdbii/contents.txt
../data/mwdbii/GLOBAL.TXT
DESTINATION ${CMAKE_INSTALL_PREFIX}/data/mwdbii)

install ( FILES
../data/seacolors.leg
../data/landcolors.leg
DESTINATION ${CMAKE_INSTALL_PREFIX}/data)


install (FILES  DESTINATION ${INCLUDE_INSTALL_DIR} )
