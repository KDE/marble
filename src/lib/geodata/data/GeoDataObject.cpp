//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "GeoDataObject.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>

#include "GeoDataDocument.h"

#include "GeoDataTypes.h"


namespace Marble
{

class GeoDataObjectPrivate
{
  public:
    GeoDataObjectPrivate()
        : m_id(0),
          m_targetId(0),
          m_parent(0)
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataObjectType;
    }

    int  m_id;
    int  m_targetId;
    GeoDataObject *m_parent;
};

GeoDataObject::GeoDataObject()
    : GeoNode(), Serializable(),
      d( new GeoDataObjectPrivate() )
{
}

GeoDataObject::GeoDataObject( GeoDataObject const & other )
    : GeoNode(), Serializable( other ),
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

const char* GeoDataObject::nodeType() const
{
    return d->nodeType();
}

GeoDataObject *GeoDataObject::parent() const
{
    return d->m_parent;
}

void GeoDataObject::setParent(GeoDataObject *parent)
{
    d->m_parent = parent;
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

QString GeoDataObject::resolvePath( const QString &relativePath ) const
{
    QUrl const url( relativePath );
    QFileInfo const fileInfo( url.path() );
    if ( url.isRelative() && fileInfo.isRelative() ) {
        GeoDataDocument const * document = dynamic_cast<GeoDataDocument const*>( this );
        if ( document ) {
            QFileInfo const documentFile = document->fileName();
            QFileInfo const absoluteImage( documentFile.absolutePath() + '/' + url.path() );
            return absoluteImage.absoluteFilePath();
        } else if ( d->m_parent ) {
            return d->m_parent->resolvePath( relativePath );
        }
    }

    return relativePath;
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
