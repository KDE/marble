// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QTreeView>

#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>
#include <marble/ParsingRunnerManager.h>

using namespace Marble;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QFileInfo inputFile(app.arguments().last());
    if (app.arguments().size() < 2 || !inputFile.exists()) {
        qWarning() << "Usage: " << app.arguments().first() << "file.kml";
        return 1;
    }

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());

    GeoDataDocument *document = manager.openFile(inputFile.absoluteFilePath());
    if (!document) {
        qDebug() << "Unable to open " << inputFile.absoluteFilePath();
        return 2;
    }

    GeoDataTreeModel treeModel;
    treeModel.addDocument(document);

    QTreeView treeView;
    treeView.setModel(&treeModel);
    treeView.show();

    return app.exec();
}
