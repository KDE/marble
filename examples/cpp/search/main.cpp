//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QDebug>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/SearchRunnerManager.h>
#include <marble/GeoDataPlacemark.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app( argc, argv );

    MarbleModel model;
    SearchRunnerManager manager( &model );

    QVector<GeoDataPlacemark*> searchResult = manager.searchPlacemarks(QStringLiteral("Karlsruhe"));
    foreach( const GeoDataPlacemark* placemark, searchResult ) {
        qDebug() << "Found" << placemark->name() << "at" << placemark->coordinate().toString();
    }

    qDeleteAll( searchResult );
}
