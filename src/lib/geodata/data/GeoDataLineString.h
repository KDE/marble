//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009  Torsten Rahn <tackat@kde.org>
// Copyright 2009       Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATALINESTRING_H
#define MARBLE_GEODATALINESTRING_H

#include <QtCore/QFlags>
#include <QtCore/QVector>
#include <QtCore/QMetaType>
#include <QtCore/QPair>

#include "MarbleGlobal.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"


namespace Marble
{

class GeoDataLineStringPrivate;

/*!
    \class GeoDataLineString
    \brief A LineString that allows to store a contiguous set of line segments.

    GeoDataLineString is a tool class that implements the LineString tag/class
    of the Open Geospatial Consortium standard KML 2.2.

    GeoDataLineString extends GeoDataGeometry to store and edit
    LineStrings.

    In the QPainter API "pure" LineStrings are also referred to as "polylines".
    As such they are similar to the outline of a non-closed QPolygon.

    Whenever a LineString is painted GeoDataLineStyle should be used to assign a
    color and line width.

    A GeoDataLineString consists of several (geodetic) nodes which are each
    connected through line segments. The nodes are stored as GeoDataCoordinates
    objects.

    The API which provides access to the nodes is similar to the API of
    QVector.

    GeoDataLineString allows LineStrings to be tessellated in order to make them
    follow the terrain and the curvature of the earth. The tessellation options
    allow for different ways of visualization:

    \li Not tessellated: A LineString that connects each two nodes directly and
        straight in screen coordinate space.
    \li A tessellated line: Each line segment is bent so that the LineString
        follows the curvature of the earth and its terrain. A tessellated
        line segment connects two nodes at the shortest possible distance
        ("along great circles").
    \li A tessellated line that follows latitude circles whenever possible:
        In this case Latitude circles are followed as soon as two subsequent
        nodes have exactly the same amount of latitude. In all other places the
        line segments follow great circles.

    Some convenience methods have been added that allow to calculate the
    geodesic bounding box or the length of a LineString.
*/

class GEODATA_EXPORT GeoDataLineString : public GeoDataGeometry
{

 public:
    typedef QVector<GeoDataCoordinates>::Iterator Iterator;
    typedef QVector<GeoDataCoordinates>::ConstIterator ConstIterator;
    typedef QVector<GeoDataCoordinates>::const_iterator const_iterator;


/*!
    \brief Creates a new LineString.
*/
    explicit GeoDataLineString( TessellationFlags f = NoTessellation );


/*!
    \brief Creates a LineString from an existing geometry object.
*/
    GeoDataLineString( const GeoDataGeometry &other );


/*!
    \brief Destroys a LineString.
*/
    virtual ~GeoDataLineString();

/*!
    \brief Returns whether a LineString is a closed polygon.

    \return <code>false</code> if the LineString is not a LinearRing.
*/
    virtual bool isClosed() const;


/*!
    \brief Returns whether the LineString follows the earth's surface.

    \return <code>true</code> if the LineString's line segments follow the
    earth's surface and terrain along great circles.
*/
    bool tessellate() const;


/*!
    \brief Sets the tessellation property for the LineString.

    If \a tessellate is <code>true</code> then the LineString's line segments
    are bent and follow the earth's surface and terrain along great circles.
    If \a tessellate is <code>false</code> then the LineString's line segments
    are rendered as straight lines in screen coordinate space.
*/
    void setTessellate( bool tessellate );


/*!
    \brief Returns the tessellation flags for a LineString.
*/
    TessellationFlags tessellationFlags() const;


/*!
    \brief Sets the given tessellation flags for a LineString.
*/
    void setTessellationFlags( TessellationFlags f );


/*!
    \brief Returns the smallest latLonAltBox that contains the LineString.

    \see GeoDataLatLonAltBox
*/

   virtual GeoDataLatLonAltBox latLonAltBox() const;


   QPair <GeoDataCoordinates, GeoDataCoordinates> southernMostIDLCrossing() const;

   QPair <GeoDataCoordinates, GeoDataCoordinates> northernMostIDLCrossing() const;

/*!
    \brief Returns the number of intersections between the linestring and the IDL
    (i.e. if the number is odd (and the linestring is closed), it means that the linestring contains a pole)
*/

   int howManyIDLCrossings() const;

/**
  * @brief Returns the length of LineString across a sphere starting from a coordinate in LineString
  * This method can be used as an approximation for distances along LineStrings.
  * The unit used for the resulting length matches the unit of the planet
  * radius.
  * @param planetRadius radius of the sphere
  * @param offset position of coordinate within LineString
  */
    virtual qreal length( qreal planetRadius, int offset = 0 ) const;

/*!
    \brief Provides a more generic representation of the LineString.

    The LineString is normalized, pole corrected and dateline corrected.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual QVector<GeoDataLineString*> toRangeCorrected() const;


/*!
    \brief The line string with nodes that have proper longitude/latitude ranges.

    \return A LineString that resembles the original linestring with nodes that
            have longitude values between -180 and +180 deg and that
            feature latitude values between -90 and +90 deg.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual GeoDataLineString toNormalized() const;


/*!
    \brief The line string with more generic pole values.

    \return A LineString that resembles the original linestring. Nodes that
            represent one of the poles are duplicated to allow for a better
            visualization of flat projections.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual GeoDataLineString toPoleCorrected() const;


/*!
    \brief The line string corrected for date line crossing.

    \return A set of LineStrings that don't cross the dateline and which
            resemble the original linestring.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual QVector<GeoDataLineString*> toDateLineCorrected() const;



    // "Reimplementation" of QVector API
/*!
    \brief Returns whether the LineString has no nodes at all.

    \return <code>true</code> if there are no nodes inside the line string.
*/
    bool isEmpty() const;


/*!
    \brief Returns the number of nodes in a LineString.
*/
    int size() const;

/*!
    \brief Returns the number of nodes in a filtered LineString.
*/

//    int sizeFiltered() const;


/*!
    \brief Returns a reference to the coordinates of a node at a given position.
    This method detaches the returned coordinate object from the line string.
*/
    GeoDataCoordinates& at( int pos );

/*!
    \brief Assigns the detail levels by using Douglas-Peucker
*/

