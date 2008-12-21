//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "AbstractTile.h"
#include "AbstractTile_p.h"

#include <QtCore/QDebug>


namespace Marble {

AbstractTilePrivate::AbstractTilePrivate( const TileId& id )
    : m_id( id ),
      m_used( false ),
      m_state( AbstractTile::TileEmpty ),
      m_created( QDateTime::currentDateTime() )
{
}

AbstractTilePrivate::~AbstractTilePrivate()
{
}

AbstractTile::AbstractTile( TileId const& id, QObject * parent )
    : QObject( parent ),
      d_ptr( new AbstractTilePrivate( id ) )
{
}

AbstractTile::AbstractTile( AbstractTilePrivate &dd, QObject *parent )
    : QObject( parent ), d_ptr( &dd )
{
}

AbstractTile::~AbstractTile()
{
    delete d_ptr;
}

TileId const& AbstractTile::id() const
{
    Q_D( const AbstractTile );
    return d->m_id;
}

bool AbstractTile::used() const
{
    Q_D( const AbstractTile );
    return d->m_used;
}

void AbstractTile::setUsed( bool used )
{
    Q_D( AbstractTile );
    d->m_used = used;
}

AbstractTile::TileState AbstractTile::state() const
{
    Q_D( const AbstractTile );
    return d->m_state;
}

void AbstractTile::setState( TileState state )
{
    Q_D( AbstractTile );
    d->m_state = state;
}

const QDateTime & AbstractTile::created() const
{
    Q_D( const AbstractTile );
    return d->m_created;
}

void AbstractTile::setCreated( QDateTime created )
{
    Q_D( AbstractTile );
    d->m_created = created;
}

}

#include "AbstractTile.moc"
