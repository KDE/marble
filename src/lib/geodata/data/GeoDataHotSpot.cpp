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

GeoDataHotSpot::GeoDataHotSpot( const QPointF& hotSpot, Units xunits, Units yunits )
  : m_hotSpot( hotSpot ),
    m_xunits( xunits ),
    m_yunits( yunits )
{
}

const QPointF& GeoDataHotSpot::hotSpot( Units &xunits, Units &yunits ) const
{
    xunits = m_xunits;
    yunits = m_yunits;

    return m_hotSpot;
}


void GeoDataHotSpot::setHotSpot( const QPointF& hotSpot, Units xunits, Units yunits )
{
    m_hotSpot = hotSpot;
    m_xunits = xunits;
    m_yunits = yunits;
}
