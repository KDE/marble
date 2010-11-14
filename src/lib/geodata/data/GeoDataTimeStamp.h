//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATATIMESTAMP_H
#define MARBLE_GEODATATIMESTAMP_H

#include <QtCore/QString>
#include <QtCore/QDateTime>

#include "GeoDataObject.h"
#include "GeoDataTimePrimitive.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataTimeStampPrivate;

class GEODATA_EXPORT GeoDataTimeStamp : public GeoDataTimePrimitive
{
  public:
    GeoDataTimeStamp();
    GeoDataTimeStamp( const GeoDataTimeStamp& other );
    virtual ~GeoDataTimeStamp();

    /**
    * @brief assignment operator
    */
    GeoDataTimeStamp& operator=( const GeoDataTimeStamp& other );

    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;

    /**
    * @brief return the when time of timestamp
    */
    QDateTime when() const;
 
    /**
    * @brief Set the when time of timestamp
    * @param when the when time of timestamp
    */
    void setWhen( const QDateTime& when );
    
    /**
     * @brief Serialize the timestamp to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * @brief  Unserialize the timestamp from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataTimeStampPrivate * const d;
};

}

#endif
