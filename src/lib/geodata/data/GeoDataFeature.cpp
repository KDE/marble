//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataFeature.h"

#include <QtCore/QDataStream>


GeoDataFeature::GeoDataFeature() :
    m_visible( true )
{
}

GeoDataFeature::GeoDataFeature( const QString& name ) :
    m_name( name )
{
}

GeoDataFeature::~GeoDataFeature()
{
}

QString GeoDataFeature::name() const
{
    return m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    m_name = value;
}

QString GeoDataFeature::address() const
{
    return m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    m_description = value;
}

bool GeoDataFeature::isVisible() const
{
    return m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    m_visible = value;
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << m_name;
    stream << m_address;
    stream << m_phoneNumber;
    stream << m_description;
    stream << m_visible;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> m_name;
    stream >> m_address;
    stream >> m_phoneNumber;
    stream >> m_description;
    stream >> m_visible;
}
