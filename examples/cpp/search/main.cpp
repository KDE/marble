// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QDebug>

#include <marble/GeoDataPlacemark.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>
#include <marble/SearchRunnerManager.h>

using namespace Marble;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MarbleModel model;
    SearchRunnerManager manager(&model);

    const QList<GeoDataPlacemark *> searchResult = manager.searchPlacemarks(QStringLiteral("Karlsruhe"));
    for (const GeoDataPlacemark *placemark : searchResult) {
        qDebug() << "Found" << placemark->name() << "at" << placemark->coordinate().toString();
    }

    qDeleteAll(searchResult);
}
