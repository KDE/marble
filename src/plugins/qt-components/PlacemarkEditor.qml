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

Item {
    id: root
    z: 10

    property Placemark placemark

    function editPlacemark(placemark)
    {
        root.placemark = placemark
        placemarkLabel.text = placemark.name
        distanceLabel.text = (marbleWidget.tracking.lastKnownPosition.distance(placemark.coordinate.longitude, placemark.coordinate.latitude) / 1000).toFixed(1) + " km"
        panoramio.update()
    }

    function startRouting()
    {
        settings.gpsTracking = true
        marbleWidget.routing.clearRoute()
        marbleWidget.routing.setVia( 0, marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
        marbleWidget.routing.setVia( 1, root.placemark.coordinate.longitude, root.placemark.coordinate.latitude )
        openActivity( "Routing" )
    }

    Item {
        id: wrapper
        anchors.fill: parent
        width: parent.width
        anchors.margins: 5

        Column {
            id: content
            width: parent.width
            spacing: 10

            Item {
                id: headerItem
                width: parent.width
                height: Math.max(placemarkLabel.height, bookmarkButton.height)

                Label {
                    id: placemarkLabel
                    anchors.left: parent.left
                    anchors.right: bookmarkButton.left
                    anchors.verticalCenter: parent.verticalCenter
                }

                ToolButton {
                    id: bookmarkButton
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right

                    property bool isBookmark: root.placemark != undefined && marbleWidget.bookmarks.isBookmark(root.placemark.coordinate.longitude,root.placemark.coordinate.latitude)
                    iconSource: isBookmark ? "qrc:/icons/bookmark.png" : "qrc:/icons/bookmark-disabled.png"

                    width: height
                    flat: true

                    onClicked: {
                        if ( isBookmark ) {
                            marbleWidget.bookmarks.removeBookmark(root.placemark.coordinate.longitude, root.placemark.coordinate.latitude)
                        } else {
                            marbleWidget.bookmarks.addBookmark(root.placemark.coordinate.longitude, root.placemark.coordinate.latitude, root.placemark.name, "Default")
                        }
                        isBookmark = marbleWidget.bookmarks.isBookmark(root.placemark.coordinate.longitude, root.placemark.coordinate.latitude)
                    }
                }
            }

            Rectangle {
                id: horizontalLine
                anchors.left: parent.left
                anchors.right: parent.right
                color: "darkgray"
                height: 1
            }

            Row {
                id: routingItem
                width: parent.width
                spacing: 10

                Button {
                    id: carButton
                    width: 120
                    iconSource: "qrc:/icons/routing-motorcar.svg"
                    onClicked: {
                        marbleWidget.routing.routingProfile = "Motorcar"
                        root.startRouting()
                    }
                }

                Button {
                    id: bikeButton
                    width: 80
                    iconSource: "qrc:/icons/routing-bike.svg"
                    onClicked: {
                        marbleWidget.routing.routingProfile = "Bicycle"
                        root.startRouting()
                    }
                }

                Button {
                    id: pedButton
                    width: 60
                    iconSource: "qrc:/icons/routing-pedestrian.svg"
                    onClicked: {
                        marbleWidget.routing.routingProfile = "Pedestrian"
                        root.startRouting()
                    }
                }

                Label {
                    id: distanceLabel
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    text: "0 km"
                    color: "darkgray"
                    font.pixelSize: 16
                }
            }

            Item {
                id: panoramioItem
                anchors.left: parent.left
                anchors.right: parent.right
                height: 64
                visible: listmodel.count > 0

                ListView {
                    anchors.left: parent.left
                    anchors.right: legalText.left
                    anchors.leftMargin: 5
                    anchors.rightMargin: 10
                    id: panoramioView
                    height: parent.height
                    spacing: 5
                    orientation: ListView.Horizontal
                    clip: true

                    model: listmodel
                    delegate: Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        color: panoramioView.currentIndex === index ? "lightblue" : "darkgray"
                        smooth: true
                        width: 64
                        height: 64
                        scale: panoramioImage.status === Image.Ready ? 1.0 : 0.0
                        property bool selected: false

                        Image {
                            id: panoramioImage
                            anchors.centerIn: parent
                            width: 60
                            height: 60
                            fillMode: Image.PreserveAspectCrop
                            source: url
                            smooth: true
                            clip: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: parent.selected = !parent.selected
                        }

                        Behavior on scale {
                            NumberAnimation {
                                easing.type: Easing.InOutQuad
                                duration: 500
                            }
                        }

                        onSelectedChanged: {
                            if (selected) {
                                legalText.customText = title + "<br /><a href=\"" + link + "\">Photo by " + owner + "</a>."
                                panoramioView.currentIndex = index
                            } else {
                                panoramioView.currentIndex = -1
                            }
                        }
                    }
                }

                Text {
                    id: legalText
                    anchors.right: parent.right
                    width: 140
                    text: panoramioView.currentIndex >= 0 ? customText : generalText
                    wrapMode: Text.WordWrap
                    color: "darkgray"
                    onLinkActivated: Qt.openUrlExternally(link)

                    property string generalText: "Photos provided by Panoramio. Photos are under the copyright of their owners."
                    property string customText
                }
            }
        }
    }

    Item {
        id: panoramio
        ListModel { id: listmodel }

        property bool active: root.placemark != undefined
        property double longitude: root.placemark != undefined ? root.placemark.coordinate.longitude : 0.0
        property double latitude: root.placemark != undefined ? root.placemark.coordinate.latitude : 0.0
        property double offset: 0.01

        function update()
        {
            if (settings.workOffline) {
                return
            }

            var R = 6378000.0
            var d = 500.0
            var brng = 215 * 3.1415926 / 180.0
            var deg2rad = 3.1415926 / 180.0
            var lat = latitude * deg2rad
            var lon = longitude * deg2rad
            var lat1 = Math.asin( Math.sin(lat)*Math.cos(d/R) + Math.cos(lat)*Math.sin(d/R)*Math.cos(brng) );
            var lon1 = lon + Math.atan2(Math.sin(brng)*Math.sin(d/R)*Math.cos(lat), Math.cos(d/R)-Math.sin(lat)*Math.sin(lat1));
            brng = 135 * 3.1415926 / 180.0
            var lat2 = Math.asin( Math.sin(lat)*Math.cos(d/R) + Math.cos(lat)*Math.sin(d/R)*Math.cos(brng) );
            var lon2 = lon + Math.atan2(Math.sin(brng)*Math.sin(d/R)*Math.cos(lat), Math.cos(d/R)-Math.sin(lat)*Math.sin(lat2));
            var url = "http://www.panoramio.com/map/get_panoramas.php?set=public&from=0&to=20&minx=" + lon1/deg2rad + "&miny=" + lat1/deg2rad + "&maxx=" + lon2/deg2rad + "&maxy=" + lat2/deg2rad + "&size=square&mapfilter=true"

//            console.log("Retrieving " + url)
            var xhr = new XMLHttpRequest;
            xhr.open("GET", url);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    listmodel.clear()
                    var a = JSON.parse(xhr.responseText);
                    for (var b in a.photos) {
                        var o = a.photos[b];
                        listmodel.append({owner: o.owner_name, url: o.photo_file_url, link: o.photo_url, title: o.photo_title});
                    }
                }
            }
            xhr.send();
        }
    }

    Connections { target: marbleWidget; onPlacemarkSelected: root.editPlacemark(placemark) }
}
