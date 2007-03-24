macx {
TEMPLATE = app

QT += svg network

SOURCES +=  katlascontrol.cpp katlasview.cpp katlasviewinputhandler.cpp main.cpp texcolorizer.cpp \
			texloader.cpp texmapper.cpp quaternion.cpp katlasglobe.cpp katlastoolbox.cpp pntmap.cpp \
			vectorcomposer.cpp vectormap.cpp clippainter.cpp geopoint.cpp placemarkmanager.cpp \
			placecontainer.cpp placemarkstorage.cpp placefolder.cpp placemark.cpp searchlistview.cpp \
			katlasmapscale.cpp placemarkmodel.cpp placemarkpainter.cpp maptheme.cpp tilescissor.cpp \
			katlastilecreatordialog.cpp katlasthemeselectview.cpp katlaswindrose.cpp katlasviewpopupmenu.cpp \
			katlascrosshair.cpp xmlhandler.cpp placemarkinfodialog.cpp katlasflag.cpp gridmap.cpp \
			tinywebbrowser.cpp httpfetchfile.cpp measuretool.cpp

HEADERS +=  clippainter.h fastmath.h geopoint.h gridmap.h httpfetchfile.h katlascontrol.h katlascrosshair.h \
			katlasdirs.h katlasflag.h katlasglobe.h katlasmapscale.h katlasthemeselectview.h katlastilecreatordialog.h \
			katlastoolbox.h katlasview.h katlasviewinputhandler.h katlasviewpopupmenu.h katlaswindrose.h maptheme.h \
			measuretool.h placecontainer.h placefolder.h placelist.h placemark.h placemarkinfodialog.h placemarkmanager.h \
			placemarkmodel.h placemarkpainter.h placemarkstorage.h pntmap.h polygon.h quaternion.h searchlistview.h \
			texcolorizer.h texloader.h texmapper.h tilescissor.h tinywebbrowser.h vectorcomposer.h \
			vectormap.h xmlhandler.h

RESOURCES = marble.qrc
FORMS = katlastilecreatordialog.ui katlastoolbox.ui placemarkinfodialog.ui
}
