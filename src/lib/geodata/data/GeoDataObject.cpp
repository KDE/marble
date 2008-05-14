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

#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>

class GeoDataObjectPrivate
{
  public:
    GeoDataObjectPrivate()
        : m_id(0),
          m_targetId(0)
    {
    }

    ~GeoDataObjectPrivate()
    {
    }

    int  m_id;
    int  m_targetId;
};

GeoDataObject::GeoDataObject()
    : d( new GeoDataObjectPrivate )
{
}


GeoDataObject::~GeoDataObject()
{
    delete d;
}

int GeoDataObject::id() const
{
    return d->m_id;
}

void GeoDataObject::setId( int value )
{
    d->m_id = value;
}

int GeoDataObject::targetId() const
{
    return d->m_targetId;
}

void GeoDataObject::setTargetId( int value )
{
    d->m_targetId = value;
}

void GeoDataObject::pack( QDataStream& stream ) const
{
    stream << d->m_id;
    stream << d->m_targetId;
}

void GeoDataObject::unpack( QDataStream& stream )
{
    stream >> d->m_id;
    stream >> d->m_targetId;
}
