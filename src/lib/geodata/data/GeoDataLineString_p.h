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
    GeoDataLineStringPrivate( TessellationFlags f )
         : m_dirtyRange( true ),
           m_dirtyBox( true ),
           m_tessellationFlags( f )
    {
    }

    GeoDataLineStringPrivate()
         : m_dirtyRange( true ),
           m_dirtyBox( true )
    {
    }

    ~GeoDataLineStringPrivate()
    {
    qDeleteAll(m_rangeCorrected);
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

    void toPoleCorrected( const GeoDataLineString & q, GeoDataLineString & poleCorrected );

    void toDateLineCorrected( const GeoDataLineString & q,
                              QVector<GeoDataLineString*> & lineStrings );

    void interpolateDateLine( const GeoDataCoordinates & previousCoords,
                              const GeoDataCoordinates & currentCoords,
                              GeoDataCoordinates & previousAtDateline,
                              GeoDataCoordinates & currentAtDateline,
                              TessellationFlags f );

    GeoDataCoordinates findDateLine( const GeoDataCoordinates & previousCoords,
                       const GeoDataCoordinates & currentCoords,
                       int recursionCounter );

    QVector<GeoDataCoordinates> m_vector;

    QVector<GeoDataLineString*>  m_rangeCorrected;
    bool                        m_dirtyRange;

    GeoDataLatLonAltBox         m_latLonAltBox;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif
