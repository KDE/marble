//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GEODATASIMPLEFIELD_H
#define MARBLE_GEODATASIMPLEFIELD_H

#include "geodata_export.h"
#include "GeoDocument.h"

class QDataStream;
class QString;

namespace Marble
{

class GeoDataSimpleFieldPrivate;

/**
 */
class GEODATA_EXPORT GeoDataSimpleField : public GeoNode
{
public:
    GeoDataSimpleField();
    GeoDataSimpleField( const GeoDataSimpleField& other );
    bool operator==( const GeoDataSimpleField& other ) const;
    bool operator!=( const GeoDataSimpleField& other ) const;
    virtual ~GeoDataSimpleField();

    /*
     * @brief Enum for different values of type attribute of SimpleField
     */
    enum SimpleFieldType {
        String,
        Int,
        UInt,
        Short,
        UShort,
        Float,
        Double,
        Bool
    };

    /*
     * @brief Return the value of type attribute of simple field
     */
    SimpleFieldType type() const;

    /*
     * @brief Sets the value of type attribute
     * @param type  The of type attribute
     */
    void setType(SimpleFieldType type);

    /*
     * @brief Returns the value of name attribute of simple field
     */
    QString name() const;

    /*
     * @brief Set the value of name attribute of SimpleField
     * @param value  The value to be set as name attribute
     */
    void setName( const QString& value );

    /*
     * @brief Returns the value of displayField child element of SimpleField tag
     */
    QString displayName() const;

    /*
     * @brief Set the value for displayName tag
     * @param displayName  The value to be set for displayName tag
     */
    void setDisplayName( const QString& displayName );

    /*
     * @brief The assignment operator
     * @param rhs  The object to be duplicated
     */
    GeoDataSimpleField& operator=( const GeoDataSimpleField& rhs );

    /*
     * @brief Provides information for downcasting a GeoNode
     */
    virtual const char* nodeType() const;

    /*
     * @brief Serialize SimpleField to a stream
     * @param stream  The stream
     */
    virtual void pack( QDataStream& stream ) const;

    /*
     * @brief Unserialize SimpleField from a stream
     * @param stream  The stream
     */
    virtual void unpack( QDataStream& stream );

private:
    GeoDataSimpleFieldPrivate * const d;

};

}

#endif // MARBLE_GEODATASIMPLEFIELD_H
