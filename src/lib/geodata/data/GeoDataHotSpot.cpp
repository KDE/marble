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

#include <QtCore/QDataStream>

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
    : d_hot( new GeoDataHotSpotPrivate( hotSpot, xunits, yunits ) )
{
}

GeoDataHotSpot::~GeoDataHotSpot()
{
    delete d_hot;
}

const QPointF& GeoDataHotSpot::hotSpot( Units &xunits, Units &yunits ) const
{
    xunits = d_hot->m_xunits;
    yunits = d_hot->m_yunits;

    return d_hot->m_hotSpot;
}


void GeoDataHotSpot::setHotSpot( const QPointF& hotSpot, Units xunits, Units yunits )
{
    d_hot->m_hotSpot = hotSpot;
    d_hot->m_xunits = xunits;
    d_hot->m_yunits = yunits;
}

void GeoDataHotSpot::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d_hot->m_xunits << d_hot->m_yunits;
    stream << d_hot->m_hotSpot;
}

void GeoDataHotSpot::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );
    int xu, yu;
    stream >> xu >> yu;
    d_hot->m_xunits = static_cast<Units>(xu);
    d_hot->m_yunits = static_cast<Units>(yu);
    stream >> d_hot->m_hotSpot;

}
