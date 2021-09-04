//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>

#include <marble/MarbleGlobal.h>
#include <marble/MarbleWidget.h>
#include <marble/AbstractFloatItem.h>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    // Create a Marble QWidget without a parent
    MarbleWidget *mapWidget = new MarbleWidget();

    // Load the OpenStreetMap map
    mapWidget->setMapThemeId(QStringLiteral("earth/bluemarble/bluemarble.dgml"));

    mapWidget->setProjection( Mercator );

    // Enable the cloud cover and enable the country borders
    mapWidget->setShowClouds( true );
    mapWidget->setShowBorders( true );

    // Hide the FloatItems: Compass and StatusBar
    mapWidget->setShowOverviewMap(false);
    mapWidget->setShowScaleBar(false);

    foreach ( AbstractFloatItem * floatItem, mapWidget->floatItems() )
        if (floatItem && floatItem->nameId() == QLatin1String("compass")) {

            // Put the compass onto the left hand side
            floatItem->setPosition( QPoint( 10, 10 ) );
            // Make the content size of the compass smaller
            floatItem->setContentSize( QSize( 50, 50 ) );
        }

    mapWidget->resize( 400, 300 );
    mapWidget->show();

    return app.exec();
}
