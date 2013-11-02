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
#include <marble/ReverseGeocodingRunnerManager.h>
#include <GeoDataPlacemark.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app( argc, argv );

    MarbleModel model;
    ReverseGeocodingRunnerManager manager( &model );

    GeoDataCoordinates position( -0.15845,  51.52380, 0.0, GeoDataCoordinates::Degree );
    qDebug() << position.toString() << "is" << manager.searchReverseGeocoding( position );
}
