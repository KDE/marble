// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

Page {
    id: placemarkActivityPage
    anchors.fill: parent

    property bool horizontal: width / height > 1.20

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    Rectangle {
        id: placemarkView

        anchors.bottom: placemarkActivityPage.bottom
        anchors.left: placemarkActivityPage.left
        width:  placemarkActivityPage.horizontal ? placemarkActivityPage.width / 2 : placemarkActivityPage.width
        height: placemarkActivityPage.horizontal ? placemarkActivityPage.height : 255

        color: "#e7e7e7"
        border.width: 2
        border.color: "darkgray"

        radius: 10
        z: 10
        opacity: 0.9
        property string searchTerm: ""

        PlacemarkEditor {
            id: placemarkEditor
            anchors.fill: parent
            anchors.margins: 10
        }
    }

    Item {
        id: mapContainer
        clip: true

        anchors.left: placemarkActivityPage.horizontal ? placemarkView.right : placemarkActivityPage.left
        anchors.bottom: placemarkActivityPage.horizontal ? placemarkActivityPage.bottom : placemarkView.top
        anchors.right: placemarkActivityPage.right
        anchors.top: placemarkActivityPage.top

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
