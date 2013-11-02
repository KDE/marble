//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

#include <MarbleWidget.h>
#include <RenderPlugin.h>

#include <QDebug>
#include <QApplication>
#include <QPixmap>
#include <QPainter>

using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    if ( argc < 2 ) {
        qDebug() << "Usage: " << argv[0] << " <maptheme> [output.png]";
        qDebug() << "Where <maptheme> is a map theme id a la 'body/maptheme/maptheme.dgml'.";
        return 1;
    }

    MarbleWidget *mapWidget = new MarbleWidget;
    mapWidget->setMapThemeId( argv[1] );
    QStringList const features = QStringList() << "atmosphere";
    foreach( RenderPlugin* plugin, mapWidget->renderPlugins() ) {
        if ( !features.contains( plugin->nameId() ) ) {
            plugin->setEnabled( false );
        }
    }
    double const scale = 4.0;
    mapWidget->resize( 130 * scale, 130 * scale );
    mapWidget->centerOn( 49.59526, 18.48104 );
    mapWidget->setRadius( 120 * scale / 2.0 );

    QPixmap canvas( ":/canvas.png" );
    QPainter globePainter( &canvas );
    QPixmap const globe = QPixmap::grabWidget( mapWidget );
    globePainter.drawPixmap( QPoint( 2, 2 ), globe.scaled( 130, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );
    globePainter.end();
    canvas.save( argc > 2 ? argv[2] : "preview.png" );

    return 0;
}
