//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Placemark.h"

namespace Marble
{
namespace Declarative
{

Placemark::Placemark( const QString &name, Coordinate *coordinate, QObject *parent ) :
    QObject( parent ), m_name( name )
{
    m_coordinate.setCoordinates( coordinate->coordinates() );
}

QString Placemark::name() const
{
    return m_name;
}

void Placemark::setName( const QString &name )
{
    m_name = name;
    emit nameChanged();
}

Coordinate *Placemark::coordinate()
{
    return &m_coordinate;
}

void Placemark::setCoordinate( Coordinate *coordinate )
{
    m_coordinate.setCoordinates( coordinate->coordinates() );
    emit coordinateChanged();
}

}
}

#include "Placemark.moc"
