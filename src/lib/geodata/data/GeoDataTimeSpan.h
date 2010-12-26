//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATATIMESPAN_H
#define MARBLE_GEODATATIMESPAN_H

#include <QtCore/QString>
#include <QtCore/QDateTime>

#include "GeoDataObject.h"
#include "GeoDataTimePrimitive.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataTimeSpanPrivate;

class GEODATA_EXPORT GeoDataTimeSpan : public GeoDataTimePrimitive
{
  public:

    GeoDataTimeSpan();
    GeoDataTimeSpan( const GeoDataTimeSpan& other );
    ~GeoDataTimeSpan();

    /**
    * @brief assignment operator
    */
    GeoDataTimeSpan& operator=( const GeoDataTimeSpan& other );

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const;

    /**
    * @brief return the beginning instant of a timespan
    */
    QDateTime begin() const;

    /**
    * @brief Set the beginning instant of a timespan
    * @param begin the beginning instant of a timespan
    */
    void setBegin( const QDateTime& begin );
    
    /**
    * @brief return the ending instant of a timespan
    */
    QDateTime end() const;

    /**
    * @brief Set the ending instant of a timespan
    * @param begin the ending instant of a timespan
    */
    void setEnd( const QDateTime& end );

    /**
     * @brief Serialize the timespan to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * @brief  Unserialize the timespan from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataTimeSpanPrivate * const d;
};

}

#endif //MARBLE_GEODATATIMESPAN_H
