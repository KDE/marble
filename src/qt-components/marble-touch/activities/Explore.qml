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
            property int padding: 7
            width: container.width + padding
            height: container.height + padding
            color: "#39AC39"
            radius: 3
            
            Item {
                id: container
                width: categoryIcon.width + 3 + venueName.width
                height: categoryIcon.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                
                Image {
                    id: categoryIcon
                    source: categoryIconUrl
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    id: venueName
                    text: name
                    anchors.left: categoryIcon.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

}