//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "GpsElement.h"



GpsElement::GpsElement(): m_name(0),        m_gpsComment(0),
                          m_description(0), m_source(0), m_links(0),
                          m_number(0)
{
}

GpsElement::~GpsElement()
{
    delete m_name;
    delete m_gpsComment;
    delete m_description;
    delete m_source;
    delete m_links;
    delete m_number;
}

void GpsElement::setName( const QString &name )
{
    m_name = new QString( name );
}

QString GpsElement::name()
{
    return QString( *m_name );
}

void GpsElement::setGpsComment( const QString &comment )
{
    m_gpsComment = new QString( comment );
}

QString GpsElement::gpsComment()
{
    return QString( *m_gpsComment );
}

void GpsElement::setDescription( const QString &description )
{
    m_description = new QString( description );
}

QString GpsElement::description()
{
    return QString( *m_description );
}

void GpsElement::setSource( const QString &source )
{
    m_source = new QString( source );
}

QString GpsElement::source()
{
    return QString( *m_source );
}

void GpsElement::addLink( const QString &link )
{
    if ( m_links == 0 ) {
        m_links = new QVector<QUrl>;
    }
    m_links -> append( QUrl ( link ) );
}

QVector<QUrl> GpsElement::links()
{
    return QVector<QUrl>( *m_links );
}

void GpsElement::setNumber( int number )
{
    m_number = new int;
    *m_number = number;
}

int GpsElement::number()
{
    return *m_number;
}
