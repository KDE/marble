//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include <QtCore/QVector>
#include <QtCore/QDebug>
#include <QtGui/QImage>
#include <QtGui/QApplication>


#define ENABLEGUI

int main(int argc, char *argv[])
{
    QCoreApplication  app(argc, argv);

    uint    level = 8;
    QImage  imgsrc( "../data/maps/etopo2/etopo2.jpg" );
    QImage  imgscl;
    QImage  tile;
    QImage  result;
#if 0
    QVector<QRgb>  legpal;
    for ( int count = 0; count < 255; count++) {
        legpal.insert(count, qRgb(count, count, count));
    }
#endif

    for ( uint count = 1; count <= level; count *= 2 ) {
        imgscl = imgsrc.scaled( imgsrc.width() / count,
                                imgsrc.height() / count, Qt::KeepAspectRatio );
        for ( uint col = 0; col < 2 * level / count; ++col ) {
            for ( uint row = 0; row < level / count; ++row ) {
                tile = imgscl.copy( col * imgsrc.width() / level / 2,
                                    row * imgsrc.height() / level,
                                    imgsrc.width() / level / 2,
                                    imgsrc.height() / level );
                QString  path = QString( "../data/maps/etopo2/etopo_%1_%2x%3.jpg" ).arg( level / count ).arg( col ).arg( row );
                // result=tile.convertToFormat(QImage::Format_Indexed8, legpal, Qt::ThresholdDither);
                result = tile;
                qDebug();
#if 0
                for (int x=0; x < result.width(); x++) {
                    for (int y=0; y < result.height(); y++) {
                        if (result.pixelIndex(x,y) < 20) {
                            result.setPixel(x,y,20);
                        }
                        if (result.pixelIndex(x,y) == 0) {
                            qDebug("Yes");
                        }
                    }
                }
#endif
                result.save( path, "jpg", 95 );
                qDebug( ) << "Saving tile " << path;
            }
        }
    }

    app.exit();
}
