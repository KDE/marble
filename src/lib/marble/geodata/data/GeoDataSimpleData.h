//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GEODATASIMPLEDATA_H
#define MARBLE_GEODATASIMPLEDATA_H

#include "GeoDocument.h"
#include "geodata_export.h"

class QString;

namespace Marble
{

class GeoDataSimpleDataPrivate;

/**
 */
class GEODATA_EXPORT GeoDataSimpleData : public GeoNode
{
public:
    GeoDataSimpleData();
    GeoDataSimpleData( const GeoDataSimpleData &other );
    virtual ~GeoDataSimpleData();

    /*
     * @brief Returns the value of name attribute of SimpleData tag
     */
    QString name() const;

    /*
     * @brief Set the name of SimpleData to @p name
     */
    void setName( const QString &name );

    /*
     * @brief Returns the data defined by SimpleData
     */
    QString data() const;

    /*
     * @brief Set the value of data defined by SimpleData to @p data
     */
    void setData( const QString &data );

    /*
     * Assignment operator
     */
    GeoDataSimpleData &operator=( const GeoDataSimpleData &rhs );

    /*
     * @brief Equality operator
     */
    bool operator==( const GeoDataSimpleData &other ) const;
    bool operator!=( const GeoDataSimpleData &other ) const;

    /*
     * Provides information for downcasting a GeoNode
     */
    virtual const char* nodeType() const;

    /*
     * Seriliaze SimpleData to stream @p stream
     */
    virtual void pack( QDataStream &stream ) const;

    /*
     * Unseriliaze SimpleData from stream @p stream
     */
    virtual void unpack( QDataStream &stream );

private:
    GeoDataSimpleDataPrivate * const d;
};

} // namespace Marble

#endif  // MARBLE_GEODATASIMPLEDATA_H
