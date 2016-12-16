//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_GEODATADATA_H
#define MARBLE_GEODATADATA_H

#include "GeoDataObject.h"

#include "geodata_export.h"

class QVariant;

namespace Marble
{

class GeoDataDataPrivate;

/**
 */
class GEODATA_EXPORT GeoDataData : public GeoDataObject
{
  public:
    GeoDataData();

    /** Convenience constructor that sets name and value directly */
    GeoDataData( const QString &name, const QVariant &value );

    GeoDataData( const GeoDataData& other );
    bool operator==( const GeoDataData& other ) const;
    bool operator!=( const GeoDataData& other ) const;
    ~GeoDataData() override;

    /**
     * @brief assignment operator
     */
    GeoDataData& operator=( const GeoDataData& other );

    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    /**
    * @brief return a reference to the value of data;
    */
    QVariant& valueRef();
    const QVariant& valueRef() const;

    /**
    * @brief return the value of data
    */
    QVariant value() const;

    /**
    * @brief set the value of data
    * @param value the value to be set
    */
    void setValue( const QVariant& value );

    /**
    * @brief return the name of data
    */
    QString name() const;

    /**
    * @brief set the name of data
    * @param name the name to be set
    */
    void setName( const QString& name );

    /**
    * @brief return the displayName of data
    */
    QString displayName() const;

    /**
    * @brief set the displayName of data
    * @param value the displayName to be set
    */
    void setDisplayName( const QString& displayName );

    /// Serialize the contents of the feature to @p stream.
    void pack( QDataStream& stream ) const override;

    /// Unserialize the contents of the feature from @p stream.
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataDataPrivate * const d;
};

}

#endif //MARBLE_GEODATADATA_H
