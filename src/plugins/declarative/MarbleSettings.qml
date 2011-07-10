import Qt 4.7
import org.kde.edu.marble 0.11

Item {
    id: root

    Settings {
        id: settings
    }

    property string mapTheme: settings.value( "MarbleWidget", "mapTheme", "earth/openstreetmap/openstreetmap.dgml" )
    onMapThemeChanged: settings.setValue( "MarbleWidget", "mapTheme", mapTheme )

    property bool workOffline: settings.value( "MainWindow", "workOffline", false )
    onWorkOfflineChanged: settings.setValue( "MainWindow", "workOffline", workOffline )

    property real quitLongitude: settings.value( "MarbleWidget", "quitLongitude", 0.0 )
    onQuitLongitudeChanged: settings.setValue( "MarbleWidget", "quitLongitude", quitLongitude )

    property real quitLatitude: settings.value( "MarbleWidget", "quitLatitude", 0.0 )
    onQuitLatitudeChanged: settings.setValue( "MarbleWidget", "quitLatitude", quitLatitude )

    property real quitZoom: settings.value( "MarbleWidget", "quitZoom", 1000.0 )
    onQuitZoomChanged: settings.setValue( "MarbleWidget", "quitZoom", quitZoom )
}
