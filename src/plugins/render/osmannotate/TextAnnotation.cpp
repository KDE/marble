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
#include "MarbleDirs.h"

#include <QtCore/QDebug>
#include <QtGui/QPixmap>

namespace Marble{

TextAnnotation::TextAnnotation()
{
}

QRect TextAnnotation::screenBounding()
{
    //FIXME the first 2 should actually offset it
    return QRect(-5,-5,20,20);
}

void TextAnnotation::geoBounding(qreal angularResolution)
{
    //FIXME
    // Remove this method when the QRegion is actually done by the painter
    // it will be preduced in the GeoGraphicsItemPrivate during a paint event


    qreal lat, lon, alt, width, height;
    coordinate(lon, lat, alt);

    //problem with this approach is that the geoBounding is View
    //Dependant. This does not lend well to a good consistant model.
    width = screenBounding().width() * angularResolution;
    height = screenBounding().height() * angularResolution;
    QRectF bound( coordinate().longitude() - (width/2),
                 coordinate().latitude() - (height/2),
                 width, height) ;
    m_regions.clear();
    QPainterPath path;
    path.addRect(bound);
    m_regions.append(path);

}

void TextAnnotation::paint( GeoPainter *painter,
                            ViewportParams *viewport,
                            const QString& renderPos,
                            GeoSceneLayer * layer )
{
    qreal degPix = viewport->angularResolution() * RAD2DEG;

    painter->drawEllipse(coordinate(), screenBounding().width(), screenBounding().height(), isGeoProjected());
    //Would it not be usefull to have a draw latlongbox?
//    painter->drawRect(geoBounding());
    qreal north, south, east, west;

    //don't need this if its done by the painter soon
    geoBounding(viewport->angularResolution());
    north = m_regions.at(0).boundingRect().top();
    south = m_regions.at(0).boundingRect().bottom();
    east = m_regions.at(0).boundingRect().right();
    west = m_regions.at(0).boundingRect().left();

    //would like a method to draw a QRegion ;)
    painter->drawRect(GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ),
                      m_regions.at(0).boundingRect().width() * RAD2DEG,
                      m_regions.at(0).boundingRect().height() * RAD2DEG,true);

    painter->drawPoint( GeoDataCoordinates((west + east) /2 , (north + south) / 2, 0 ) );

    painter->drawPixmap( coordinate(), QPixmap( MarbleDirs::path( "bitmaps/annotation.png" ) )  );


}

bool TextAnnotation::isGeoProjected()
{
    return false;
}

}
