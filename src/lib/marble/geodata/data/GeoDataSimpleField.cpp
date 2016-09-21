//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

// Qt
#include <QDataStream>
#include <QString>

// Marble
#include "GeoDataSimpleField.h"
#include "GeoDataTypes.h"


namespace Marble
{

class GeoDataSimpleFieldPrivate
{
  public:
    QString m_name;
    GeoDataSimpleField::SimpleFieldType m_type;
    QString m_displayName;
};

GeoDataSimpleField::GeoDataSimpleField()
    : GeoNode(),
      d( new GeoDataSimpleFieldPrivate )
{
}

GeoDataSimpleField::GeoDataSimpleField( const GeoDataSimpleField& other )
    : GeoNode(),
      d( new GeoDataSimpleFieldPrivate( *other.d ) )
{
}

bool GeoDataSimpleField::operator==(const GeoDataSimpleField& other) const
{
    return d->m_name == other.d->m_name && 
           d->m_type == other.d->m_type && 
           d->m_displayName == other.d->m_displayName;
}

bool GeoDataSimpleField::operator!=(const GeoDataSimpleField& other) const
{
    return !this->operator==( other );
}

GeoDataSimpleField::~GeoDataSimpleField()
{
    delete d;
}

GeoDataSimpleField::SimpleFieldType GeoDataSimpleField::type() const
{
    return d->m_type;
}

void GeoDataSimpleField::setType(SimpleFieldType type)
{
    d->m_type = type;
}

QString GeoDataSimpleField::name() const
{
    return d->m_name;
}

void GeoDataSimpleField::setName( const QString& value )
{
    d->m_name = value;
}

QString GeoDataSimpleField::displayName() const
{
    return d->m_displayName;
}

void GeoDataSimpleField::setDisplayName( const QString& displayName )
{
    d->m_displayName = displayName;
}

GeoDataSimpleField& GeoDataSimpleField::operator=( const GeoDataSimpleField& other )
{
    *d = *other.d;
    return *this;
}

const char* GeoDataSimpleField::nodeType() const
{
    return GeoDataTypes::GeoDataSimpleFieldType;
}

void GeoDataSimpleField::pack( QDataStream& stream ) const
{
    stream << d->m_name;
    stream << d->m_displayName;
}

void GeoDataSimpleField::unpack( QDataStream& stream )
{
    stream >> d->m_name;
    stream >> d->m_displayName;
}


}
