//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Placemark.h"

Placemark::Placemark(QObject *parent ) :
    QObject( parent )
{
    // nothing to do
}

void Placemark::setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark )
{
    m_placemark = placemark;
    m_coordinate.setCoordinates( placemark.coordinate() );
    emit coordinateChanged();
    emit nameChanged();
}

Coordinate *Placemark::coordinate()
{
    return &m_coordinate;
}

QString Placemark::name() const
{
    return m_placemark.name();
}

#include "Placemark.moc"