    void assignDetailLevelsDP( QVector<GeoDataCoordinates>::ConstIterator itLeft, QVector<GeoDataCoordinates>::ConstIterator itRight, int currentDetailLevel ) const;

/*!
    \brief Returns a reference to the coordinates of the first node that has a detailLevel greater or equal than the current one.
*/
   
    void nextFilteredAt( QVector<GeoDataCoordinates>::ConstIterator &itCoordsCurrent, int detailLevel ) const;

/*!
    \brief Returns a reference to the coordinates of a node at a given position.
    This method does not detach the returned coordinate object from the line string.
*/
    const GeoDataCoordinates& at( int pos ) const;

/*!
    \brief Returns a reference to the coordinates of a node at a given position.
    This method detaches the returned coordinate object from the line string.
*/
    GeoDataCoordinates& operator[]( int pos );


/*!
    \brief Returns a reference to the coordinates of a node at a given position.
    This method does not detach the returned coordinate object from the line string.
*/
    const GeoDataCoordinates& operator[]( int pos ) const;


/*!
    \brief Returns a reference to the first node in the LineString.
    This method detaches the returned coordinate object from the line string.
*/
    GeoDataCoordinates& first();


/*!
    \brief Returns a reference to the first node in the LineString.
    This method does not detach the returned coordinate object from the line string.
*/
    const GeoDataCoordinates& first() const;


/*!
    \brief Returns a reference to the last node in the LineString.
    This method detaches the returned coordinate object from the line string.
*/
    GeoDataCoordinates& last();


/*!
    \brief Returns a reference to the last node in the LineString.
    This method does not detach the returned coordinate object from the line string.
*/
    const GeoDataCoordinates& last() const;


/*!
    \brief Appends a given geodesic position as a new node to the LineString.
*/
    void append ( const GeoDataCoordinates& position );


/*!
    \brief Appends a given geodesic position as a new node to the LineString.
*/
    GeoDataLineString& operator << ( const GeoDataCoordinates& position );


/*!
    \brief Appends a given LineString to the end of the LineString.
*/
    GeoDataLineString& operator << ( const GeoDataLineString& lineString );


/*!
    \brief Returns an iterator that points to the begin of the LineString.
*/
    QVector<GeoDataCoordinates>::Iterator begin();


/*!
    \brief Returns an iterator that points to the end of the LineString.
*/
    QVector<GeoDataCoordinates>::Iterator end();


/*!
    \brief Returns a const iterator that points to the begin of the LineString.
*/
    QVector<GeoDataCoordinates>::ConstIterator constBegin() const;


/*!
    \brief Returns a const iterator that points to the end of the LineString.
*/
    QVector<GeoDataCoordinates>::ConstIterator constEnd() const;


/*!
    \brief Douglas-Peucker algorithm for filtering the LineString
*/

//    void douglasPeucker( QVector<GeoDataCoordinates> &vector, qreal epsilon, QVector<GeoDataCoordinates> &vectorFiltered ) const;
//    void douglasPeucker( QVector<GeoDataCoordinates>::const_iterator itLeft, QVector<GeoDataCoordinates>::const_iterator itRight, qreal epsilon ) const;


/*!
    \brief Returns the distance from the first point to the segment determined by the other two
*/

    qreal perpendicularDistance( const GeoDataCoordinates &A, const GeoDataCoordinates &B, const GeoDataCoordinates &C ) const;

/*!
    \brief Returns the right epsilon for Douglas-Peucker according to the detail level
*/

    qreal epsilonFromDetailLevel( int detailLevel ) const;

/*!
    \brief Returns an iterator that points to the begin of the filtered LineString.
*/
   
/*
    QVector<GeoDataCoordinates>::ConstIterator constBeginFiltered( int detailLevel ) const;
*/    

/*!
    \brief Returns an iterator that points to the end of the filtered LineString.
*/

/*
    QVector<GeoDataCoordinates>::ConstIterator constEndFiltered() const;
*/


/*!
    \brief Destroys all nodes in a LineString.
*/
    void clear();


/*!
    \brief Removes the node at the given position and returns it.
*/
    QVector<GeoDataCoordinates>::Iterator erase ( QVector<GeoDataCoordinates>::Iterator position );


/*!
    \brief Removes the nodes within the given range and returns them.
*/
    QVector<GeoDataCoordinates>::Iterator erase ( QVector<GeoDataCoordinates>::Iterator begin,
                                                  QVector<GeoDataCoordinates>::Iterator end );


/*!
    \brief Removes the node at the given position and destroys it.
*/
    void remove ( int i );


    // Serialization
/*!
    \brief Serialize the LineString to a stream.
    \param stream the stream.
*/
    virtual void pack( QDataStream& stream ) const;


/*!
    \brief Unserialize the LineString from a stream.
    \param stream the stream.
*/
    virtual void unpack( QDataStream& stream );


 protected:
    GeoDataLineStringPrivate *p() const;
    GeoDataLineString(GeoDataLineStringPrivate* priv);
};

}

Q_DECLARE_METATYPE( Marble::GeoDataLineString )

#endif
