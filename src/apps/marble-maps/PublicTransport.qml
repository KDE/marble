//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3


Item {
    id: root

    implicitHeight: Math.min(0.75 * Screen.height, listView.contentHeight)
    property var marbleMaps

    ListModel {
        id: transportModel
        ListElement {
            name: qsTr("Train")
            icon: "material/directions-railway.svg"
            key: "train"
        }
        ListElement {
            name: qsTr("Subway")
            icon: "material/directions-subway.svg"
            key: "subway"
        }
        ListElement {
            name: qsTr("Tram")
            icon: "material/directions-tram.svg"
            key: "tram"
        }
        ListElement {
            name: qsTr("Bus")
            icon: "material/directions-bus.svg"
            key: "bus"
        }
        ListElement {
            name: qsTr("Trolley Bus")
            icon: "material/directions-bus.svg"
            key: "trolley-bus"
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        contentWidth: width

        model: transportModel
        clip: true
        spacing: Screen.pixelDensity * 2

        delegate: Row {
            CheckBox {
                id: control
                text: name

                checked: root.marbleMaps.isRelationTypeVisible(key)

                contentItem: Row {
                    Item {
                        height: parent.height
                        width: control.indicator.width + control.spacing
                    }

                    Image {
                        source: icon
                        height: parent.height
                        sourceSize.height: height
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        height: parent.height
                        text: control.text
                        font: control.font
                        opacity: enabled ? 1.0 : 0.3
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                onCheckedChanged: {
                    root.marbleMaps.setRelationTypeVisible(key, checked)
                }
            }
        }
        ScrollBar.vertical: ScrollBar {}
    }
}
