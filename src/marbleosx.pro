macx {
TEMPLATE = app

QT += svg network

SOURCES += Quaternion.cpp GeoPoint.cpp GeoPolygon.cpp TextureTile.cpp TextureMapper.cpp GridMap.cpp \
           katlascontrol.cpp MarbleWidget.cpp marblecontrol.cpp katlasviewinputhandler.cpp \
           texcolorizer.cpp TileLoader.cpp katlasglobe.cpp MarbleControlBox.cpp vectorcomposer.cpp \
           vectormap.cpp clippainter.cpp placemarkmanager.cpp placecontainer.cpp placemark.cpp \
           searchlistview.cpp katlasmapscale.cpp placemarkmodel.cpp placemarkpainter.cpp maptheme.cpp \
           tilescissor.cpp katlastilecreatordialog.cpp katlasthemeselectview.cpp katlaswindrose.cpp \
           katlasviewpopupmenu.cpp katlascrosshair.cpp xmlhandler.cpp placemarkinfodialog.cpp \
           katlasflag.cpp tinywebbrowser.cpp HttpDownloadManager.cpp httpfetchfile.cpp \
           measuretool.cpp katlasaboutdialog.cpp QtMainWindow.cpp main.cpp

HEADERS += Quaternion.h GeoPoint.h GeoPolygon.h ScreenPolygon.h TextureTile.h TextureMapper.h \
           GridMap.h katlasdirs.h katlascontrol.h MarbleWidget.h marblecontrol.h katlasviewinputhandler.h \
           QtMainWindow.h texcolorizer.h TileLoader.h katlasglobe.h MarbleControlBox.h vectorcomposer.h \
           vectormap.h clippainter.h placemarkmanager.h placecontainer.h placemark.h searchlistview.h \
           katlasmapscale.h placemarkmodel.h placemarkpainter.h maptheme.h tilescissor.h \
           katlastilecreatordialog.h katlasthemeselectview.h katlaswindrose.h katlasviewpopupmenu.h \
           katlascrosshair.h xmlhandler.h placemarkinfodialog.h katlasflag.h tinywebbrowser.h \
           HttpDownloadManager.h httpfetchfile.h measuretool.h katlasaboutdialog.h

RESOURCES = marble.qrc
FORMS = katlastilecreatordialog.ui MarbleControlBox.ui placemarkinfodialog.ui \
        katlasaboutdialog.ui marblecontrol.ui
}
