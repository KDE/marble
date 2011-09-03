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

/* 
 * Main window of the application. Also contains activity properties,
 * settings and manages plugin states on activity changes.
 */
PageStackWindow {
    id: main
    // Use full screen.
    // FIXME Fix for desktop screens.
    width: screen.displayWidth
    height: screen.displayHeight
    platformStyle: defaultStyle
    initialPage: activitySelection
    property alias activityModel: activitySelection.model
    
    // System dependent style for the main window.
    PageStackWindowStyle {
        id: defaultStyle
    }

    // Stores the settings of the application.
    MarbleSettings {
        id: settings
    }
    
    // Default toolbar layout for pages if there is no other defined.
    ToolBarLayout {
        id: commonToolBar
        visible: false
        ToolIcon { iconId: "toolbar-back"; onClicked: { pageStack.pop() } }
    }
    
    // Displays all available activities and starts them if the user clicks on them.
    ActivitySelectionView {
        id: activitySelection
        onActivityChanged: {
            console.log( "onActivityChanged", oldActivity, newActivity )
            main.changeActivity( oldActivity, newActivity )
        }
    }

    // Returns the model which contains routing instructions.
    function routeRequestModel() {
        return mainWidget.routeRequestModel()
    }
    
    // Returns the model which contains points on the route.
    function waypointModel() {
        return mainWidget.waypointModel()
    }
    
    // Returns object to define and clear route.
    function getRouting() {
        return mainWidget.getRouting()
    }

    // Returns object to define and clear searches.
    function getSearch() {
        return mainWidget.getSearch()
    }
    
    // Applies all changes necessary to switch from "oldActivity" to "newActivity".
    function changeActivity( oldActivity, newActivity ) {
        console.log( "changeActivity: ", oldActivity, " - " , newActivity )
        adjustPlugins( oldActivity, newActivity )
        adjustSettings( newActivity )
    }
    
    // Enables the plugin with the passed name.
    function enablePlugin( name ) {
        console.log( "trying to enable ", name )
        var tmp = settings.activeRenderPlugins
        // Only add plugin if it is not already enabled.
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
    
    // Disables the plugin with the passed name.
    function disablePlugin( name ) {
        console.log( "trying to disable ", name )
        var tmp = new Array()
        // Add all enabled plugins, except the one with the passed name.
        for( var i = 0; i < settings.activeRenderPlugins.length; i++ ) {
            if( settings.activeRenderPlugins[i] != name ) {
                tmp.push( settings.activeRenderPlugins[i] )
            }
            else {
                console.log( "- disabled: ", name )
            }
        }
        // Apply changes.
        settings.activeRenderPlugins = tmp
        console.log( "finished disablePlugin ", name )
    }
    
    // Adjust plugins for switching from "oldActivity" to "newActivity".
    function adjustPlugins( oldActivity, newActivity ) {
        console.log( "adjustingPlugins ", oldActivity, newActivity )
        var preserve = undefined
        var enable = activitySelection.model.get( newActivity, "enablePlugins" )
        var disable = activitySelection.model.get( newActivity, "disablePlugins" )
        var oldRelated = activitySelection.model.get( oldActivity, "relatedActivities" )
        // Preserve related plugins if "newActivity" is a related activity.
        if( oldRelated != undefined ) {
            preserve = oldRelated[newActivity]
        }
        // Enable plugins for "newActivity".
        if( enable != undefined ) {
            for( var i = 0; i < enable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( enable[i] ) == -1 ) {
                    enablePlugin( enable[i] )
                }
            }
        }
        // Disable plugins for "newActivity".
        if( disable != undefined ) {
            for( var i = 0; i < disable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( disable[i] ) == -1 ) {
                    disablePlugin( disable[i] )
                }
            }
        }
        console.log( "finished adjusting plugins" )
    }

    // Adjust settings for passed activity.
    function adjustSettings( activity ) {
        console.log( "adjustSettings: ", activity )
        var changes = activitySelection.model.get( activity, "settings" )
        for( var i in changes ) {
            console.log( "- ", i, changes[i] )
            settings[i] = changes[i]
        }
    }

}