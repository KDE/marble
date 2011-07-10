import Qt 4.7
import org.kde.edu.marble 0.11

Item {
    id: root

    Settings {
        id: settings
    }

    property string mapTheme: settings.value( "MarbleWidget", "mapTheme", "earth/openstreetmap/openstreetmap.dgml" )
    property bool workOffline: settings.value( "MainWindow", "workOffline", false )
    property real quitLongitude: settings.value( "MarbleWidget", "quitLongitude", 0.0 )
    property real quitLatitude: settings.value( "MarbleWidget", "quitLatitude", 0.0 )
    property real quitZoom: settings.value( "MarbleWidget", "quitZoom", 1000.0 )
    
    Component.onDestruction: {
        console.log( "saving settings: ", root.mapTheme, root.workOffline, 
                                          root.quitLongitude, root.quitLatitude,
                                          root.quitZoom )
        settings.setValue( "MarbleWidget", "mapTheme", root.mapTheme )
        settings.setValue( "MainWindow", "workOffline", root.workOffline )
        settings.setValue( "MarbleWidget", "quitLongitude", root.quitLongitude )
        settings.setValue( "MarbleWidget", "quitLatitude", root.quitLatitude )
        settings.setValue( "MarbleWidget", "quitZoom", root.quitZoom )
    }
}
