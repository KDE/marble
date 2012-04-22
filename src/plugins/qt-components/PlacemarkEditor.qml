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

                    width: 32
                    height: 32
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
                height: carButton.height
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
                    text: "0 km"
                }
            }

            Item {
                id: panoramioItem
                anchors.left: parent.left
                anchors.right: parent.right
                height: 64

                Item {
                    id: weatherItem
                    anchors.left: parent.left
                    width: 234
                    Repeater {
                        model: weatherModel

                        Item {
                            Image {
                                id: weatherConditionImage
                                visible: weatherConditionImage.source !== ""

                                width: 64
                                height: width
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                                property string condition: weatherCondition
                                property string icon: ""
                                onConditionChanged: icon = parseCondition(weatherCondition, clouds)
                                source: icon === "" ? "" : Marble.resolvePath("weather/weather-" + icon + ".png")
                            }

                            Flickable {
                                anchors.left: weatherConditionImage.right
                                anchors.margins: 5
                                width: 160
                                height: 64
                                contentWidth: 160
                                contentHeight: temperatureLabel.height
                                clip: true

                                Label {
                                    id: temperatureLabel
                                    width: 160
                                    property string cloudsText: clouds === "n/a" ? "" : clouds
                                    property string weatherConditionText: weatherCondition === "n/a" ? "" : (cloudsText === "" ? "" : ", ") + weatherCondition
                                    text: "<font size=\"-2\">" + temperature + " °C<br />" + cloudsText + weatherConditionText + "</font>"
                                }
                            }

                            function parseCondition(condition, clouds) {
                                if (condition === "light rain") return "showers-scattered"
                                if (condition.match("rain")) return "showers"
                                if (condition.match("drizzle")) return "showers"
                                if (condition === "light snow") return "snow-scattered"
                                if (condition.match("hail")) return "hail"
                                if (condition.match("snow")) return "snow"
                                if (condition.match("ice")) return "freezing-rain"
                                if (condition === "mist") return "mist"
                                if (condition === "fog") return "mist"
                                if (condition === "smoke") return "mist"
                                if (condition === "volcanic ash") return "mist"
                                if (condition === "sand") return "mist"
                                if (condition === "haze") return "mist"
                                if (condition === "spray") return "mist"
                                if (condition === "widespread dust") return "mist"
                                if (condition === "squall") return "storm"
                                if (condition === "sandstorm") return "storm"
                                if (condition === "duststorm") return "storm"
                                if (condition === "well developed dust/sand whirls") return "storm"
                                console.debug("unknown condition " + condition)

                                if (clouds === "clear sky") return "clear";
                                if (clouds === "few clouds") return "few-clouds";
                                if (clouds === "scattered clouds") return "few-clouds";
                                if (clouds === "broken clouds") return "clouds";
                                if (clouds === "overcast") return "many-clouds";
                                console.debug("unknown clouds " + clouds)

                                return ""
                            }
                        }
                    }
                }

                ListView {
                    id: panoramioView
                    anchors.right: parent.right
                    visible: listmodel.count > 0
                    height: parent.height
                    width: 200
                    spacing: 4
                    orientation: ListView.Horizontal
                    snapMode: ListView.SnapToItem
                    clip: true

                    model: listmodel
                    delegate: Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        color: panoramioView.currentIndex === index ? "lightblue" : "darkgray"
                        smooth: true
                        width: 64
                        height: 64
                        scale: panoramioImage.status === Image.Ready ? 1.0 : 0.0

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
                            onClicked: Qt.openUrlExternally(link)
                        }

                        Behavior on scale {
                            NumberAnimation {
                                easing.type: Easing.InOutQuad
                                duration: 500
                            }
                        }
                    }
                }

                Label {
                    id: legalText
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: panoramioView.bottom
                    anchors.topMargin: 5
                    visible: listmodel.count > 0
                    wrapMode: Text.WordWrap
                    color: "darkgray"
                    text: "<font size=\"-3\">Photos provided by Panoramio. Photos are under the copyright of their owners.</font>"
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
                    panoramioView.currentIndex=-1
                }
            }
            xhr.send();
        }
    }

    XmlListModel {
         id: weatherModel

         property double longitude: root.placemark != undefined ? root.placemark.coordinate.longitude : 0.0
         property double latitude: root.placemark != undefined ? root.placemark.coordinate.latitude : 0.0

         source: "http://ws.geonames.org/findNearByWeatherXML?lat=" + latitude + "&lng=" + longitude
         query: "/geonames/observation"

         XmlRole { name: "temperature"; query: "temperature/string()" }
         XmlRole { name: "weatherCondition"; query: "weatherCondition/string()" }
         XmlRole { name: "clouds"; query: "clouds/string()" }
     }

    Connections { target: marbleWidget; onPlacemarkSelected: root.editPlacemark(placemark) }
}
