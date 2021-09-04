// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATALINESTRING_H
#define MARBLE_GEODATALINESTRING_H

#include <QVector>
#include <QMetaType>

#include "MarbleGlobal.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"


namespace Marble
{
class GeoDataCoordinates;
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
    using Iterator = QVector<GeoDataCoordinates>::Iterator;
    using ConstIterator = QVector<GeoDataCoordinates>::ConstIterator;
    using const_iterator = QVector<GeoDataCoordinates>::const_iterator;


/*!
    \brief Creates a new LineString.
*/
    explicit GeoDataLineString( TessellationFlags f = NoTessellation );


/*!
    \brief Creates a LineString from an existing geometry object.
*/
    explicit GeoDataLineString( const GeoDataGeometry &other );


/*!
    \brief Destroys a LineString.
*/
    ~GeoDataLineString() override;

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

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
    \brief Reverses the LineString.
    @since 0.26.0
*/
     void reverse();

/*!
    \brief Returns the smallest latLonAltBox that contains the LineString.

    \see GeoDataLatLonAltBox
*/

   const GeoDataLatLonAltBox& latLonAltBox() const override;

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

    The LineString is normalized, and pole corrected.

    Deprecation Warning: This method will likely be removed from the public API.
*/
    virtual GeoDataLineString toRangeCorrected() const;


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
    \brief Returns a reference to the coordinates of a node at a given position.
    This method detaches the returned coordinate object from the line string.
*/
    GeoDataCoordinates& at( int pos );


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


    /**
      Returns a sub-string which contains elements from this vector, starting at position pos. If length is -1
      (the default), all elements after pos are included; otherwise length elements (or all remaining elements if
      there are less than length elements) are included.
      */
    GeoDataLineString mid(int pos, int length = -1) const;

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
    \brief Inserts a new node at the given index.
*/
    void insert( int index, const GeoDataCoordinates& value );

/*!
    \brief Attempts to allocate memory for at least \a size coordinates.
*/
    void reserve(int size);

/*!
    \brief Appends a given geodesic position as a new node to the LineString.
*/
    void append ( const GeoDataCoordinates& value );


/*!
    \brief Appends a given geodesic position as new nodes to the LineString.
*/
    void append(const QVector<GeoDataCoordinates>& values);


/*!
    \brief Appends a given geodesic position as a new node to the LineString.
*/
    GeoDataLineString& operator << ( const GeoDataCoordinates& value );


/*!
    \brief Appends a given LineString to the end of the LineString.
*/
    GeoDataLineString& operator << ( const GeoDataLineString& lineString );


/*!
    \brief Returns true/false depending on whether this and other are/are not equal.
*/
    bool operator==( const GeoDataLineString &other ) const;
    bool operator!=( const GeoDataLineString &other ) const;


/*!
    \brief Returns an iterator that points to the begin of the LineString.
*/
    QVector<GeoDataCoordinates>::Iterator begin();
    QVector<GeoDataCoordinates>::ConstIterator begin() const;


/*!
    \brief Returns an iterator that points to the end of the LineString.
*/
    QVector<GeoDataCoordinates>::Iterator end();
    QVector<GeoDataCoordinates>::ConstIterator end() const;


/*!
    \brief Returns a const iterator that points to the begin of the LineString.
*/
    QVector<GeoDataCoordinates>::ConstIterator constBegin() const;


/*!
    \brief Returns a const iterator that points to the end of the LineString.
*/
    QVector<GeoDataCoordinates>::ConstIterator constEnd() const;


/*!
    \brief Destroys all nodes in a LineString.
*/
    void clear();


/*!
    \brief Removes the node at the given position and returns it.
*/
    QVector<GeoDataCoordinates>::Iterator erase ( const QVector<GeoDataCoordinates>::Iterator& position );


/*!
    \brief Removes the nodes within the given range and returns them.
*/
    QVector<GeoDataCoordinates>::Iterator erase ( const QVector<GeoDataCoordinates>::Iterator& begin,
                                                  const QVector<GeoDataCoordinates>::Iterator& end );


/*!
    \brief Removes the node at the given position and destroys it.
*/
    void remove ( int i );

    /*!
        \brief Returns a linestring with detail values assigned to each node.
    */
    GeoDataLineString optimized() const;

    /*!
        \brief Returns a javascript-style list (that can be used e.g. with the QML GeoPolyline element).
    */
    QVariantList toVariantList() const;

    // Serialization
/*!
    \brief Serialize the LineString to a stream.
    \param stream the stream.
*/
    void pack( QDataStream& stream ) const override;


/*!
    \brief Unserialize the LineString from a stream.
    \param stream the stream.
*/
    void unpack( QDataStream& stream ) override;

 protected:
    explicit GeoDataLineString(GeoDataLineStringPrivate* priv);

 private:
    Q_DECLARE_PRIVATE(GeoDataLineString)
};

}

Q_DECLARE_METATYPE( Marble::GeoDataLineString )

#endif
