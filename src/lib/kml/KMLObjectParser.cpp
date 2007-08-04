//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLObjectParser.h"
#include "KMLObject.h"

KMLObjectParser::KMLObjectParser( KMLObject& object )
  : m_object( object ),
    m_parsed( false ),
    m_level( 0 )
{
}

KMLObjectParser::~KMLObjectParser()
{
}

bool KMLObjectParser::isParsed() const
{
    return m_parsed;
}
