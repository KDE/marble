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
#include "MarbleDebug.h"

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

QString GeoDataLinearRing::nodeType() const
{
    return p()->nodeType();
}

bool GeoDataLinearRing::isClosed() const
{
    return true;
}

qreal GeoDataLinearRing::length( qreal planetRadius, int offset ) const
{
    qreal  length = GeoDataLineString::length( planetRadius, offset );

    return length + planetRadius * distanceSphere( last(), first() );
}

QVector<GeoDataLineString*> GeoDataLinearRing::toRangeCorrected() const
{
    if ( p()->m_dirtyRange ) {

        qDeleteAll( p()->m_rangeCorrected ); // This shouldn't be needed

        GeoDataLinearRing poleCorrected;

        if ( latLonAltBox().crossesDateLine() )
        {
            GeoDataLinearRing normalizedLineString = toNormalized();
            poleCorrected = normalizedLineString.toPoleCorrected();
            p()->m_rangeCorrected = poleCorrected.toDateLineCorrected();
        }
        else {
            poleCorrected = toPoleCorrected();
        }

        p()->m_rangeCorrected = poleCorrected.toDateLineCorrected();
    }

    return p()->m_rangeCorrected;
}

}

