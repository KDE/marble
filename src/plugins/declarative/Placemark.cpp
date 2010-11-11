//
// This file is part of the Marble Desktop Globe.
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

Placemark::Placemark( const QString &name, qreal lon, qreal lat, QObject *parent ) :
    QObject( parent ), m_name( name ), m_longitude( lon ), m_latitude( lat )
{
    // nothing to do
}

QString Placemark::name() const
{
    return m_name;
}

void Placemark::setName( const QString &name )
{
    m_name = name;
}

qreal Placemark::longitude() const
{
    return m_longitude;
}

void Placemark::setLongitude( qreal lon )
{
    m_longitude = lon;
}

qreal Placemark::latitude() const
{
    return m_latitude;
}

void Placemark::setLatitude( qreal lat )
{
    m_latitude = lat;
}

}
}

#include "Placemark.moc"
