// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
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
    ~GeoDataTimeSpan() override;

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
    const char* nodeType() const override;

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
    * @param end the ending instant of a timespan
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
    void pack( QDataStream& stream ) const override;

    /**
     * @brief  Unserialize the timespan from a stream
     * @param  stream  the stream
     */
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataTimeSpanPrivate * const d;
};

}

#endif //MARBLE_GEODATATIMESPAN_H
