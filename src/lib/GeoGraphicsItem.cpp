//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "GeoGraphicsItem.h"
#include "GeoGraphicsItem_p.h"

//Marble
#include "GeoDataCoordinates.h"

using namespace Marble;

GeoGraphicsItem::GeoGraphicsItem()
    : MarbleGraphicsItem( new GeoGraphicsItemPrivate )
{
}

GeoGraphicsItem::GeoGraphicsItem( GeoGraphicsItemPrivate *d_ptr )
    : MarbleGraphicsItem( d_ptr )
{
}

GeoGraphicsItem::~GeoGraphicsItem() {
}

GeoDataCoordinates GeoGraphicsItem::coordinate() const {
    return p()->m_coordinate;
}

void GeoGraphicsItem::coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const {
    longitude = p()->m_coordinate.longitude();
    latitude = p()->m_coordinate.latitude();
    altitude = p()->m_coordinate.altitude();
}

void GeoGraphicsItem::setCoordinate( qreal longitude, qreal latitude, qreal altitude ) {
    p()->m_coordinate.setLongitude( longitude );
    p()->m_coordinate.setLatitude( latitude );
    p()->m_coordinate.setAltitude( altitude );
}

void GeoGraphicsItem::setCoordinate( const GeoDataCoordinates &point ) {
    p()->m_coordinate = point;
}

QString GeoGraphicsItem::target() {
    return p()->m_target;
}

void GeoGraphicsItem::setTarget( const QString& target ) {
    p()->m_target = target;
}

QList<QPoint> GeoGraphicsItem::positions() {
    return p()->positions();
}

GeoGraphicsItemPrivate *GeoGraphicsItem::p() const {
    return reinterpret_cast<GeoGraphicsItemPrivate *>( d );
}
