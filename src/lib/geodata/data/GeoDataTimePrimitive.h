//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATATIMEPRIMITIVE_H
#define MARBLE_GEODATATIMEPRIMITIVE_H

#include <QtCore/QString>

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataTimePrimitivePrivate;

/**
 * @short a base class for the style classes
 *
 * A GeoDataTimePrimitive is a base class for the time classes TimeSpan
 * and TimeStamp classes.
 *
 * @See GeoDataTimeSpan
 * @See GeoDataTimeStamp
 */
class GEODATA_EXPORT GeoDataTimePrimitive : public GeoDataObject
{
  public:
    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;

    GeoDataTimePrimitive& operator=( const GeoDataTimePrimitive& other );

    /**
     * @brief Serialize the styleselector to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * @brief  Unserialize the styleselector from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

    GeoDataTimePrimitive();
    GeoDataTimePrimitive( const GeoDataTimePrimitive& other );
    ~GeoDataTimePrimitive();
private:
    GeoDataTimePrimitivePrivate * const d;
};

}

#endif
