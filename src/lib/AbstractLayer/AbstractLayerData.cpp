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
#include <QtCore/QDebug>

#include "ClipPainter.h"
#include "BoundingBox.h"
#include "ViewParams.h"

AbstractLayerData::AbstractLayerData( const GeoDataCoordinates &position ):
                    m_visible(true)
{
    m_position = new GeoDataCoordinates;
    *m_position = position;
}

AbstractLayerData::AbstractLayerData( double lat, 
                                      double lon): m_visible(true)
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
    qDebug() <<"in AbstractLayerData::draw() for single point";
}


void AbstractLayerData::draw(ClipPainter *painter, 
                             const QSize &canvasSize,
                             ViewParams *viewParams )
{
    Q_UNUSED( painter );
    Q_UNUSED( canvasSize );
    Q_UNUSED( viewParams );

    //qDebug() <<"in AbstractLayerData::draw() without bounding box";
}

void AbstractLayerData::draw( ClipPainter *painter, 
                              const QSize &canvasSize,
                              ViewParams *viewParams,
                              const BoundingBox &box )
{
    // Does not apply to abstractLayerData
    Q_UNUSED( painter );
    Q_UNUSED( canvasSize );
    Q_UNUSED( viewParams );
    Q_UNUSED( box );

    //qDebug() <<"in AbstractLayerData::draw() with bounding box";
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

double  AbstractLayerData::lat() const
{
    double tmpLat;
    double tmpLon;

    m_position->geoCoordinates( tmpLon, tmpLat, GeoDataCoordinates::Degree );

    return tmpLat;
}

double  AbstractLayerData::lon() const
{
    double tmpLat;
    double tmpLon;

    m_position->geoCoordinates( tmpLon, tmpLat, GeoDataCoordinates::Degree );

    return tmpLon;
}

void AbstractLayerData::setPosition( const GeoDataCoordinates &position )
{
    delete m_position;
    m_position = new GeoDataCoordinates(position);
}

void AbstractLayerData::setPosition( const double &lat,
                                     const double &lon )
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
    point -> setX( (int)tempPoint.x() );
    point -> setY( (int)tempPoint.y() );

    return tempBool;
}

bool AbstractLayerData::getPixelPos( const QSize &screenSize,
                                     ViewParams *viewParams,
                                     QPointF *point )
{
    int         radius = viewParams->radius();
    Quaternion  qpos   = m_position->quaternion(); 
    switch ( viewParams->projection() ) {
        case Spherical:
            qpos.rotateAroundAxis( viewParams->planetAxis().inverse() );

            if ( qpos.v[Q_Z] > 0 ){
                point->setX( ( ( screenSize.width() / 2 )
                        + ( radius * qpos.v[Q_X] ) ) );
                point->setY( ( ( screenSize.height() / 2 )
                        - ( radius * qpos.v[Q_Y] ) ) );

                return true;
            } else {
                return false;
            }
        break;
        case Equirectangular:
            double lon;
            double lat;
            double xyFactor = 2 * viewParams->radius() / M_PI;

            double centerLon;
            double centerLat;

            // Let (x, y) be the position on the screen of the placemark..
            qpos.getSpherical( lon, lat );
            viewParams->centerCoordinates( centerLon, centerLat );

            int x = (int)( screenSize.width()/ 2 - xyFactor * (centerLon - lon));
            int y = (int)(screenSize.height()/ 2 + xyFactor * (centerLat - lat));

            point->setX( x );
            point->setY( y );

            if ( x < 0 || x >= screenSize.width() ||
                 y < screenSize.height() / 2 - 2*viewParams->radius() || 
                 y >= screenSize.height()/ 2 + 2*viewParams->radius() )
            {
                return false;
            }
    }
    return true;
}

void AbstractLayerData::printToStream( QTextStream &out ) const
{
    out << "AbstractLayerData";
}
