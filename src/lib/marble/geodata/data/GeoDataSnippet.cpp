//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Levente Kurusa <levex@linux.com>
//

#include "GeoDataSnippet.h"

namespace Marble
{

GeoDataSnippet::GeoDataSnippet(const QString &text , int maxLines ) :
    m_text( text ),
    m_maxLines( maxLines )
{
    // nothing to do
}

bool GeoDataSnippet::operator==( const GeoDataSnippet &other ) const
{
    return m_text == other.m_text && m_maxLines == other.m_maxLines;
}

bool GeoDataSnippet::operator!=( const GeoDataSnippet &other ) const
{
    return !this->operator==( other );
}

int GeoDataSnippet::maxLines() const
{
    return m_maxLines;
}

void GeoDataSnippet::setMaxLines( int lines )
{
    m_maxLines = lines;
}

QString GeoDataSnippet::text() const
{
    return m_text;
}

void GeoDataSnippet::setText( const QString &text )
{
    m_text = text;
}

}

