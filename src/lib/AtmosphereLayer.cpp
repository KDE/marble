//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//

#include "AtmosphereLayer.h"

#include "GeoPainter.h"
#include "ViewportParams.h"

namespace Marble
{

QStringList AtmosphereLayer::renderPosition() const
{
    return QStringList() << "BEHIND_TARGET";
}

bool AtmosphereLayer::render( GeoPainter *painter,
                              ViewportParams *viewParams,
                              const QString &renderPos,
                              GeoSceneLayer *layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    // Only draw an atmosphere if projection is spherical
    if ( viewParams->projection() != Spherical )
        return true;

    // No use to draw atmosphere if it's not visible in the area.
    if ( viewParams->mapCoversViewport() )
        return true;

    // Ok, now we know that at least a little of the atmosphere is
    // visible, if nothing else in the corners.  Draw the atmosphere
    // by using a circular gradient.  This is a pure visual effect and
    // has nothing to do with real physics.

    int  imageHalfWidth  = viewParams->width() / 2;
    int  imageHalfHeight = viewParams->height() / 2;

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imageHalfWidth, imageHalfHeight ),
                           1.05 * viewParams->radius() );
    grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
    grad1.setColorAt( 1.00, QColor( 255, 255, 255, 0 ) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );

    painter->setBrush( brush1 );
    painter->setPen( pen1 );
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->drawEllipse( imageHalfWidth  - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                          imageHalfHeight - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                          (int) ( 2.1 * (qreal) ( viewParams->radius()) ),
                          (int) ( 2.1 * (qreal) ( viewParams->radius()) ) );

    return true;
}

}
