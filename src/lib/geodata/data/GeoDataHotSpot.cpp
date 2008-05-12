//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataHotSpot.h"

class GeoDataHotSpotPrivate
{
  public:
    GeoDataHotSpotPrivate()
    {
    }

    ~GeoDataHotSpotPrivate()
    {
    }

    QPointF m_hotSpot;
    GeoDataHotSpot::Units m_xunits;
    GeoDataHotSpot::Units m_yunits;
};

GeoDataHotSpot::GeoDataHotSpot( const QPointF& hotSpot, Units xunits, Units yunits ) :
    d( new GeoDataHotSpotPrivate() )
{
    setHotSpot( hotSpot, xunits, yunits );
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
