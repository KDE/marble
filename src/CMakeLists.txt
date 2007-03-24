
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
           measuretool.cpp
)

set (marble_UI katlastilecreatordialog.ui
	   katlastoolbox.ui placemarkinfodialog.ui)

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

if (NOT QTONLY)
install( FILES marble.desktop  DESTINATION ${XDG_APPS_DIR})
else (NOT QTONLY)
install( FILES marble.desktop  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif (NOT QTONLY)

