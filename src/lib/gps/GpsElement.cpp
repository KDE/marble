//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "GpsElement.h"

using namespace Marble;

GpsElement::GpsElement()
    : m_number(0)
{
}

GpsElement::~GpsElement()
{
}

void GpsElement::setName( const QString &name )
{
    m_name = name;
}

QString GpsElement::name() const
{
    return m_name;
}

void GpsElement::setGpsComment( const QString &comment )
{
    m_gpsComment = comment;
}

QString GpsElement::gpsComment() const
{
    return m_gpsComment;
}

void GpsElement::setDescription( const QString &description )
{
    m_description = description;
}

QString GpsElement::description() const
{
    return m_description;
}

void GpsElement::setSource( const QString &source )
{
    m_source = source;
}

QString GpsElement::source() const
{
    return m_source;
}

void GpsElement::addLink( const QString &link )
{
    m_links.append( QUrl( link ) );
}

QVector<QUrl> GpsElement::links() const
{
    return m_links;
}

void GpsElement::setNumber( int number )
{
    m_number = number;
}

int GpsElement::number() const
{
    return m_number;
}
