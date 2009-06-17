//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TextAnnotation.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtCore/QDebug>

namespace Marble{

TextAnnotation::TextAnnotation()
{
}

QRect TextAnnotation::screenBounding()
{
    //FIXME the first 2 should actually offset it
    return QRect(-5,-5,20,20);
}

GeoDataLatLonBox TextAnnotation::geoBounding(qreal angularResolution)
{
    //TALK possible for the geoBounding to be automatic?
    qreal lat, lon, alt, width, height;
    coordinate(lon, lat, alt);

    //problem with this approach is that the geoBounding is View
    //Dependant. This does not lend well to a good consistant model.
    width = screenBounding().width() * angularResolution;
    height = screenBounding().height() * angularResolution;
    m_geoBoundCache =  GeoDataLatLonBox(coordinate().latitude() - (height/2),
                            coordinate().latitude() + (height/2),
                            coordinate().longitude() + (width/2),
                            coordinate().longitude() - (width/2));

    return m_geoBoundCache;
}

void TextAnnotation::paint( GeoPainter *painter,
                            ViewportParams *viewport,
                            const QString& renderPos,
                            GeoSceneLayer * layer )
{
    qreal degPix = viewport->angularResolution() * RAD2DEG;

    painter->drawEllipse(coordinate(), screenBounding().width(),
                         screenBounding().height(), false);
    //Would it not be usefull to have a draw latlongbox?
//    painter->drawRect(geoBounding());
    qreal north, south, east, west;
    geoBounding(viewport->angularResolution()).boundaries(north, south, east, west);
    painter->drawRect(GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ),
                      geoBounding(viewport->angularResolution()).width(GeoDataCoordinates::Degree),
                      geoBounding(viewport->angularResolution()).height(GeoDataCoordinates::Degree),true);
    painter->drawPoint( GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ) );
    qDebug() << geoBounding(viewport->angularResolution()).width();

}

}
