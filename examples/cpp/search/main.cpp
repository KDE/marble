//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include <QApplication>
#include <QDebug>

#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/SearchRunnerManager.h>
#include <GeoDataPlacemark.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app( argc, argv );

    MarbleModel model;
    SearchRunnerManager manager( &model );

    QVector<GeoDataPlacemark*> searchResult = manager.searchPlacemarks( "Karlsruhe" );
    foreach( const GeoDataPlacemark* placemark, searchResult ) {
        qDebug() << "Found" << placemark->name() << "at" << placemark->coordinate().toString();
    }

    qDeleteAll( searchResult );
}
