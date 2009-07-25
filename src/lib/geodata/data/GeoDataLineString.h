//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef GEODATALINESTRING_H
#define GEODATALINESTRING_H

#include <QtCore/QFlags>
#include <QtCore/QVector>

#include "global.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"


namespace Marble
{

class GeoDataLineStringPrivate;

/**
 * @short A class that defines a contiguous set of line segments.
 *
 * Also known as: non-closed Polyline
 *
 * GeoDataLineString consists of GeoDataCoordinates connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataLineString.
 * This class will at some point entirely replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataLineString : public GeoDataGeometry {

 public:
    typedef QVector<GeoDataCoordinates>::Iterator Iterator;
    typedef QVector<GeoDataCoordinates>::ConstIterator ConstIterator;
    typedef QVector<GeoDataCoordinates>::const_iterator const_iterator;
    
    GeoDataLineString( TessellationFlags f = NoTessellation );
    GeoDataLineString( const GeoDataGeometry &other );

    virtual ~GeoDataLineString();

    virtual bool isClosed() const;

    bool tessellate() const;
    void setTessellate( bool tessellate );

    TessellationFlags tessellationFlags() const;
    void setTessellationFlags( TessellationFlags f );

    GeoDataLatLonAltBox latLonAltBox() const;

    // Returns the length of the linestring on a sphere.
    // As a parameter the planet's radius needs to be passed.
    // The return value is the length of the line string.
    // The unit used for the length matches the unit of the
    // planet radius.
    virtual qreal length( qreal planetRadius ) const;

    virtual QVector<GeoDataLineString*> toRangeCorrected() const;

    virtual GeoDataLineString toNormalized() const;

    virtual GeoDataLineString toPoleCorrected() const;

    virtual QVector<GeoDataLineString*> toDateLineCorrected() const;

    bool isEmpty() const;
    int size() const;
    GeoDataCoordinates& at( int pos );
    const GeoDataCoordinates& at( int pos ) const;
    GeoDataCoordinates& operator[]( int pos );
    const GeoDataCoordinates& operator[]( int pos ) const;

    GeoDataCoordinates& first();
    const GeoDataCoordinates& first() const;
    GeoDataCoordinates& last();
    const GeoDataCoordinates& last() const;

    void append ( const GeoDataCoordinates& value );
    GeoDataLineString& operator << ( const GeoDataCoordinates& value );
    GeoDataLineString& operator << ( const GeoDataLineString& value );
    
    QVector<GeoDataCoordinates>::Iterator begin();
    QVector<GeoDataCoordinates>::Iterator end();
    QVector<GeoDataCoordinates>::ConstIterator constBegin() const;
    QVector<GeoDataCoordinates>::ConstIterator constEnd() const;
    void clear();

    QVector<GeoDataCoordinates>::Iterator erase ( QVector<GeoDataCoordinates>::Iterator pos );
    QVector<GeoDataCoordinates>::Iterator erase ( QVector<GeoDataCoordinates>::Iterator begin,
                                                  QVector<GeoDataCoordinates>::Iterator end );

    /**
     * @brief  Serialize the style to a stream.
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );
 protected:
    GeoDataLineStringPrivate *p() const;
    GeoDataLineString(GeoDataLineStringPrivate* priv);
};

}

#endif // GEODATALINESTRING_H
