//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010     Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATAEXTENDEDDATA_H
#define MARBLE_GEODATAEXTENDEDDATA_H

#include <QtCore/QString>

#include "GeoDataObject.h"
#include "GeoDataData.h"

#include "geodata_export.h"
#include <QHash>

namespace Marble
{

class GeoDataExtendedDataPrivate;

/**
 * @short a class which allows to add custom data to KML Feature.
 *
 * @See GeoDataData
 */
class GEODATA_EXPORT GeoDataExtendedData : public GeoDataObject
{
  public:
    GeoDataExtendedData();
    GeoDataExtendedData( const GeoDataExtendedData& other );
    virtual ~GeoDataExtendedData();

    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const;

    /**
     * @brief assignment operator
     */
    GeoDataExtendedData& operator=( const GeoDataExtendedData& other );

    /**
     * @brief return the value of GeoDataExtendedData associated with the given @p key 
     */
    GeoDataData value( const QString& key ) const;

    /**
     * @brief add a data object to the GeoDataExtendedData with the @p key 
     */
    void addValue( const GeoDataData& data );
    
    /**
      * @brief return const Begin iterator for QHash
      */
    QHash< QString, GeoDataData >::const_iterator constBegin( ) const;

    /**
     * @brief return const End iterator for QHash
     */

	QHash< QString, GeoDataData >::const_iterator constEnd( ) const;
    
    /**
     * @brief return size of QHash
     */

    int size( ) const;

    /**
      * @brief return wthethe QHash is empty or not
      */
    bool isEmpty( ) const;

    /**
      * @brief Returns true if there exists a value for the given key
      */
    bool contains( const QString &key ) const;

    /**
     * @brief return value of GeoDataExtendedData object associated with the given @p key as a modifiable reference
     */
    GeoDataData& valueRef( const QString& key ) const;
    

    /**
     * @brief Serialize the ExtendedData to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * @brief  Unserialize the ExtendedData from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

private:
    GeoDataExtendedDataPrivate * const d;
};

}

#endif
