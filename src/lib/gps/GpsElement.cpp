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

GpsElement::~GpsElement()
{
    delete m_name;
    delete m_gpsComment;
    delete m_description;
    delete m_source;
    delete m_link;
    delete m_number;
}

void GpsElement::setName( const QString &name )
{
    m_name = new QString( name );
}

void GpsElement::setGpsComment( const QString &comment )
{
    m_gpsComment = new QString( comment );
}

void GpsElement::setDescription( const QString &description )
{
    m_description = new QString( description );
}

void GpsElement::setSource( const QString &source )
{
    m_source = new QString( source );
}

void GpsElement::addLink( const QString &link )
{
    if ( m_link == 0 ) {
        m_link = new QVector<QUrl>;
    }
    m_link -> append( QUrl ( link ) );
}

void GpsElement::setNumber( int number )
{
    m_number = new int;
    *m_number = number;
}
