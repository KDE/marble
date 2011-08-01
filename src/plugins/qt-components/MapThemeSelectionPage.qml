// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

ListView {
    id: mapListView
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    highlightFollowsCurrentItem: true
    model: themes.mapThemes()
    currentIndex: themes.mapThemes().indexOf( settings.mapTheme )
    spacing: 10
    
    MapThemeManager {
      id: themes
    }
    
    delegate:
        Rectangle {
            id: themeItem
            width: mapListView.width
            height: 50
            color: model.modelData.id == settings.mapTheme ? "lightsteelblue" : "white"
            Label {
                id: themeLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                text: model.modelData.name
            }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var x = mouseX + mapListView.contentX
                var y = mouseY + mapListView.contentY
                mapListView.currentIndex = mapListView.indexAt( x, y )
                settings.mapTheme = themes.mapThemes()[mapListView.currentIndex].id
            }
        }

}