//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// self
#include "AbstractDataPluginItem.h"

// Marble
#include "AbstractProjection.h"
#include "GeoDataCoordinates.h"
#include "ViewportParams.h"

// Qt
#include <QtGui/QAction>
#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QSize>

namespace Marble {

class AbstractDataPluginItemPrivate {
 public:
    AbstractDataPluginItemPrivate()
        : m_addedAngularResolution( 0 )
    {
    };
    
    QString m_id;
    GeoDataCoordinates m_coordinates;
    QString m_target;
    QRect m_paintPosition;
    qreal m_addedAngularResolution;
};

AbstractDataPluginItem::AbstractDataPluginItem( QObject *parent )
    : QObject( parent ),
      d( new AbstractDataPluginItemPrivate )
{
}

AbstractDataPluginItem::~AbstractDataPluginItem() {
}

GeoDataCoordinates AbstractDataPluginItem::coordinates() {
    return d->m_coordinates;
}

void AbstractDataPluginItem::setCoordinates( const GeoDataCoordinates& coordinates ) {
    d->m_coordinates = coordinates;
}

QString AbstractDataPluginItem::target() {
    return d->m_target;
}

void AbstractDataPluginItem::setTarget( const QString& target ) {
    d->m_target = target;
}

QString AbstractDataPluginItem::id() const {
    return d->m_id;
}

void AbstractDataPluginItem::setId( const QString& id ) {
    d->m_id = id;
}

qreal AbstractDataPluginItem::addedAngularResolution() const {
    return d->m_addedAngularResolution;
}

void AbstractDataPluginItem::setAddedAngularResolution( qreal resolution ) {
    d->m_addedAngularResolution = resolution;
}

bool AbstractDataPluginItem::isItemAt( const QPoint& curpos ) const {
    if( d->m_paintPosition.contains( curpos ) ) {
        return true;
    }
    return false;
}

void AbstractDataPluginItem::updatePaintPosition( ViewportParams *viewport,
                                                  const QSize& size ) {
    GeoDataCoordinates coords = coordinates();
    
    qreal x[100], y;
    int pointRepeatNumber;
    bool globeHidesPoint;
    if( viewport->currentProjection()->screenCoordinates( coords,
                                                          viewport,
                                                          x, y,
                                                          pointRepeatNumber,
                                                          size,
                                                          globeHidesPoint ) )
    {
        // FIXME: We need to handle multiple coords here
        qint32 width = size.width();
        qint32 height = size.height();
        qint32 leftX = x[0] - ( size.width()/2 );
        qint32 topY = y    - ( size.height()/2 );
        
        if( leftX < 0 ) {
            width += leftX;
            leftX = 0;
        }
        if( topY < 0 ) {
            height += topY;
            topY = 0;
        }
            
        d->m_paintPosition.setRect( leftX, topY, width, height );
    }
    else {
        d->m_paintPosition = QRect();
    }
}

QRect AbstractDataPluginItem::paintPosition() {
    return d->m_paintPosition;
}

} // Marble namespace

#include "AbstractDataPluginItem.moc"
