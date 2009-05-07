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
#include "AbstractDataPluginWidget.h"

// Marble
#include "AbstractProjection.h"
#include "GeoDataCoordinates.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QSize>

namespace Marble {

class AbstractDataPluginWidgetPrivate {
 public:
    AbstractDataPluginWidgetPrivate()
        : m_addedAngularResolution( 0 )
    {
    };
    
    QString m_id;
    GeoDataCoordinates m_coordinates;
    QRect m_paintPosition;
    qreal m_addedAngularResolution;
};

AbstractDataPluginWidget::AbstractDataPluginWidget( QObject *parent )
    : QObject( parent ),
      d( new AbstractDataPluginWidgetPrivate )
{
}

AbstractDataPluginWidget::~AbstractDataPluginWidget() {
}

GeoDataCoordinates AbstractDataPluginWidget::coordinates() {
    return d->m_coordinates;
}

void AbstractDataPluginWidget::setCoordinates( GeoDataCoordinates coordinates ) {
    d->m_coordinates = coordinates;
}

QString AbstractDataPluginWidget::id() const {
    return d->m_id;
}

void AbstractDataPluginWidget::setId( QString id ) {
    d->m_id = id;
}

qreal AbstractDataPluginWidget::addedAngularResolution() const {
    return d->m_addedAngularResolution;
}

void AbstractDataPluginWidget::setAddedAngularResolution( qreal resolution ) {
    d->m_addedAngularResolution = resolution;
}

void AbstractDataPluginWidget::updatePaintPosition( ViewportParams *viewport,
                                                    QSize size ) {
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

QRect AbstractDataPluginWidget::paintPosition() {
    return d->m_paintPosition;
}

} // Marble namespace

#include "AbstractDataPluginWidget.moc"
