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
    m_valid = false;
}

BoundingBox::BoundingBox(const QVector<QPointF> &vector )
{
    m_topX = -180;
    m_botX = 180;
    m_topY = -90;
    m_botY = 90;
    
//     qDebug()<<vector;
    
    
    QVector<QPointF>::const_iterator it;
    for ( it = vector.constBegin(); it < vector.constEnd() ; ++it ) {
//         qDebug() << "test then";
//         qDebug()<< m_topX << ( it->x() > m_topX ) <<
// it->x()<<m_topX;
        m_topX = ( it->x() > m_topX ) ? it->x() : m_topX;
//         qDebug() << m_topX;
        m_botX = ( it->x() < m_botX ) ? it->x() : m_botX;
        m_topY = ( it->y() > m_topY ) ? it->y() : m_topY;
        m_botY = ( it->y() < m_botY ) ? it->y() : m_botY;
    }
//     qDebug() << "what" << m_topX;
    
    m_valid = isValid();
}
bool BoundingBox::contains( const QPointF &point )
{
    if ( (point.x() < m_botX) || (point.x() > m_topX ) ) {
        return false;
    }
    
    if ( (point.y() < m_botY ) || (point.y() > m_topY ) ) {
        return false;
    }
    return true;
}

bool BoundingBox::intersects( BoundingBox box ) 
{
    if ( contains( QPointF( box.m_botX, box.m_botY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_botX, box.m_topY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_topX, box.m_botY ) ) ) {
        return true;
    }
    
    if ( contains ( QPointF( box.m_topX, box.m_topY ) ) ) {
        return true;
    }
    return false;
}

bool BoundingBox::isValid()
{
    if (m_topX == -180) return false;
    if (m_botX == 180)return false;
    if (m_topY == -90)return false;
    if (m_botY == 90)return false;
    return true;
}

QString BoundingBox::string() 
{
    QString temp;
    
    temp += "TopY"; temp += QString::number(m_topY); temp += ";";
    temp += "TopX"; temp += QString::number(m_topX); temp += ";";
    temp += "BotY"; temp += QString::number(m_botY); temp += ";";
    temp += "BotX"; temp += QString::number(m_botX); temp += ";";
    
    return temp;
}
