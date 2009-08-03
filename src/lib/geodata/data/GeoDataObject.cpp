//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//


#include "GeoDataDocument.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>

namespace Marble
{

class GeoDataObjectPrivate
{
  public:
    GeoDataObjectPrivate()
        : m_id(0),
          m_targetId(0)
    {
    }

    virtual QString nodeType() const
    {
        return GeoDataTypes::GeoDataObjectType;
    }

    int  m_id;
    int  m_targetId;
};

GeoDataObject::GeoDataObject()
    : GeoNode(), Serializable(),
      d( new GeoDataObjectPrivate() )
{
}

GeoDataObject::GeoDataObject( GeoDataObject const & other )
    : Serializable( other ),
      d( new GeoDataObjectPrivate( *other.d ) )
{
}

GeoDataObject & GeoDataObject::operator=( const GeoDataObject & rhs )
{
    *d = *rhs.d;
    return *this;
}

GeoDataObject::~GeoDataObject()
{
    delete d;
}

QString GeoDataObject::nodeType() const
{
    return d->nodeType();
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

GeoDataObject* GeoDataObject::child(int /* pos */ )
{
    return 0;
}

int GeoDataObject::row()
{
    return 0;
}

int GeoDataObject::childPosition( GeoDataObject *child )
{
    Q_UNUSED( child );
    return 0;
}

int GeoDataObject::childCount()
{
    return 0;
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

}
