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


namespace Marble
{

GeoDataLinearRing::GeoDataLinearRing( TessellationFlags f )
    : GeoDataLineString( new GeoDataLinearRingPrivate( f ) )
{
}

GeoDataLinearRing::~GeoDataLinearRing()
{
}

bool GeoDataLinearRing::isClosed() const
{
    return true;
}

}

