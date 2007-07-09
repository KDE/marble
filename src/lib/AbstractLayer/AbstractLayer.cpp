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

#include "clippainter.h"


AbstractLayer::AbstractLayer( QObject *parent,
                              AbstractLayerContainer *container) 
    : QObject( parent )
{
    m_layerContainer = container;
    m_visible        = false;
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


QPoint *AbstractLayer::paint( ClipPainter*, const QSize& screenSize,
                           double radius, Quaternion rotAxis, 
                           QPoint *previous, AbstractLayerData *point)

{
    //should be pure virtual
    return 0;
}

void AbstractLayer::paint( ClipPainter*, const QSize& screenSize,
                           double radius, Quaternion rotAxis, 
                           AbstractLayerData *point )
{
    //should be pure virtual
}

void AbstractLayer::paintLayer( ClipPainter*, const QSize& screenSize,
                                double radius, Quaternion rotAxis)
{
    //should be pure virtual
}


#include "AbstractLayer.moc"
