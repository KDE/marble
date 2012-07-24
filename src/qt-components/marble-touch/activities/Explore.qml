// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

Page {
    id: exploreActivityPage
    anchors.fill: parent

    property bool horizontal: width / height > 1.20
    
    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-previous-view", 48 );
            onClicked: pageStack.pop()
        }
    }
    
    Item {
        id: mapContainer
        anchors.fill: parent
        clip: true

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            plugins.push( "foursquare" )
            
            settings.activeRenderPlugins =  plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPositionIndicator = true
            settings.showTrack = false
            
            marbleWidget.tracking.positionMarkerType = Tracking.Circle
            marbleWidget.visible = true
            marbleWidget.setDataPluginDelegate( "foursquare", foursquareDelegate )
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.setDataPluginDelegate( "foursquare", 0 )
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
    
    Component {
        id: foursquareDelegate

        Rectangle {
            id: venueRectangle
            property int padding: 7
            width: container.width + padding
            height: container.height + padding
            scale: 0.0
            color: "#88D788"
            border.color: "#39AC39"
            border.width: 2
            radius: 3

            SequentialAnimation {
                id: venueAppearAnimation
                PauseAnimation { duration: Math.random() * 100 }
                NumberAnimation {
                    target: venueRectangle
                    property: "scale"
                    to: 1.0
                    duration: 150
                }
            }
            
            Component.onCompleted: {
                venueAppearAnimation.running = true
            }

            Item {
                id: container
                width: 32 + ( focus ? venueName.width + venueName.anchors.leftMargin : 0 )
                height: Math.max( 32, venueName.height )
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Rectangle {
                    id: iconFallBack
                    width: 30
                    height: width
                    radius: 5
                    color: "white"
                    border.width: 2
                    border.color: "darkgray"
                }

                Image {
                    id: categoryIcon
                    source: categoryIconUrl
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    id: venueName
                    scale: container.focus ? 1.0 : 0.0
                    text: name
                    anchors.left: iconFallBack.right
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    visible: parent.focus ? true : false

                    Behavior on scale { NumberAnimation { duration: 150 } }
                }

                Behavior on width { NumberAnimation { duration: 150 } }
                onFocusChanged: sticky = focus
            }

            function updateDetails() {
                venueDetails.venueId = identifier
                venueDetails.name = name
                venueDetails.category = category
                venueDetails.address = address
                venueDetails.city = city
                venueDetails.country = country
                venueDetails.usersCount = usersCount
                venueDetails.largeIcon = categoryLargeIconUrl
            }
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if( container.focus == true ) {
                        updateDetails()
                        listModel.retrieve()
                        venueDetails.visible = true
                    } else {
                        container.focus = true
                    }
                }
            }
        }
    }

    Rectangle {
        id: venueDetails
        
        anchors.bottom: exploreActivityPage.bottom
        width: exploreActivityPage.horizontal ? exploreActivityPage.width / 4 : exploreActivityPage.width
        height: exploreActivityPage.horizontal ? exploreActivityPage.height : exploreActivityPage.height / 3
        visible: false
        radius: 10
        color: "#f7f7f7"
        border.width: 2
        border.color: "darkgray"
        
        property string venueId
        property string name
        property string category
        property string address
        property string city
        property string country
        property string usersCount
        property string largeIcon
        
        Image {
            id: detailIcon
            source: venueDetails.largeIcon
            anchors.top: venueDetails.top
            anchors.left: detailName.left
            anchors.topMargin: 10
        }
        
        Text {
            id: detailName
            text: venueDetails.name
            anchors.top: detailIcon.bottom
            anchors.left: venueDetails.left
            anchors.topMargin: 5
            anchors.leftMargin: 7
        }
        
        Text {
            id: detailCategory
            text: venueDetails.category
            anchors.top: detailName.bottom
            anchors.left: detailName.left
        }
        
        Text {
            id: detailAddressTitle
            text: qsTr( "Address" )
            anchors.top: detailCategory.bottom
            anchors.left: detailName.left
            anchors.topMargin: 5
            font.bold: true
        }
        
        Text {
            id: detailAddress
            text: venueDetails.address
            anchors.top: detailAddressTitle.bottom
            anchors.left: detailName.left
        }
        
        Text {
            id: detailCity
            text: venueDetails.city
            anchors.top: detailAddress.bottom
            anchors.left: detailName.left
        }
        
        Text {
            id: detailCountry
            text: venueDetails.country
            anchors.top: detailCity.bottom
            anchors.left: detailName.left
        }
        
        ListView {
            id: photoView
            width: exploreActivityPage.horizontal ? venueDetails.width : 64
            
            anchors.top: exploreActivityPage.horizontal ? detailCountry.bottom : venueDetails.top
            anchors.left: exploreActivityPage.horizontal ? venueDetails.left : venueDetails.horizontalCenter
            anchors.bottom: venueDetails.bottom
            anchors.right:venueDetails.right
            anchors.margins: 5
            spacing: 2
            
            model: listModel
            orientation: ListView.Vertical 
            clip: true
            
            delegate: Image {
                id: venuePhoto
                width: venueDetails.width
                scale: 0.0
                fillMode: Image.PreserveAspectFit
                source: url
                smooth: true
                             
                SequentialAnimation {
                    id: photoAppearAnimation
                    PauseAnimation { duration: Math.random() * 100 }
                    NumberAnimation {
                        target: venuePhoto
                        property: "scale"
                        to: 1.0
                        duration: 150
                    }
                }
                
                Component.onCompleted: {
                    photoAppearAnimation.running = true
                }
            }
        }
        
        Item {
            id: modelData
            property string clientId: "YPRWSYFW1RVL4PJQ2XS5G14RTOGTHOKZVHC1EP5KCCCYQPZF"
            property string clientSecret: "5L2JDCAYQCEJWY5FNDU4A1RWATE4E5FIIXXRM41YBTFSERUH"
            property string source: "https://api.foursquare.com/v2/venues/" + venueDetails.venueId + "/photos?v=20120617&group=venue&limit=5&client_id=" + clientId + "&client_secret=" + clientSecret
            
            ListModel {
                id: listModel
                property string oldId
                
                function retrieve() {
                    if( oldId == venueDetails.venueId ) {
                        return
                    }
                    
                    var xhr = new XMLHttpRequest;
                    xhr.open( "GET", modelData.source );
                    xhr.onreadystatechange = function() {
                        if( xhr.readyState === XMLHttpRequest.DONE ) {
                            listModel.oldId = venueDetails.venueId
                            listModel.clear()
                            var data = JSON.parse( xhr.responseText );
                            for( var item in data.response.photos.items ) {
                                var photo = data.response.photos.items[ item ];
                                listModel.append({
                                    url: photo.prefix + "original" + photo.suffix,
                                    width: photo.width,
                                    height: photo.height
                                });
                            }
                        }
                    }
                    
                    xhr.send();
                }
            }
        }
        
        Behavior on width { NumberAnimation { duration: 150 } }
    }
}
