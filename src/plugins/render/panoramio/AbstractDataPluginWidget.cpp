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

namespace Marble {

class AbstractDataPluginWidgetPrivate {
 public:
    AbstractDataPluginWidgetPrivate()
        : m_addedAngularResolution( 0 )
    {
    };
    
    QString m_id;
    GeoDataCoordinates m_coordinates;
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

QString AbstractDataPluginWidget::id() {
    return d->m_id;
}

void AbstractDataPluginWidget::setId( QString id ) {
    d->m_id = id;
}

qreal AbstractDataPluginWidget::addedAngularResolution() {
    return d->m_addedAngularResolution;
}

void AbstractDataPluginWidget::setAddedAngularResolution( qreal resolution ) {
    d->m_addedAngularResolution = resolution;
}

} // Marble namespace

#include "AbstractDataPluginWidget.moc"
