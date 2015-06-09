//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QTreeView>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/ParsingRunnerManager.h>
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

    MarbleModel model;
    ParsingRunnerManager manager( model.pluginManager() );

    GeoDataDocument* document = manager.openFile( inputFile.absoluteFilePath() );
    if ( !document ) {
        qDebug() << "Unable to open " << inputFile.absoluteFilePath();
        return 2;
    }

    GeoDataTreeModel treeModel;
    treeModel.addDocument( document );

    QTreeView treeView;
    treeView.setModel( &treeModel );
    treeView.show();

    return app.exec();
}
