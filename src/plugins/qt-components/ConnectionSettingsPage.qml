// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Column {
    id: connectionSettings
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    
    CheckBox {
        id: gpsEnabled
        text: "GPS enabled"
        checked: settings.gpsTracking
        onClicked: {
            settings.gpsTracking = gpsEnabled.checked
        }
    }

    CheckBox {
        id: showPosition
        text: "Show position"
        checked: settings.showPosition
        onClicked: {
            settings.showPosition = showPosition.checked
        }
    }
    
    CheckBox {
        id: showTrack
        text: "Show track"
        checked: settings.showTrack
        onClicked: {
            settings.showTrack = showTrack.checked
        }
    }
    
    CheckBox {
        id: autoCenter
        text: "Auto-center"
        checked: settings.autoCenter
        onClicked: {
            settings.autoCenter = autoCenter.checked
        }
    }
    
    CheckBox {
        id: offlineMode
        text: "Offline-mode enabled"
        checked: settings.workOffline
        onClicked: {
            settings.workOffline = offlineMode.checked
        }
    }

}