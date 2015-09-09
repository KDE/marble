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

Item {
    id: root

    property string selectedProfile: qsTr("Car (fastest)")
    property string profileIcon: "qrc:///car.png"
    height: rowLayout.height
    width: rowLayout.width

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
            checked: true
            exclusiveGroup: profileGroup
            property string profile: qsTr("Car (fastest)")
            imageSource: "qrc:///car.png"
        }
        RouteProfileRadioButton {
            exclusiveGroup: profileGroup
            property string profile: qsTr("Bicycle")
            imageSource: "qrc:///bike.png"
        }
        RouteProfileRadioButton {
            exclusiveGroup: profileGroup
            property string profile: qsTr("Pedestrian")
            imageSource: "qrc:///walk.png"
        }
    }
}
