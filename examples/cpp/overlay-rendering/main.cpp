#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QImage>

#include <marble/MarbleWidget.h>
#include <GeoDataDocument.h>
#include <GeoDataGroundOverlay.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>

using namespace Marble;

int main(int argc, char** argv) {

    QApplication app(argc,argv);

    QFileInfo inputFile( app.arguments().last() );
    if ( app.arguments().size() < 2 || !inputFile.exists() ) {
        qWarning() << "Usage: " << app.arguments().first() << "file.png";
        return 1;
    }

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the Satellite map
    mapWidget->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );

    // Create a bounding box from the given corner points
    GeoDataLatLonBox box( 55, 48, 14.5, 6, GeoDataCoordinates::Degree );
    box.setRotation( 0, GeoDataCoordinates::Degree );

    // Create an overlay and assign the image to render and its bounding box to it
    GeoDataGroundOverlay *overlay = new GeoDataGroundOverlay;
    overlay->setLatLonBox( box );
    overlay->setIcon( QImage( inputFile.absoluteFilePath() ) );

    // Create a document as a container for the overlay
    GeoDataDocument *document = new GeoDataDocument();
    document->append( overlay );

    // Add the document to MarbleWidget's tree model
    mapWidget->model()->treeModel()->addDocument( document );

    mapWidget->show();

    return app.exec();
}

