//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//
#include "BoundingBox.h"

#include <QtCore/QPointF>
#include <QDebug>

BoundingBox::BoundingBox()
{
    m_topX = -180.0;
    m_bottomX = +180.0;
    m_topY = -90.0;
    m_bottomY = +90.0;
}

BoundingBox::BoundingBox(const QVector<QPointF> &vector )
{
    m_topX = -180.0;
    m_bottomX = +180.0;
    m_topY = -90.0;
    m_bottomY = +90.0;
    
//     qDebug()<<vector;
    
    
    QVector<QPointF>::const_iterator it;
    for ( it = vector.constBegin(); it < vector.constEnd() ; ++it ) {
//         qDebug() << "test then";
//         qDebug()<< m_topX << ( it->x() > m_topX ) <<
// it->x()<<m_topX;
        m_topX =    ( it->x() > m_topX )    ? it->x() : m_topX;
//         qDebug() << m_topX;
        m_bottomX = ( it->x() < m_bottomX ) ? it->x() : m_bottomX;
        m_topY =    ( it->y() > m_topY )    ? it->y() : m_topY;
        m_bottomY = ( it->y() < m_bottomY ) ? it->y() : m_bottomY;
    }
//     qDebug() << "what" << m_topX;
    
//     m_valid = isValid();
}
bool BoundingBox::contains( const QPointF &point )
{
    if ( ( point.x() < m_bottomX ) || ( point.x() > m_topX ) ) {
        return false;
    }
    
    if ( ( point.y() < m_bottomY ) || ( point.y() > m_topY ) ) {
        return false;
    }
    return true;
}

bool BoundingBox::intersects( BoundingBox box ) 
{
    if ( contains( QPointF( box.m_bottomX, box.m_bottomY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_bottomX, box.m_topY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_topX, box.m_bottomY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_topX, box.m_topY ) ) ) {
        return true;
    }
    return false;
}

bool BoundingBox::isValid()
{
    if (m_topX == -180.0) return false;
    if (m_bottomX == +180.0) return false;
    if (m_topY == -90.0) return false;
    if (m_bottomY == +90.0) return false;
    return true;
}

QString BoundingBox::string() 
{
    QString temp;
    
    temp += QString( "TopY: %1; TopX: %2; BotY: %3; BotX: %4 " )
            .arg( m_topY ).arg( m_topX ).arg( m_bottomY ).arg( m_bottomX ); 

    return temp;
}
