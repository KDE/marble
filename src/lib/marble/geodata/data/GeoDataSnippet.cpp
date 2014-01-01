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

GeoDataSnippet::GeoDataSnippet()
{
    this->m_text = QString();
    this->m_maxLines = 0;
}

GeoDataSnippet::GeoDataSnippet( const QString &text , const int maxLines )
{
    this->m_text = text;
    this->m_maxLines = maxLines;
}

int GeoDataSnippet::maxLines() const
{
	return this->m_maxLines;
}

void GeoDataSnippet::setMaxLines( const int lines )
{
	this->m_maxLines = lines;
}

QString GeoDataSnippet::text() const
{
	return this->m_text;
}

void GeoDataSnippet::setText( const QString &text )
{
	this->m_text = text;
}

GeoDataSnippet::~GeoDataSnippet()
{
}

}

