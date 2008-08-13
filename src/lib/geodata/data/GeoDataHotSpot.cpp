//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#include "GeoDataHotSpot.h"

#include <QtCore/QDataStream>

namespace Marble
{

class GeoDataHotSpotPrivate
{
  public:
    GeoDataHotSpotPrivate()
    {
    }

    GeoDataHotSpotPrivate( const QPointF& hotSpot, GeoDataHotSpot::Units xunits, GeoDataHotSpot::Units yunits )
        : m_hotSpot( hotSpot ),
          m_xunits( xunits ),
          m_yunits( yunits )
    {
    }

    ~GeoDataHotSpotPrivate()
    {
    }

    QPointF m_hotSpot;
    GeoDataHotSpot::Units m_xunits;
    GeoDataHotSpot::Units m_yunits;
};

GeoDataHotSpot::GeoDataHotSpot( const QPointF& hotSpot, Units xunits, Units yunits ) 
    : d( new GeoDataHotSpotPrivate( hotSpot, xunits, yunits ) )
{
}

GeoDataHotSpot::~GeoDataHotSpot()
{
    delete d;
}

const QPointF& GeoDataHotSpot::hotSpot( Units &xunits, Units &yunits ) const
{
    xunits = d->m_xunits;
    yunits = d->m_yunits;

    return d->m_hotSpot;
}


void GeoDataHotSpot::setHotSpot( const QPointF& hotSpot, Units xunits, Units yunits )
{
    d->m_hotSpot = hotSpot;
    d->m_xunits = xunits;
    d->m_yunits = yunits;
}

void GeoDataHotSpot::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_xunits << d->m_yunits;
    stream << d->m_hotSpot;
}

void GeoDataHotSpot::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );
    int xu, yu;
    stream >> xu >> yu;
    d->m_xunits = static_cast<Units>(xu);
    d->m_yunits = static_cast<Units>(yu);
    stream >> d->m_hotSpot;
}

}
