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
           m_dirtyDetail( true ),
           m_tessellationFlags( f )
    {
    }

    GeoDataLineStringPrivate()
         : m_dirtyRange( true ),
           m_dirtyBox( true ),
           m_dirtyDetail( true )
    {
    }

    ~GeoDataLineStringPrivate()
    {
        qDeleteAll(m_rangeCorrected);
    }

    void operator=( const GeoDataLineStringPrivate &other)
    {
        GeoDataGeometryPrivate::operator=( other );
        m_vector = other.m_vector;
<<<<<<< HEAD
=======
		m_vectorFiltered = other.m_vectorFiltered;
        m_vectorDetailLevels = other.m_vectorDetailLevels;
<<<<<<< HEAD
>>>>>>> 9054e85... Current state of linestring filtering
=======
>>>>>>> 9054e85... Current state of linestring filtering
        qDeleteAll( m_rangeCorrected );
        foreach( GeoDataLineString *lineString, other.m_rangeCorrected )
        {
            m_rangeCorrected.append( new GeoDataLineString( *lineString ) );
        }
        m_dirtyRange = other.m_dirtyRange;
        m_latLonAltBox = other.m_latLonAltBox;
        m_dirtyBox = other.m_dirtyBox;
        m_dirtyDetail = other.m_dirtyDetail;
        m_tessellationFlags = other.m_tessellationFlags;
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

<<<<<<< HEAD
<<<<<<< HEAD
=======
	mutable QVector<GeoDataCoordinates> m_vectorFiltered;   // Keeps just some of the nodes in the linestring, 
												            // since not all of them are needed for a lower zoom
												            // level, for example. Saves performance. 
=======
	mutable QVector<GeoDataCoordinates> m_vectorFiltered;   // Keeps just some of the nodes in the linestring, 
												            // since not all of them are needed for a lower zoom
												            // level, for example. Saves performance. 

    QVector<int>                m_vectorDetailLevels;   // Keeps the detail levels for each node of the current linestring.
                                                        // A high detail level means that the node is going to be used on 
                                                        // more lower zoom levels (it's a more important point of the linestring)
>>>>>>> 9054e85... Current state of linestring filtering

    QVector<int>                m_vectorDetailLevels;   // Keeps the detail levels for each node of the current linestring.
                                                        // A high detail level means that the node is going to be used on 
                                                        // more lower zoom levels (it's a more important point of the linestring)

>>>>>>> 9054e85... Current state of linestring filtering
    QVector<GeoDataLineString*>  m_rangeCorrected;
    bool                        m_dirtyRange;

    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 

    bool                        m_dirtyDetail;  // same as m_dirtyBox, but for the filtering 
                                                // of the linestring

    TessellationFlags           m_tessellationFlags;
};

} // namespace Marble

#endif
