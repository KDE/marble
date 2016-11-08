//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GEODATASCHEMA_H
#define MARBLE_GEODATASCHEMA_H

// Qt
#include <QHash>
#include <QList>

// Marble
#include "GeoDataObject.h"
#include "geodata_export.h"

class QDataStream;

namespace Marble
{

class GeoDataSchemaPrivate;
class GeoDataSimpleField;

/**
 */
class GEODATA_EXPORT GeoDataSchema : public GeoDataObject
{
public:
    GeoDataSchema();
    explicit GeoDataSchema( const QHash<QString, GeoDataSimpleField>& simpleFields );
    GeoDataSchema( const GeoDataSchema& other );
    GeoDataSchema& operator=( const GeoDataSchema& other );
    bool operator==( const GeoDataSchema& other ) const;
    bool operator!=( const GeoDataSchema& other ) const;
    ~GeoDataSchema();

    /*
     * @brief Returns the name attribute of schema
     */
    QString schemaName() const;

    /*
     * @brief Sets the name attribute of the schema
     * @param name  The name to be set
     */
    void setSchemaName( const QString& name );

    /*
     * @brief Returns the SimpleField child of schema
     * @param name  The value of name attribute of SimpleField which is to be returned
     */
    GeoDataSimpleField& simpleField( const QString &name ) const;

    /*
     * @brief Adds a SimpleField to schema
     * @param value  The SimpleField to be added
     */
    void addSimpleField( const GeoDataSimpleField& value );

    /*
     * @brief dump a vector containing all simple fields of schema
     */
    QList<GeoDataSimpleField> simpleFields() const;

    virtual const char* nodeType() const;

    virtual void pack( QDataStream& stream ) const;

    virtual void unpack( QDataStream& stream );

private:
    GeoDataSchemaPrivate * const d;

};

}

#endif  // MARBLE_GEODATASCHEMA_H
