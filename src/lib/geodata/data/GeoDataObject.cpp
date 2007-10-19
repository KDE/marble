//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataObject.h"

#include <QtGlobal>
#include <QtCore/QDataStream>

#ifdef GeoData_DEBUG
namespace
{
    // This variable used to count number of allocated
    // kml objects. Debug only
    int refCount = 0;
}
#endif

GeoDataObject::GeoDataObject()
  : m_id(0),
    m_targetId(0)
{
    #ifdef GeoData_DEBUG
        ++refCount;
        qDebug("GeoDataObject count: %d", refCount);
    #endif
}

GeoDataObject::~GeoDataObject()
{
    #ifdef GeoData_DEBUG
        --refCount;
        qDebug("GeoDataObject count: %d", refCount);
    #endif
}

int GeoDataObject::id() const
{
    return m_id;
}

void GeoDataObject::setId( int value )
{
    m_id = value;
}

int GeoDataObject::targetId() const
{
    return m_targetId;
}

void GeoDataObject::setTargetId( int value )
{
    m_targetId = value;
}

void GeoDataObject::pack( QDataStream& stream ) const
{
    stream << m_id;
    stream << m_targetId;
}

void GeoDataObject::unpack( QDataStream& stream )
{
    stream >> m_id;
    stream >> m_targetId;
}
