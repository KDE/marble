//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLObject.h"

#include <QtGlobal>

namespace
{
    // This variable used to cound number of allocated
    // kml objects. Debug only
    int refCount = 0;
}

KMLObject::KMLObject()
  : m_id(0),
    m_targetId(0)
{
    #ifdef KML_DEBUG
        ++refCount;
        qDebug("KMLObject count: %d", refCount);
    #endif
}

KMLObject::~KMLObject()
{
    #ifdef KML_DEBUG
        --refCount;
        qDebug("KMLObject count: %d", refCount);
    #endif
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
