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

#include "GeoDataObject.h"

class QVariant;

namespace Marble {

class GeoDataSimpleArrayDataPrivate;

/**
 */
class GEODATA_EXPORT GeoDataSimpleArrayData : public GeoDataObject
{

public:
    GeoDataSimpleArrayData();
    GeoDataSimpleArrayData( const GeoDataSimpleArrayData& other );
    bool operator==( const GeoDataSimpleArrayData &other ) const;
    bool operator!=( const GeoDataSimpleArrayData &other) const;
    ~GeoDataSimpleArrayData() override;

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

    const char* nodeType() const override;
    void pack( QDataStream& stream ) const override;
    void unpack( QDataStream& stream ) override;

private:
    GeoDataSimpleArrayDataPrivate *d;
};

}

#endif // MARBLE_GEODATASIMPLEARRAYDATA_H
