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
#include <QProcess>
#include <QDir>

using namespace Marble;

QPixmap resize(const QPixmap &pixmap)
{
    if ( QProcess::execute("convert -version") == 0 ) {
        QString const inputFile = QDir::tempPath() + "/marble-preview.png";
        QString const outputFile = QDir::tempPath() + "/marble-preview-scaled.png";
        if ( pixmap.save( inputFile )
             && QProcess::execute( "convert", QStringList() << inputFile << "-resize" << "130x130"
                                   << "-sharpen" << "1x1" << outputFile ) == 0
             ) {
            QPixmap result( outputFile );
            if ( !result.isNull() ) {
                return result;
            }
        }
    }

    qDebug() << "Warning: Unable to resize pixmap properly. Check imagemagick installation.";
    return pixmap.scaled( 130, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

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
    Q_ASSERT(!canvas.isNull());
    QPainter globePainter( &canvas );
    QPixmap const globe = QPixmap::grabWidget( mapWidget );
    globePainter.drawPixmap( QPoint( 2, 2 ), resize( globe ) );
    globePainter.end();
    canvas.save( argc > 2 ? argv[2] : "preview.png" );

    return 0;
}
