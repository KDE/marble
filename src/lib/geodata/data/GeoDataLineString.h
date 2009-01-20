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
 * This class will at some point replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataLineString : public QVector<GeoDataCoordinates>,
                                         public GeoDataGeometry {

 public:

    explicit GeoDataLineString( GeoDataObject *parent = 0, TessellationFlags f = NoTessellation);
    GeoDataLineString( const GeoDataLineString & );
    GeoDataLineString& operator=( const GeoDataLineString & );

    virtual ~GeoDataLineString();

    virtual bool isClosed() const;

    bool tessellate() const;
    void setTessellate( bool tessellate );

    TessellationFlags tessellationFlags() const;
    void setTessellationFlags( TessellationFlags f );

    GeoDataLatLonAltBox latLonAltBox() const;

    void append ( const GeoDataCoordinates& value );
    GeoDataLineString& operator << ( const GeoDataCoordinates& value );
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

    virtual EnumGeometryId geometryId() const { return GeoDataLineStringId; };
 protected:
    GeoDataLineStringPrivate  * d;
 private:
    void detach();
};

}

#endif // GEODATALINESTRING_H
