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

namespace Marble
{

class GeoDataStyleSelectorPrivate
{
  public:
    GeoDataStyleSelectorPrivate()
    {
    }

    GeoDataStyleSelectorPrivate( const GeoDataStyleSelectorPrivate& other )
    {
        m_styleId = other.m_styleId;
    }

    ~GeoDataStyleSelectorPrivate()
    {
    }

    /// The style id.
    QString  m_styleId;
};


GeoDataStyleSelector::GeoDataStyleSelector()
    : d( new GeoDataStyleSelectorPrivate )
{
}

GeoDataStyleSelector::GeoDataStyleSelector( const GeoDataStyleSelector& other )
    : GeoDataObject( other ), d( new GeoDataStyleSelectorPrivate( *other.d ) )
{
}

GeoDataStyleSelector::~GeoDataStyleSelector()
{
    delete d;
}

void GeoDataStyleSelector::setStyleId( const QString &value )
{
    d->m_styleId = value;
}

QString GeoDataStyleSelector::styleId() const
{
    return d->m_styleId;
}

void GeoDataStyleSelector::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_styleId;
}

void GeoDataStyleSelector::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_styleId;
}

}
