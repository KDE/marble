#include <QtGui/QApplication>
#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/RouteRequest.h>
#include <marble/RoutingManager.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the OpenStreetMap map
    mapWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    mapWidget->setProjection( Mercator );

    // Access the shared route request (start, destination and parameters)
    RoutingManager* manager = mapWidget->model()->routingManager();
    RouteRequest* request = manager->routeRequest();

    // Use default routing settings for cars
    request->setRoutingProfile( manager->defaultProfile( RoutingProfile::Motorcar ) );

    // Set start and destination
    request->append( GeoDataCoordinates( 8.38942, 48.99738, 0.0, GeoDataCoordinates::Degree ) );
    request->append( GeoDataCoordinates( 8.42002, 49.0058, 0.0, GeoDataCoordinates::Degree ) );

    // Calculate the route
    manager->retrieveRoute();

    // Center the map on the route start point and show it
    mapWidget->centerOn( request->at( 0 ) );
    mapWidget->setDistance( 0.75 );
    mapWidget->show();

    return app.exec();
}
