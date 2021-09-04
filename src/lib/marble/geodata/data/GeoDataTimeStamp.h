// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATATIMESTAMP_H
#define MARBLE_GEODATATIMESTAMP_H

#include "GeoDataTimePrimitive.h"

#include "geodata_export.h"

class QDateTime;

namespace Marble
{

class GeoDataTimeStampPrivate;

/**
 */
class GEODATA_EXPORT GeoDataTimeStamp : public GeoDataTimePrimitive
{
  public:
    enum TimeResolution {
        SecondResolution,
        DayResolution,
        MonthResolution,
        YearResolution
    };

    GeoDataTimeStamp();
    GeoDataTimeStamp( const GeoDataTimeStamp& other );
    ~GeoDataTimeStamp() override;

    /**
    * @brief assignment operator
    */
    GeoDataTimeStamp& operator=( const GeoDataTimeStamp& other );

    /**
     * @brief equality operators
     */
    bool operator==( const GeoDataTimeStamp& other ) const;
    bool operator!=( const GeoDataTimeStamp& other ) const;


    /// Provides type information for downcasting a GeoNode
    const char* nodeType() const override;

    /**
    * @brief return the when time of timestamp
    */
    QDateTime when() const;
 
    /**
    * @brief Set the when time of timestamp
    * @param when the when time of timestamp
    */
    void setWhen( const QDateTime& when );
    
    void setResolution( TimeResolution resolution );

    TimeResolution resolution() const;

    /**
     * @brief Serialize the timestamp to a stream
     * @param  stream  the stream
     */
    void pack( QDataStream& stream ) const override;

    /**
     * @brief  Unserialize the timestamp from a stream
     * @param  stream  the stream
     */
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataTimeStampPrivate * const d;
};

}

#endif
