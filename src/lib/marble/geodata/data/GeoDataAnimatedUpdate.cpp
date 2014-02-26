//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataUpdate.h"
#include "GeoDataTypes.h"
#include "GeoDataAbstractView.h"
#include "GeoDataUpdate.h"

namespace Marble {

class GeoDataAnimatedUpdatePrivate
{
public:
    double m_duration;
    GeoDataUpdate m_update;
    GeoDataAnimatedUpdatePrivate();
};

GeoDataAnimatedUpdatePrivate::GeoDataAnimatedUpdatePrivate() :
    m_duration( 0.0 ), m_update( )
{

}

GeoDataAnimatedUpdate::GeoDataAnimatedUpdate() : d( new GeoDataAnimatedUpdatePrivate )
{

}

GeoDataAnimatedUpdate::GeoDataAnimatedUpdate( const Marble::GeoDataAnimatedUpdate &other ) :
    GeoDataTourPrimitive( other ), d( new GeoDataAnimatedUpdatePrivate( *other.d ) )
{

}

GeoDataAnimatedUpdate &GeoDataAnimatedUpdate::operator=( const GeoDataAnimatedUpdate &other )
{
    GeoDataTourPrimitive::operator=( other );
    *d = *other.d;
    return *this;
}

GeoDataAnimatedUpdate::~GeoDataAnimatedUpdate()
{
    delete d;
}

const char *GeoDataAnimatedUpdate::nodeType() const
{
    return GeoDataTypes::GeoDataAnimatedUpdateType;
}

const GeoDataUpdate &GeoDataAnimatedUpdate::update() const
{
    return d->m_update;
}

GeoDataUpdate &GeoDataAnimatedUpdate::update()
{
    return d->m_update;
}

void GeoDataAnimatedUpdate::setUpdate( GeoDataUpdate &update )
{
    d->m_update = update;
}

double GeoDataAnimatedUpdate::duration() const
{
    return d->m_duration;
}

void GeoDataAnimatedUpdate::setDuration( double duration )
{
    d->m_duration = duration;
}
}