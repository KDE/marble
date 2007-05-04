
if (QTONLY)
  # ce: don't know why this is needed here - on win32 'O2' is activated by default in release mode
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} -O2)
  endif(NOT WIN32)
  include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${QT_INCLUDE_DIR})
else (QTONLY)
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -O2) 
  endif(NOT WIN32)
  include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${KDE4_INCLUDES})
endif (QTONLY)

if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif(MSVC)

link_directories (${QT_LIBRARY_DIR})

set(marble_SRCS
           Quaternion.cpp
           GeoPoint.cpp
           GeoPolygon.cpp
	   TextureTile.cpp
           TextureMapper.cpp
           GridMap.cpp
           katlascontrol.cpp
           katlasview.cpp
           katlasviewinputhandler.cpp
           #KdeMainWindow.cpp
           texcolorizer.cpp
           TileLoader.cpp
           katlasglobe.cpp
           katlastoolbox.cpp
           vectorcomposer.cpp
           vectormap.cpp
           clippainter.cpp
           placemarkmanager.cpp
           placecontainer.cpp
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
           tinywebbrowser.cpp
           httpfetchfile.cpp
           measuretool.cpp
	   katlasaboutdialog.cpp
)

set(marble_HDRS
           Quaternion.h
           GeoPoint.h
           GeoPolygon.h
           ScreenPolygon.h
           TextureTile.h
           TextureMapper.h
           GridMap.h
           katlasdirs.h
           katlascontrol.h
           katlasview.h
           katlasviewinputhandler.h
           QtMainWindow.h
           #KdeMainWindow.h
           texcolorizer.h
           TileLoader.h
           katlasglobe.h
           katlastoolbox.h
           vectorcomposer.h
           vectormap.h
           clippainter.h
           placemarkmanager.h
           placecontainer.h
           placemark.h
           searchlistview.h
           katlasmapscale.h
           placemarkmodel.h
           placemarkpainter.h
           maptheme.h
           tilescissor.h
           katlastilecreatordialog.h
           katlasthemeselectview.h
           katlaswindrose.h
           katlasviewpopupmenu.h
           katlascrosshair.h
           xmlhandler.h
           placemarkinfodialog.h
           katlasflag.h
           tinywebbrowser.h
           httpfetchfile.h
           measuretool.h
           katlasaboutdialog.h
)

if (QTONLY)
  set(marble_SRCS ${marble_SRCS} qtmain.cpp QtMainWindow.cpp)
  set(marble_HDRS ${marble_HDRS} QtMainWindow.h)
else (QTONLY)
  set(marble_SRCS ${marble_SRCS} kdemain.cpp KdeMainWindow.cpp)
  set(marble_HDRS ${marble_HDRS} KdeMainWindow.h)
endif (QTONLY)


# cmake does not support this for mingw - see bugtracker
if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.rc)
endif(MSVC)

set (marble_UI katlastilecreatordialog.ui
	   katlastoolbox.ui placemarkinfodialog.ui katlasaboutdialog.ui)

qt4_add_resources(marble_SRCS marble.qrc)

if (QTONLY)
  qt4_wrap_ui(marble_SRCS ${marble_UI})
  qt4_automoc(${marble_SRCS})
else (QTONLY)
  kde4_add_ui_files(marble_SRCS ${marble_UI})
  kde4_automoc(${marble_SRCS})
endif (QTONLY)

#kde4_add_library (marble SHARED ${marble_SRCS})
if (QTONLY)
  add_executable (marble WIN32 ${marble_SRCS} ${marble_HDRS})
else (QTONLY)
  kde4_add_executable (marble ${marble_SRCS} ${marble_HDRS})
endif (QTONLY)

if (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY})
else (QTONLY)
	target_link_libraries (marble ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${QT_QTMAIN_LIBRARY})
endif (QTONLY)
if(WIN32)
  target_link_libraries(marble ws2_32 imm32 winmm)
endif(WIN32)

## this works for win32 only because FindQt4 does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

if(WIN32)
    install (TARGETS marble DESTINATION "")
else(WIN32)
    #set_target_properties(marble PROPERTIES VERSION 1.0.0 SOVERSION 1)
    if( NOT QTONLY)
      install (TARGETS marble DESTINATION ${BIN_INSTALL_DIR} )
    else( NOT QTONLY)
      install (TARGETS marble DESTINATION bin )
    endif(NOT QTONLY)
    #install (TARGETS marble DESTINATION ${LIB_INSTALL_DIR} )

    if (NOT QTONLY)
      install( FILES marble.desktop  DESTINATION ${XDG_APPS_DIR})
    else (NOT QTONLY)
      install( FILES marble.desktop  DESTINATION share/applications)
    endif (NOT QTONLY)
endif(WIN32)

