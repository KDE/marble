//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <x@y>
//


#include "KMLObject.h"

KMLObject::KMLObject()
  : m_id(0),
    m_targetId(0)
{
}

int KMLObject::id() const
{
    return m_id;
}

void KMLObject::setId( int value )
{
    m_id = value;
}

int KMLObject::targetId() const
{
    return m_targetId;
}

void KMLObject::setTargetId( int value )
{
    m_targetId = value;
}
