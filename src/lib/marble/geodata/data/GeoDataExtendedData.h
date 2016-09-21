//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010     Harshit Jain <hjain.itbhu@gmail.com>
// Copyright 2011     Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_GEODATAEXTENDEDDATA_H
#define MARBLE_GEODATAEXTENDEDDATA_H

#include "GeoDataObject.h"

#include "geodata_export.h"
#include <QHash>

namespace Marble
{

class GeoDataSimpleArrayData;
class GeoDataSchemaData;
class GeoDataData;

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
    virtual const char* nodeType() const;

    /**
     * @brief assignment operator
     */
    GeoDataExtendedData& operator=( const GeoDataExtendedData& other );

    bool operator==( const GeoDataExtendedData& other ) const;
    bool operator!=( const GeoDataExtendedData& other ) const;

    /**
     * @brief return the value of GeoDataExtendedData associated with the given @p key 
     */
    GeoDataData value( const QString& key ) const;

    /**
     * @brief add a data object to the GeoDataExtendedData with the @p key 
     */
    void addValue( const GeoDataData& data );

    /**
     * @since 0.26.0
     */
    void removeKey(const QString &key);
    
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
     * @brief set SimpleArrayData for given @p key
     *
     * ExtendedData takes ownership of SimpleArrayData
     */
    void setSimpleArrayData( const QString& key, GeoDataSimpleArrayData* values );

    /**
     * @brief return SimpleArrayData for given @p key, 0 pointer if none is set
     */
    GeoDataSimpleArrayData* simpleArrayData( const QString& key ) const;

    /**
     * @brief Adds a SchemaData @p schemaData element to schemaDataHash
     */
    GeoDataSchemaData& schemaData( const QString& schemaUrl ) const;

    /**
     * @brief Adds a SchemaData @p schemaData element to schemaDataHash
     */
    void addSchemaData( const GeoDataSchemaData& schemaData );

    /**
     * @brief Removes a SchemaData element with schema url @p schemaUrl from schemaDataHash
     */
    void removeSchemaData( const QString& schemaUrl );

    /**
     * @brief Dump a vector containing all SchemaData element
     */
    QList< GeoDataSchemaData > schemaDataList() const;

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
