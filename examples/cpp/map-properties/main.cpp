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
    mapWidget->setMapThemeId("earth/bluemarble/bluemarble.dgml");

    mapWidget->setProjection( Mercator );

    // Enable the cloud cover and enable the country borders
    mapWidget->setShowClouds( true );
    mapWidget->setShowBorders( true );

    // Hide the FloatItems: Compass and StatusBar
    mapWidget->setShowOverviewMap(false);
    mapWidget->setShowScaleBar(false);

    foreach ( AbstractFloatItem * floatItem, mapWidget->floatItems() )
        if ( floatItem && floatItem->nameId() == "compass" ) {

            // Put the compass onto the left hand side
            floatItem->setPosition( QPoint( 10, 10 ) );
            // Make the content size of the compass smaller
            floatItem->setContentSize( QSize( 50, 50 ) );
        }

    mapWidget->resize( 400, 300 );
    mapWidget->show();

    return app.exec();
}
