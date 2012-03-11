//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

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
