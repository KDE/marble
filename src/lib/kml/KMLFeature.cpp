#include "KMLFeature.h"

KMLFeature::KMLFeature()
{
}

QString KMLFeature::name() const
{
    return m_name;
}

void KMLFeature::setName( QString value )
{
    m_name = value;
}

QString KMLFeature::address() const
{
    return m_address;
}

void KMLFeature::setAddress( QString value)
{
    m_address = value;
}

QString KMLFeature::phoneNumber() const
{
    return m_phoneNumber;
}

void KMLFeature::setPhoneNumber( QString value)
{
    m_phoneNumber = value;
}

QString KMLFeature::description() const
{
    return m_description;
}

void KMLFeature::setDescription( QString value)
{
    m_description = value;
}
