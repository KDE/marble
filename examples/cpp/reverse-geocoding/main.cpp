#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleRunnerManager.h>
#include <marble/GeoDataPlacemark.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app( argc, argv );

    MarbleModel model;
    MarbleRunnerManager manager( model.pluginManager() );
    manager.setModel( &model );

    GeoDataCoordinates position( -0.15845,  51.52380, 0.0, GeoDataCoordinates::Degree );
    qDebug() << position.toString() << "is" << manager.searchReverseGeocoding( position );
}
