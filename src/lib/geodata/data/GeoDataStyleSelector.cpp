//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyleSelector.h"
#include <QtCore/QDataStream>

GeoDataStyleSelector::GeoDataStyleSelector()
{
}

void GeoDataStyleSelector::setStyleId( const QString &value )
{
    m_styleId = value;
}

QString GeoDataStyleSelector::styleId() const
{
    return m_styleId;
}

void GeoDataStyleSelector::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << m_styleId;
}

void GeoDataStyleSelector::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> m_styleId;
}
