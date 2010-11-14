//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#ifndef MARBLE_GEODATAPOLYGON_H
#define MARBLE_GEODATAPOLYGON_H

#include <QtCore/QVector>

#include "global.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

class GeoDataPolygonPrivate;

/*!
    \class GeoDataPolygon
    \brief A polygon that can have "holes".

    GeoDataPolygon is a tool class that implements the Polygon tag/class
    of the Open Geospatial Consortium standard KML 2.2.

    GeoDataPolygon extends GeoDataGeometry to store and edit
    Polygons.

    In the QPainter API "pure" Polygons would represent polygons with
    "holes" inside. However QPolygon doesn't provide this feature directly.

    Whenever a Polygon is painted GeoDataLineStyle should be used to assign a
    color and line width.

    The polygon consists of
    \li a single outer boundary and
    \li optionally a set of inner boundaries.

    All boundaries are LinearRings.

    The boundaries of a GeoDataPolygon consist of several (geodetic) nodes which
    are each connected through line segments. The nodes are stored as
    GeoDataCoordinates objects.


    The API which provides access to the nodes is similar to the API of
    QVector.

    GeoDataPolygon allows Polygons to be tessellated in order to make them
    follow the terrain and the curvature of the earth. The tessellation options
    allow for different ways of visualization:

    \li Not tessellated: A Polygon that connects each two nodes directly and
        straight in screen coordinate space.
    \li A tessellated line: Each line segment is bent so that the Polygon
        follows the curvature of the earth and its terrain. A tessellated
        line segment connects two nodes at the shortest possible distance
        ("along great circles").
    \li A tessellated line that follows latitude circles whenever possible:
        In this case Latitude circles are followed as soon as two subsequent
        nodes have exactly the same amount of latitude. In all other places the
        line segments follow great circles.

    Some convenience methods have been added that allow to calculate the
    geodesic bounding box or the length of a Polygon.

    \see GeoDataLinearRing
*/

class GEODATA_EXPORT GeoDataPolygon : public GeoDataGeometry
{
 public:
/*!
    \brief Creates a new Polygon.
*/
    explicit GeoDataPolygon( TessellationFlags f = Tessellate );


/*!
    \brief Creates a Polygon from an existing geometry object.
*/
    GeoDataPolygon( const GeoDataGeometry &other );

    
/*!
    \brief Destroys a Polygon.
*/
    virtual ~GeoDataPolygon();


    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;


/*!
    \brief Returns whether a Polygon is a closed polygon.

    \return <code>true</code> for a Polygon.
*/    
    virtual bool isClosed() const;


/*!
    \brief Returns whether the Polygon follows the earth's surface.

    \return <code>true</code> if the Polygon's line segments follow the
    earth's surface and terrain along great circles.
*/    
    bool tessellate() const;


/*!
    \brief Sets the tessellation property for the Polygon.

    If \a tessellate is <code>true</code> then the Polygon's line segments
    are bent and follow the earth's surface and terrain along great circles.
    If \a tessellate is <code>false</code> then the Polygon's line segments
    are rendered as straight lines in screen coordinate space.
*/
    void setTessellate( bool tessellate );

    
/*!
    \brief Returns the tessellation flags for a Polygon.
*/
    TessellationFlags tessellationFlags() const;


/*!
    \brief Sets the given tessellation flags for a Polygon.
*/
    void setTessellationFlags( TessellationFlags f );


/*!
    \brief Returns the smallest latLonAltBox that contains the Polygon.

    \see GeoDataLatLonAltBox
*/
    virtual GeoDataLatLonAltBox latLonAltBox() const;

/*!
    \brief Returns the outer boundary that is represented as a LinearRing.

    \see GeoDataLinearRing
*/
    GeoDataLinearRing& outerBoundary() const;

/*!
    \brief Sets the given LinearRing as an outer boundary of the Polygon.

    \see GeoDataLinearRing
*/
    void setOuterBoundary( const GeoDataLinearRing& boundary );

/*!
    \brief Returns a set of inner boundaries which are represented as LinearRings.

    \see GeoDataLinearRing
*/
    QVector<GeoDataLinearRing>& innerBoundaries() const;


/*!
    \brief Appends a given LinearRing as an inner boundary of the Polygon.

    \see GeoDataLinearRing
*/
    void appendInnerBoundary( const GeoDataLinearRing& boundary );

/*!
    \brief Returns whether the given coordinates lie within the polygon.

    \return <code>true</code> if the coordinates lie within the polygon
    (and not in its holes), false otherwise.
*/
    virtual bool contains( const GeoDataCoordinates &coordinates ) const;

    // Serialization
/*!
    \brief Serialize the Polygon to a stream.
    \param stream the stream.
*/
    virtual void pack( QDataStream& stream ) const;

    
/*!
    \brief Unserialize the Polygon from a stream.
    \param stream the stream.
*/
    virtual void unpack( QDataStream& stream );

    
 protected:
    GeoDataPolygonPrivate *p() const;
};

class GEODATA_EXPORT GeoDataOuterBoundary : public GeoDataPolygon
{
};

class GEODATA_EXPORT GeoDataInnerBoundary : public GeoDataPolygon
{
};

}

#endif // GEODATAPOLYGON_H
