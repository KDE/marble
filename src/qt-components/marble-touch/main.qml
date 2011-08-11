// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import org.kde.edu.marble.qtcomponents 0.12
import org.kde.edu.marble 0.11
import QtQuick 1.0
import com.nokia.meego 1.0

PageStackWindow {
    id: main
    width: screen.displayWidth
    height: screen.displayHeight
    platformStyle: defaultStyle
    initialPage: activitySelection
    property alias activityModel: activitySelection.model
    
    PageStackWindowStyle {
        id: defaultStyle
    }

    MarbleSettings {
        id: settings
    }
    
    ToolBarLayout {
        id: commonToolBar
        visible: false
        ToolIcon { iconId: "toolbar-back"; onClicked: { pageStack.pop() } }
    }
    
    ActivitySelectionView {
        id: activitySelection
        onActivityChanged: {
            console.log( "onActivityChanged", oldActivity, newActivity )
            main.changeActivity( oldActivity, newActivity )
        }
    }

    function routeRequestModel() {
        return mainWidget.routeRequestModel()
    }
    
    function waypointModel() {
        return mainWidget.waypointModel()
    }
    
    function getRouting() {
        return mainWidget.getRouting()
    }

    function getSearch() {
        return mainWidget.getSearch()
    }
    
    function changeActivity( oldActivity, newActivity ) {
        console.log( "changeActivity: ", oldActivity, newActivity )
        adjustPlugins( oldActivity, newActivity )
        adjustSettings( newActivity )
    }
    
    function enablePlugin( name ) {
        console.log( "trying to enable ", name )
        var tmp = settings.activeRenderPlugins
        if( tmp.indexOf( name ) == -1 ) {
            console.log( "- enabling: ", name )
            tmp.push( name )
            settings.activeRenderPlugins = tmp
        }
        else {
            console.log( "- ", name, " is already enabled" )
        }
        console.log( "finished enablePlugin ", name )
    }
    
    function disablePlugin( name ) {
        console.log( "trying to disable ", name )
        var tmp = new Array()
        for( var i = 0; i < settings.activeRenderPlugins.length; i++ ) {
            if( settings.activeRenderPlugins[i] != name ) {
                tmp.push( settings.activeRenderPlugins[i] )
            }
            else {
                console.log( "- disabled: ", name )
            }
        }
        settings.activeRenderPlugins = tmp
        console.log( "finished disablePlugin ", name )
    }
    
    function adjustPlugins( oldActivity, newActivity ) {
        console.log( "adjustingPlugins ", oldActivity, newActivity )
        var preserve = undefined
        var enable = activitySelection.model.get( newActivity, "enablePlugins" )
        var disable = activitySelection.model.get( newActivity, "disablePlugins" )
        var oldRelated = activitySelection.model.get( oldActivity, "relatedActivities" )
        if( oldRelated != undefined ) {
            preserve = oldRelated[newActivity]
        }
        if( enable != undefined ) {
            for( var i = 0; i < enable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( enable[i] ) == -1 ) {
                    enablePlugin( enable[i] )
                }
            }
        }
        if( disable != undefined ) {
            for( var i = 0; i < disable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( disable[i] ) == -1 ) {
                    disablePlugin( disable[i] )
                }
            }
        }
        console.log( "finished adjusting plugins" )
    }

    function adjustSettings( activity ) {
        console.log( "adjustSettings: ", activity )
        var changes = activitySelection.model.get( activity, "settings" )
        for( var i in changes ) {
            console.log( "- ", i, changes[i] )
            settings[i] = changes[i]
        }
    }

}