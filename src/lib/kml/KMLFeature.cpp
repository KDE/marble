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


KMLFeature::KMLFeature()
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
