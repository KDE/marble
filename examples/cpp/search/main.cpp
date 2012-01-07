#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleRunnerManager.h>
#include <marble/GeoDataPlacemark.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    MarbleModel *model = new MarbleModel;

    MarbleRunnerManager* manager = new MarbleRunnerManager( model->pluginManager() );
    manager->setModel( model );

    QVector<GeoDataPlacemark*> searchResult = manager->searchPlacemarks( "Karlsruhe" );
    foreach( GeoDataPlacemark* placemark, searchResult ) {
        qDebug() << "Found " << placemark->name() << "at" << placemark->coordinate().toString();
    }
}
