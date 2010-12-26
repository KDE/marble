//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyleSelector.h"
#include <QtCore/QDataStream>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataStyleSelectorPrivate
{
  public:
    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataStyleSelectorType;
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

GeoDataStyleSelector& GeoDataStyleSelector::operator=( const GeoDataStyleSelector& other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

const char* GeoDataStyleSelector::nodeType() const
{
    return d->nodeType();
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
