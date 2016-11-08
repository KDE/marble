//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GEODATASCHEMADATA_H
#define MARBLE_GEODATASCHEMADATA_H

// Qt
#include <QList>

// Marble
#include "GeoDocument.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataSchemaDataPrivate;
class GeoDataSimpleData;

/**
 */
class GEODATA_EXPORT GeoDataSchemaData : public GeoNode
{
public:
    GeoDataSchemaData();
    GeoDataSchemaData( const GeoDataSchemaData& other );
    GeoDataSchemaData& operator=( const GeoDataSchemaData& rhs );
    bool operator==( const GeoDataSchemaData &other ) const;
    bool operator!=( const GeoDataSchemaData &other ) const;
    ~GeoDataSchemaData();

    /*
     * @brief Returns the id for schema which defines custom data
     */
    QString schemaUrl() const;

    /*
     * @brief Set the schemaUrl attribute of SchemaData to @p schemaUrl
     */
    void setSchemaUrl( const QString& schemaUrl );

    /*
     * @brief Returns the SimpleData having name attribute @p name
     */
    GeoDataSimpleData& simpleData( const QString& name ) const;

    /*
     * @brief Adds a SimpleData @p simpleData to schemaDataHash
     */
    void addSimpleData( const GeoDataSimpleData& simpleData );

    /*
     * @brief Dump a list containing all SimpleData values stored in schemaDataHash
     */
    QList<GeoDataSimpleData> simpleDataList() const;

    /*
     * @brief Set the parent @parent
     */
    void setParent( GeoNode *parent );

    /*
     * @brief Get the parent
     */
    const GeoNode *parent() const;
    GeoNode *parent();

    /*
     * @brief Provides information for downcasting a GeoNode
     */
    virtual const char* nodeType() const;

    /*
     * @brief Serialize SchemaData to a stream @p stream
     */
    virtual void pack( QDataStream& stream ) const;

    /*
     * @brief Unserialize SchemaData from a stream @p stream
     */
    virtual void unpack( QDataStream& stream );

private:
    GeoDataSchemaDataPrivate * const d;
};

}   // namespace Marble

#endif  // MARBLE_GEODATASCHEMADATA_H
