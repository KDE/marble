//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLineString.h"


class GeoDataLineStringPrivate
{
 public:
    GeoDataLineStringPrivate()
        : m_extrude( false ),
          m_tesselate( false ),
          m_altitudeMode( ClampToGround ),
          m_dirtyBox( true )
    {
    }

    ~GeoDataLineStringPrivate()
    {
    }

    bool         m_extrude;
    bool         m_tesselate;
    AltitudeMode m_altitudeMode;

    bool         m_dirtyBox; // tells whether there have been changes to the
                             // GeoDataPoints since the LatLonAltBox has 
                             // been calculated. Saves performance. 
};

GeoDataLineString::GeoDataLineString()
    : d( new GeoDataLineStringPrivate() )
{
}

GeoDataLineString::~GeoDataLineString()
{
    delete d;
}

bool GeoDataLineString::extrude() const
{
    return d->m_extrude;
}

void GeoDataLineString::setExtrude( bool extrude )
{
    d->m_extrude = extrude;
}

bool GeoDataLineString::tesselate() const
{
    return d->m_tesselate;
}

GeoDataLatLonAltBox GeoDataLineString::latLonAltBox() const
{
    if (d->m_dirtyBox) {
    // calulate LatLonAltBox
    }
    d->m_dirtyBox = false;

    return GeoDataLatLonAltBox();
}


void GeoDataLineString::setTesselate( bool tesselate )
{
    d->m_tesselate = tesselate;
}

AltitudeMode GeoDataLineString::altitudeMode() const
{
    return d->m_altitudeMode;
}

void GeoDataLineString::setAltitudeMode( const AltitudeMode altitudeMode )
{
    d->m_altitudeMode = altitudeMode;
}

void GeoDataLineString::append ( const GeoDataPoint & value )
{
    d->m_dirtyBox = true;
    QVector<GeoDataPoint>::append( value );
}

void GeoDataLineString::clear()
{
    d->m_dirtyBox = true;
    QVector<GeoDataPoint>::clear();
}

QVector<GeoDataPoint>::Iterator GeoDataLineString::erase ( QVector<GeoDataPoint>::Iterator pos )
{
    d->m_dirtyBox = true;
    QVector<GeoDataPoint>::erase( pos );
}

QVector<GeoDataPoint>::Iterator GeoDataLineString::erase ( QVector<GeoDataPoint>::Iterator begin, 
                                                           QVector<GeoDataPoint>::Iterator end )
{
    d->m_dirtyBox = true;
    QVector<GeoDataPoint>::erase( begin, end );
}

