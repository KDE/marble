import org.kde.edu.marble 0.20
import QtQuick 2.3
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.2

ApplicationWindow {
    id: mainWindow

    width: 800
    height: 600

    property Item marbleWidget: MainWidget {}
    property bool inPortrait: width < height

    toolBar: RowLayout {
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }

        function replaceWith( item ) {
            for(var i = toolBar.children.length; i > 0 ; i--) {
              toolBar.children[i-1].destroy()
            }
            item.parent = toolBar
        }
    }

    // Stores the settings of the application.
    MarbleSettings {
        id: settings
    }

    ActivitySelectionView {
        id: activitySelection
        anchors.fill: parent
    }

    function openActivity( activity ) {
        activitySelection.openActivity( activity )
    }

    function openPage( path ) {
        pageStack.push( path )
    }

    Component.onCompleted: {
        if ( settings.lastActivity === "" ) {
            activitySelection.initializeDelayed()
        } else {
            activitySelection.openActivity( settings.lastActivity )
        }
    }

    function icon( name, size ) {
        if ( name === "actions/go-previous-view" ) {
            return "image://theme/icon-m-toolbar-back"
        } else if ( name === "actions/edit-find" ) {
            return "image://theme/icon-m-toolbar-search"
        } else if ( name === "actions/configure" ) {
            return "image://theme/icon-m-toolbar-settings"
        } else if ( name === "actions/go-up" ) {
            return "image://theme/icon-m-toolbar-up"
        } else if ( name === "places/user-identity" ) {
            return "image://theme/icon-s-common-location"
        } else if ( name === "devices/network-wireless" ) {
            return "qrc:/marble/wireless.svg"
        } else if ( name === "actions/show-menu" ) {
            return "image://theme/icon-m-toolbar-view-menu"
        } else if ( name === "actions/document-edit" ) {
            return "image://theme/icon-m-toolbar-edit"
        } else if ( name === "actions/edit-clear-locationbar-rtl" ) {
            return "image://theme/icon-m-input-clear"
        } else if ( name === "actions/text-speak" ) {
            return "image://theme/icon-m-toolbar-volume"
        } else if ( name === "status/task-attention" ) {
            return "image://theme/icon-l-error"
        } else if ( name === "actions/media-playback-start" ) {
            return "image://theme/icon-m-toolbar-mediacontrol-play"
        } else if ( name === "actions/dialog-cancel" ) {
            return "image://theme/icon-s-cancel"
        } else if ( name === "actions/dialog-close" ) {
            return "image://theme/icon-s-cancel"
        } else if ( name === "places/folder" ) {
            return "image://theme/icon-m-common-directory"
        } else if ( name === "mimetypes/unknown" ) {
            return "image://theme/icon-m-content-document"
        } else if ( name === "places/favorites" ) {
            return "image://theme/icon-m-toolbar-favorite-mark"
        } else if ( name === "actions/go-home" ) {
            return "image://theme/icon-m-toolbar-home"
        } else if ( name === "actions/download" ) {
            return "image://theme/icon-s-transfer-download"
        } else if ( name === "actions/upload" ) {
            return "image://theme/icon-s-transfer-upload"
        }

        return name
    }
}

