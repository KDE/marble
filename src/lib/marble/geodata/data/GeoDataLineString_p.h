//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATALINESTRINGPRIVATE_H
#define MARBLE_GEODATALINESTRINGPRIVATE_H

#include "GeoDataGeometry_p.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataLineStringPrivate : public GeoDataGeometryPrivate
{
  public:
    explicit GeoDataLineStringPrivate( TessellationFlags f )
        :  m_rangeCorrected( 0 ),
           m_dirtyRange( true ),
           m_dirtyBox( true ),
           m_tessellationFlags( f ),
           m_previousResolution( -1 ),
           m_level( -1 )
    {
    }

    GeoDataLineStringPrivate()
         : m_rangeCorrected( 0 ),
           m_dirtyRange( true ),
           m_dirtyBox( true )
    {
    }

    ~GeoDataLineStringPrivate()
    {
        delete m_rangeCorrected;
    }

    GeoDataLineStringPrivate& operator=( const GeoDataLineStringPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );
        m_vector = other.m_vector;
        m_rangeCorrected = 0;
        m_dirtyRange = true;
        m_dirtyBox = other.m_dirtyBox;
        m_tessellationFlags = other.m_tessellationFlags;
        return *this;
    }


    virtual GeoDataGeometryPrivate* copy()
    { 
        GeoDataLineStringPrivate* copy = new GeoDataLineStringPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataLineStringType;
    }

    virtual EnumGeometryId geometryId() const 
    {
        return GeoDataLineStringId;
    }

    void toPoleCorrected( const GeoDataLineString & q, GeoDataLineString & poleCorrected ) const;

    void toDateLineCorrected( const GeoDataLineString & q,
                              QVector<GeoDataLineString*> & lineStrings ) const;

    void interpolateDateLine( const GeoDataCoordinates & previousCoords,
                              const GeoDataCoordinates & currentCoords,
                              GeoDataCoordinates & previousAtDateline,
                              GeoDataCoordinates & currentAtDateline,
                              TessellationFlags f ) const;

    GeoDataCoordinates findDateLine( const GeoDataCoordinates & previousCoords,
                       const GeoDataCoordinates & currentCoords,
                       int recursionCounter ) const;

    quint8 levelForResolution(qreal resolution) const;
    qreal resolutionForLevel(int level) const;
    void optimize(GeoDataLineString& lineString) const;

    QVector<GeoDataCoordinates> m_vector;

    mutable GeoDataLineString*  m_rangeCorrected;
    mutable bool                m_dirtyRange;

    mutable bool                m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
    mutable qreal  m_previousResolution;
    mutable quint8 m_level;

};

} // namespace Marble

#endif
