// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

ListView {
    id: mapListView
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    //titleText: "Select Map Theme"
    model: themes.mapThemes()
    
    MapThemeManager {
      id: themes
    }
    
    delegate: 
        Item {
            width: mapListView.width
            height: 50
            Label {
                anchors.fill: parent
                text: model.modelData.name;
            }
        }
        
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var x = mouseX + mapListView.contentX
                var y = mouseY + mapListView.contentY
                mapListView.currentIndex = mapListView.indexAt( x, y )
                settings.mapTheme = themes.mapThemes()[mapListView.currentIndex].id
                console.log( "applying theme: ", settings.mapTheme )
            }
        }

}