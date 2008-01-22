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
    init();
}

BoundingBox::BoundingBox(const QVector<QPointF> &vector )
{
    init();
    
    QVector<QPointF>::const_iterator  it;
    for ( it = vector.constBegin(); it < vector.constEnd() ; ++it ) {
        if ( it->x() > m_topX )
            m_topX = it->x();
        if ( it->x() < m_bottomX )
            m_bottomX = it->x();
        if ( it->y() > m_topY )
            m_topY = it->y();
        if ( it->y() < m_bottomY )
            m_bottomY = it->y();
    }
}


void BoundingBox::init()
{
    m_topX    = -180.0;
    m_bottomX = +180.0;
    m_topY    = -90.0;
    m_bottomY = +90.0;
}


bool BoundingBox::contains( const QPointF &point )
{
    if ( point.x() < m_bottomX || point.x() > m_topX )
        return false;
    
    if ( point.y() < m_bottomY || point.y() > m_topY )
        return false;

    return true;
}


bool BoundingBox::intersects( const BoundingBox &box ) 
{
    // Case 1: east border of box intersects:
    if ( m_bottomX < box.m_topX && box.m_topX < m_topX )
        return true;

    // Case 2: west border of box intersects:
    if ( m_bottomX < box.m_bottomX && box.m_bottomX < m_topX )
        return true;

    // Case 3: north border of box intersects:
    if ( m_bottomY < box.m_topY && box.m_topY < m_topY )
        return true;

    // Case 4: south border of box intersects:
    if ( m_bottomY < box.m_bottomY && box.m_bottomY < m_topY )
        return true;

    // Case 5: box totally contains this one, i.e. all box's borders
    //         are outside, but they still intersect..
    if ( m_bottomX > box.m_bottomX && m_topX < box.m_topX
         &&  m_bottomY > box.m_bottomY && m_topY < box.m_topY )
        return true;

    return false;
}

bool BoundingBox::isValid()
{
    if ( m_topX    == -180.0) return false;
    if ( m_bottomX == +180.0) return false;
    if ( m_topY    == -90.0)  return false;
    if ( m_bottomY == +90.0)  return false;

    return true;
}

QString BoundingBox::string() 
{
    QString temp;
    
    // FIXME: Find a better bounding box representation.
    temp += QString( "TopY: %1; TopX: %2; BotY: %3; BotX: %4 " )
            .arg( m_topY ).arg( m_topX ).arg( m_bottomY ).arg( m_bottomX ); 

    return temp;
}
