//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLinearRing.h"
#include "GeoDataLinearRing_p.h"

#include "MarbleMath.h"

#include <QtCore/QDebug>

namespace Marble
{

GeoDataLinearRing::GeoDataLinearRing( TessellationFlags f )
    : GeoDataLineString( new GeoDataLinearRingPrivate( f ) )
{
}

GeoDataLinearRing::GeoDataLinearRing( const GeoDataGeometry & other )
  : GeoDataLineString( other )
{
}

GeoDataLinearRing::~GeoDataLinearRing()
{
}

bool GeoDataLinearRing::isClosed() const
{
    return true;
}

qreal GeoDataLinearRing::length( qreal planetRadius ) const
{
    qreal  length = GeoDataLineString::length( planetRadius );

    return length + planetRadius * distanceSphere( last(), first() );
}

QVector<GeoDataLineString*> GeoDataLinearRing::toRangeCorrected() const
{
    if ( p()->m_dirtyRange ) {

        qDeleteAll( p()->m_rangeCorrected );

        GeoDataLinearRing poleCorrected = toPoleCorrected();

        p()->m_rangeCorrected = poleCorrected.toDateLineCorrected();
    }

    return p()->m_rangeCorrected;
}

}

