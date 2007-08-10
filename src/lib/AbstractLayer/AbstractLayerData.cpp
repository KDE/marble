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
#include <QDebug>

#include "ClipPainter.h"
#include "BoundingBox.h"

AbstractLayerData::AbstractLayerData( const GeoPoint &position ):
                    m_visible(true)
{
    m_position = new GeoPoint;
    *m_position = position;
}

AbstractLayerData::AbstractLayerData( double lat, 
                                      double lon): m_visible(true)
{
    m_position = new GeoPoint( lon, lat, GeoPoint::Degree);
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
    qDebug() <<"in AbstractLayerData::draw() for single point";
}


void AbstractLayerData::draw(ClipPainter *painter, 
                             const QSize &canvasSize, double radius,
                             Quaternion invRotAxis)
{
    qDebug() <<"in AbstractLayerData::draw() without bounding box";
}

void AbstractLayerData::draw( ClipPainter *painter, 
                  const QSize &canvasSize, double radius,
                  Quaternion invRotAxis, BoundingBox box )
{
    //does not apply to abstractLayerData
    qDebug() <<"in AbstractLayerData::draw() with bounding box";
}

bool AbstractLayerData::visible() const
{
    return m_visible;
}

void AbstractLayerData::setVisible( bool visible )
{
    m_visible = visible;
}

GeoPoint AbstractLayerData::position() const
{
    return *m_position; 
} 

void AbstractLayerData::setPosition( const GeoPoint &position )
{
    delete m_position;
    m_position = new GeoPoint(position);
}

void AbstractLayerData::setPosition( const double &lat,
                                     const double &lon )
{
    //int detail = m_position->detail();
    delete m_position;
    m_position = new GeoPoint( lon, lat, GeoPoint::Degree);
}

bool AbstractLayerData::getPixelPos( const QSize &screenSize,
                                     Quaternion invRotAxis, 
                                     int radius, QPoint *point)
{
    QPointF tempPoint;
    bool    tempBool;
    
    tempBool = getPixelPos( screenSize, invRotAxis, radius,
                            &tempPoint );
    point -> setX( (int)tempPoint.x() );
    point -> setY( (int)tempPoint.y() );
    
    return tempBool;
}

bool AbstractLayerData::getPixelPos( const QSize &screenSize,
                                     Quaternion invRotAxis, 
                                     int radius, QPointF *point)
{
    Quaternion  qpos = m_position->quaternion(); 
    qpos.rotateAroundAxis( invRotAxis );

    if ( qpos.v[Q_Z] > 0 ){
        point->setX( ( ( screenSize.width() / 2 )
                + ( radius * qpos.v[Q_X] ) ) );
        point->setY( ( ( screenSize.height() / 2 )
                + ( radius * qpos.v[Q_Y] ) ) );
        
        return true;
    } else {
        return false;
    }
}
