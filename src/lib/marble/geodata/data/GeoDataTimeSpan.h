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

#include "GeoDataTimePrimitive.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataTimeStamp;
class GeoDataTimeSpanPrivate;

/**
 */
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

    /**
     * @brief equality operators
     */
    bool operator==( const GeoDataTimeSpan& other ) const;
    bool operator!=( const GeoDataTimeSpan& other ) const;

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const;

    /**
    * @brief return the beginning instant of a timespan
    */
    const GeoDataTimeStamp & begin() const;
    GeoDataTimeStamp & begin();

    /**
    * @brief Set the beginning instant of a timespan
    * @param begin the beginning instant of a timespan
    */
    void setBegin( const GeoDataTimeStamp& begin );
    
    /**
    * @brief return the ending instant of a timespan
    */
    const GeoDataTimeStamp & end() const;
    GeoDataTimeStamp & end();

    /**
    * @brief Set the ending instant of a timespan
    * @param begin the ending instant of a timespan
    */
    void setEnd( const GeoDataTimeStamp& end );

    /**
     * @return True iff either of begin or end is valid, or if begin and end are both valid and begin is <= end
     */
    bool isValid() const;

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
