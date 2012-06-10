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
            settings.projection = "Spherical"
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
                id: appearAnimation
                PauseAnimation { duration: Math.random() * 100 }
                NumberAnimation {
                    target: venueRectangle
                    property: "scale"
                    to: 1.0
                    duration: 150
                }
            }
            
            Component.onCompleted: {
                appearAnimation.running = true
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
            }

            MouseArea {
                anchors.fill: parent
                onClicked: container.focus = true
            }
        }
    }
}
