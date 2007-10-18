//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLFeature.h"

#include <QtCore/QDataStream>


KMLFeature::KMLFeature() :
    m_visible( true )
{
}

KMLFeature::KMLFeature( const QString& name ) :
    m_name( name )
{
}

KMLFeature::~KMLFeature()
{
}

QString KMLFeature::name() const
{
    return m_name;
}

void KMLFeature::setName( const QString &value )
{
    m_name = value;
}

QString KMLFeature::address() const
{
    return m_address;
}

void KMLFeature::setAddress( const QString &value)
{
    m_address = value;
}

QString KMLFeature::phoneNumber() const
{
    return m_phoneNumber;
}

void KMLFeature::setPhoneNumber( const QString &value)
{
    m_phoneNumber = value;
}

QString KMLFeature::description() const
{
    return m_description;
}

void KMLFeature::setDescription( const QString &value)
{
    m_description = value;
}

bool KMLFeature::isVisible() const
{
    return m_visible;
}

void KMLFeature::setVisible( bool value )
{
    m_visible = value;
}

void KMLFeature::pack( QDataStream& stream ) const
{
    KMLObject::pack( stream );

    stream << m_name;
    stream << m_address;
    stream << m_phoneNumber;
    stream << m_description;
    stream << m_visible;
}

void KMLFeature::unpack( QDataStream& stream )
{
    KMLObject::unpack( stream );

    stream >> m_name;
    stream >> m_address;
    stream >> m_phoneNumber;
    stream >> m_description;
    stream >> m_visible;
}
