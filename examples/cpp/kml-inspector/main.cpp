#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QTreeView>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleRunnerManager.h>
#include <marble/GeoDataTreeModel.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    QFileInfo inputFile( app.arguments().last() );
    if ( app.arguments().size() < 2 || !inputFile.exists() ) {
        qWarning() << "Usage: " << app.arguments().first() << "file.kml";
        return 1;
    }

    MarbleModel *model = new MarbleModel;
    MarbleRunnerManager* manager = new MarbleRunnerManager( model->pluginManager() );

    GeoDataDocument* document = manager->openFile( inputFile.absoluteFilePath() );
    if ( document ) {
        GeoDataTreeModel* treeModel = new GeoDataTreeModel;
        treeModel->addDocument( document );
        QTreeView* treeView = new QTreeView;
        treeView->setModel( treeModel );
        treeView->show();
    } else {
        qDebug() << "Unable to open " << inputFile.absoluteFilePath();
    }

    return app.exec();
}
