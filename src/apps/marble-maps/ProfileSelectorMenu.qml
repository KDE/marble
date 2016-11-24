//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.marble 0.20

Item {
    id: root

    property string selectedProfile: carProfileButton.profile
    property string profileIcon: "qrc:///material/directions-car.svg"
    height: rowLayout.height
    width: rowLayout.width

    Settings {
        id: settings
        Component.onDestruction: {
            settings.setValue("Routing", "profile", root.selectedProfile)
        }
    }

    RowLayout {
        id: rowLayout
        ExclusiveGroup {
            id: profileGroup
            onCurrentChanged: {
                profileIcon = current.imageSource
                selectedProfile = current.profile;
            }
        }

        RouteProfileRadioButton {
            id: carProfileButton
            checked: settings.value("Routing", "profile") === "" || settings.value("Routing", "profile", profile) === profile
            exclusiveGroup: profileGroup
            property string profile: qsTr("Car (fastest)")
            imageSource: "qrc:///material/directions-car.svg"
        }
        RouteProfileRadioButton {
            checked: settings.value("Routing", "profile") === profile
            exclusiveGroup: profileGroup
            property string profile: qsTr("Bicycle")
            imageSource: "qrc:///material/directions-bike.svg"
        }
        RouteProfileRadioButton {
            checked: settings.value("Routing", "profile") === profile
            exclusiveGroup: profileGroup
            property string profile: qsTr("Pedestrian")
            imageSource: "qrc:///material/directions-walk.svg"
        }
    }
}
