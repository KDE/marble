// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page to save the current track to disk
 */
Page {
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    FileSaveDialog {
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        filename: ""
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.getTracking().saveTrack( folder + "/" + filename ); pageStack.pop() }
        onCancelled: pageStack.pop()

        Component.onCompleted: {
            var now = new Date()
            filename = Qt.formatDateTime(now, "yyyy-MM-dd_hh.mm.ss") + ".kml"
        }
    }
}
