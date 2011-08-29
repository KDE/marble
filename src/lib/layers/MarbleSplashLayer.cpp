//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "MarbleSplashLayer.h"

#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

#include <QtGui/QPixmap>

namespace Marble
{

MarbleSplashLayer::MarbleSplashLayer()
{
}

QStringList MarbleSplashLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool MarbleSplashLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    painter->save();

    QPixmap logoPixmap( MarbleDirs::path( "svg/marble-logo-inverted-72dpi.png" ) );

    if ( logoPixmap.width() > viewport->width() * 0.7
         || logoPixmap.height() > viewport->height() * 0.7 )
    {
        logoPixmap = logoPixmap.scaled( QSize( viewport->width(), viewport->height() ) * 0.7,
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation );
    }

    QPoint logoPosition( ( viewport->width()  - logoPixmap.width() ) / 2,
                         ( viewport->height() - logoPixmap.height() ) / 2 );
    painter->drawPixmap( logoPosition, logoPixmap );

    QString message; // "Please assign a map theme!";

    painter->setPen( Qt::white );

    int yTop = logoPosition.y() + logoPixmap.height() + 10;
    QRect textRect( 0, yTop,
                    viewport->width(), viewport->height() - yTop );
    painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, message );

    painter->restore();

    return true;
}


}
