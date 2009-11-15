//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "AbstractLayerData.h"

#include <QtCore/QSize>
#include <QtCore/QPoint>

#include "MarbleDebug.h"
#include "ClipPainter.h"
#include "ViewParams.h"
#include "AbstractProjection.h"

using namespace Marble;

AbstractLayerData::AbstractLayerData( const GeoDataCoordinates &position ):
                    m_visible(true)
{
    m_position = new GeoDataCoordinates;
    *m_position = position;
}

AbstractLayerData::AbstractLayerData( qreal lat, 
                                      qreal lon): m_visible(true)
{
//  FIXME: Add altitude as soon as it becomes relevant  
    m_position = new GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree);
}

AbstractLayerData::~AbstractLayerData()
{
    delete m_position;
}

QString AbstractLayerData::toString()
{
    return m_position->toString();
}

void AbstractLayerData::draw ( ClipPainter *, const QPoint & )
{
    mDebug() <<"in AbstractLayerData::draw() for single point";
}


void AbstractLayerData::draw(ClipPainter *painter, 
                             const QSize &canvasSize,
                             ViewParams *viewParams )
{
    Q_UNUSED( painter );
    Q_UNUSED( canvasSize );
    Q_UNUSED( viewParams );

    //mDebug() <<"in AbstractLayerData::draw() without bounding box";
}

bool AbstractLayerData::visible() const
{
    return m_visible;
}

void AbstractLayerData::setVisible( bool visible )
{
    m_visible = visible;
}

GeoDataCoordinates AbstractLayerData::position() const
{
    return *m_position; 
} 

qreal  AbstractLayerData::lat() const
{
    qreal tmpLat;
    qreal tmpLon;

    m_position->geoCoordinates( tmpLon, tmpLat, GeoDataCoordinates::Degree );

    return tmpLat;
}

qreal  AbstractLayerData::lon() const
{
    qreal tmpLat;
    qreal tmpLon;

    m_position->geoCoordinates( tmpLon, tmpLat, GeoDataCoordinates::Degree );

    return tmpLon;
}

void AbstractLayerData::setPosition( const GeoDataCoordinates &position )
{
    delete m_position;
    m_position = new GeoDataCoordinates(position);
}

void AbstractLayerData::setPosition( const qreal &lat,
                                     const qreal &lon )
{
    //int detail = m_position->detail();
    delete m_position;
//  FIXME: Add altitude as soon as it becomes relevant  
    m_position = new GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree);
}

bool AbstractLayerData::getPixelPos( const QSize &screenSize,
                                      ViewParams *viewParams,
                                      QPoint *point )
{
    QPointF tempPoint;
    bool    tempBool;

    tempBool = getPixelPos( screenSize, viewParams, &tempPoint );
    point->setX( (int)tempPoint.x() );
    point->setY( (int)tempPoint.y() );

    return tempBool;
}

bool AbstractLayerData::getPixelPos( const QSize &screenSize,
                                     ViewParams *viewParams,
                                     QPointF *point )
{
    Q_UNUSED( screenSize );
    bool hidden;
    qreal x, y;

    bool visable = viewParams->currentProjection()->screenCoordinates( *m_position,
                                                                       viewParams->viewport(),
                                                                       x, y, hidden );
    if ( ! hidden  && visable ) {
        point->setX( x );
        point->setY( y );
        return true;
    }

    return false;
}

void AbstractLayerData::printToStream( QTextStream &out ) const
{
    out << "AbstractLayerData";
}
