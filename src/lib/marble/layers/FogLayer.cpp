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

#include "FogLayer.h"

#include "GeoPainter.h"
#include "ViewportParams.h"
#include "RenderState.h"

namespace Marble
{
    
QStringList FogLayer::renderPosition() const
{
    return QStringList(QStringLiteral("ATMOSPHERE"));
}

bool FogLayer::render( GeoPainter *painter,
                       ViewportParams *viewParams,
                       const QString &renderPos,
                       GeoSceneLayer *layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    // FIXME: The fog layer is really slow. That's why we defer it to
    //        PrintQuality. Either cache on a pixmap - or maybe
    //        better: Add to GlobeScanlineTextureMapper.
    if ( painter->mapQuality() != PrintQuality )
        return true;

    if ( viewParams->projection() != Spherical)
        return true;

    // No use to draw the fog if it's not visible in the area.
    if ( viewParams->mapCoversViewport() )
        return true;

    int imgWidth2  = viewParams->width() / 2;
    int imgHeight2 = viewParams->height() / 2;

    int radius = viewParams->radius();

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imgWidth2, imgHeight2 ), radius );

    // FIXME: Add a cosine relationship
    grad1.setColorAt( 0.85, QColor( 255, 255, 255, 0 ) );
    grad1.setColorAt( 1.00, QColor( 255, 255, 255, 64 ) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );

    painter->save();

    painter->setBrush( brush1 );
    painter->setPen( pen1 );
    painter->setRenderHint( QPainter::Antialiasing, false );

    // FIXME: Cut out what's really needed
    painter->drawEllipse( imgWidth2  - radius,
                         imgHeight2 - radius,
                         2 * radius,
                         2 * radius );

    painter->restore();

    return true;
}

RenderState FogLayer::renderState() const
{
    return RenderState(QStringLiteral("Fog"));
}

}
