//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_GEODATASIMPLEARRAYDATA_H
#define MARBLE_GEODATASIMPLEARRAYDATA_H

#include "GeoDataCoordinates.h"
#include "GeoDataGeometry.h"

#include <QtCore/QDateTime>
#include <QtCore/QPair>

namespace Marble {

class GeoDataSimpleArrayDataPrivate;

class GEODATA_EXPORT GeoDataSimpleArrayData : public GeoDataGeometry
{

public:
    GeoDataSimpleArrayData();
    GeoDataSimpleArrayData( const GeoDataGeometry &other );

    /**
     * Returns the number of value in the array
     */
    int size() const;

    /**
     * Returns the value at index @p index
     */
    QVariant valueAt( int index ) const;

    /**
     * Returns all values in the array
     */
    QList< QVariant > valuesList() const;

    /**
     * Append a value to the array
     */
    void append( const QVariant& value );

    virtual const char* nodeType() const;
    virtual EnumGeometryId geometryId() const;
    virtual GeoDataLatLonAltBox latLonAltBox() const;
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

private:
    GeoDataSimpleArrayDataPrivate *d;
};

}

#endif // MARBLE_GEODATASIMPLEARRAYDATA_H
