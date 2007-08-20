//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>"
//


#include "AbstractLayer.h"

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtCore/QPoint>

#include "BoundingBox.h"
#include "ClipPainter.h"


AbstractLayer::AbstractLayer( QObject *parent ) 
    : QObject( parent )
{
    m_visible       = false;
    m_containers    = new QVector<AbstractLayerContainer *>();
}

AbstractLayer::~AbstractLayer()
{
    delete m_containers;
}


bool AbstractLayer::getPixelPosFromGeoPoint( double _lon, double _lat,
                                             const QSize &screenSize, 
                                             Quaternion invRotAxis, 
                                             int radius, 
                                             QPoint *point)
{
    Quaternion  qpos( _lon, _lat ); //temp
    qpos.rotateAroundAxis(invRotAxis);

    if ( qpos.v[Q_Z] > 0 ) {
        point->setX( (int)( ( screenSize.width() / 2 ) 
                            + ( radius * qpos.v[Q_X] ) ) );
        point->setY( (int)( ( screenSize.height() / 2 ) 
                            + ( radius * qpos.v[Q_Y] ) ) );
        return true;
    } else {
        return false;
    }
}


bool AbstractLayer::getPixelPosFromGeoPoint( GeoPoint position,
                                             const QSize &screenSize, 
                                             Quaternion invRotAxis,
                                             int radius, 
                                             QPoint *point)
{
    Quaternion  qpos = position.quaternion(); //temp
    qpos.rotateAroundAxis( invRotAxis );

    if ( qpos.v[Q_Z] > 0 ){
        point->setX( (int)( ( screenSize.width() / 2 )
                            + ( radius * qpos.v[Q_X] ) ) );
        point->setY( (int)( ( screenSize.height() / 2 )
                            + ( radius * qpos.v[Q_Y] ) ) );
        
        return true;
    } else {
        return false;
    }
}


bool AbstractLayer::visible() const 
{
    return m_visible;
}

void AbstractLayer::setVisible( bool visible ) 
{
    m_visible = visible;
}

void AbstractLayer::paintLayer( ClipPainter* painter, 
                                const QSize& screenSize,
                                double radius, Quaternion rotAxis)
{
    Quaternion invRotAxis = rotAxis.inverse();
    QVector<AbstractLayerContainer *>::const_iterator it;
    
    for( it = m_containers->begin(); it < m_containers->end(); ++it ){
        if ( (*it) != 0 ) {
            (*it)->draw( painter, screenSize, radius, invRotAxis );
        }
    }
}

void AbstractLayer::paintLayer( ClipPainter* painter, 
                                const QSize& screenSize,
                                double radius, Quaternion rotAxis, 
                                BoundingBox bounding )
{
    Quaternion invRotAxis = rotAxis.inverse();
    QVector<AbstractLayerContainer *>::const_iterator it;
    
    for( it = m_containers->begin(); it < m_containers->end(); ++it ){
        if ( (*it) != 0 ) {
            (*it)->draw( painter, screenSize, radius, invRotAxis, 
                         bounding );
        }
    }
}

double AbstractLayer::distance ( const QPoint &a, const QPoint &b )
{
    return distance( QPointF( a.x(), a.y() ),
                     QPointF( b.x(), b.y() ) );
    
}

double AbstractLayer::distance ( const QPointF &a, const QPointF &b )
{
    return (  ( ( a.x() - b.x() ) * ( a.x() - b.x() ) )
            + ( ( a.y() - b.y() ) * ( a.y() - b.y() ) ) );
}




#include "AbstractLayer.moc"
