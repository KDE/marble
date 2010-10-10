//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//


#ifndef MARBLE_GEODATALINEARRING_H
#define MARBLE_GEODATALINEARRING_H


#include "geodata_export.h"
#include "GeoDataLineString.h"


namespace Marble
{

/*!
    \class GeoDataLinearRing
    \brief A LinearRing that allows to store a closed, contiguous set of line segments.

    GeoDataLinearRing is a tool class that implements the LinearRing tag/class
    of the Open Geospatial Consortium standard KML 2.2.

    Unlike suggested in the KML spec GeoDataLinearRing extends GeoDataLineString
    to store a closed LineString (the KML specification suggests to inherit from
    the Geometry class directly).

    In the QPainter API LinearRings are also referred to as "polygons".
    As such they are similar to QPolygons.

    Whenever a LinearRing is painted GeoDataLineStyle should be used to assign a
    color and line width.

    A GeoDataLinearRing consists of several (geodetic) nodes which are each
    connected through line segments. The nodes are stored as GeoDataCoordinates
    objects.

    The API which provides access to the nodes is similar to the API of
    QVector.

    GeoDataLinearRing allows LinearRings to be tessellated in order to make them
    follow the terrain and the curvature of the earth. The tessellation options
    allow for different ways of visualization:

    \li Not tessellated: A LinearRing that connects each two nodes directly and
        straight in screen coordinate space.
    \li A tessellated line: Each line segment is bent so that the LinearRing
        follows the curvature of the earth and its terrain. A tessellated
        line segment connects two nodes at the shortest possible distance
        ("along great circles").
    \li A tessellated line that follows latitude circles whenever possible:
        In this case Latitude circles are followed as soon as two subsequent
        nodes have exactly the same amount of latitude. In all other places the
        line segments follow great circles.

    Some convenience methods have been added that allow to calculate the
    geodesic bounding box or the length of a LinearRing.
*/

class GeoDataLinearRingPrivate;

class GEODATA_EXPORT GeoDataLinearRing : public GeoDataLineString
{

 public:
/*!
    \brief Creates a new LinearRing.
*/
    explicit GeoDataLinearRing( TessellationFlags f = NoTessellation);


/*!
    \brief Creates a LinearRing from an existing geometry object.
*/
    GeoDataLinearRing( const GeoDataGeometry &other );

    
/*!
    \brief Destroys a LinearRing.
*/
    virtual ~GeoDataLinearRing();

    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;
    
/*!
    \brief Returns whether a LinearRing is a closed polygon.

    \return <code>true</code> for a LinearRing.
*/
    virtual bool isClosed() const;

    
/*!
    \brief Returns the length of the LinearRing across a sphere.

    As a parameter the \a planetRadius needs to be passed.

    \return The return value is the length of the LinearRing.
    The unit used for the resulting length matches the unit of the planet
    radius.

    This method can be used as an approximation for the circumference of a
    LinearRing.
*/
    virtual qreal length( qreal planetRadius, int offset = 0 ) const;

    
/*!
    \brief Provides a more generic representation of the LinearRing.

    The LinearRing is normalized, pole corrected and dateline corrected.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual QVector<GeoDataLineString*> toRangeCorrected() const;

/*!
    \brief Returns whether the given coordinates lie within the polygon.

    \return <code>true</code> if the coordinates lie within the polygon, false otherwise.
*/
    virtual bool contains( const GeoDataCoordinates &coordinates ) const;
};

}

#endif
