//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "RenderState.h"

#include <QList>

namespace Marble {

class Q_DECL_HIDDEN RenderState::Private
{
public:
    QString m_name;
    QList<RenderState> m_children;
    RenderStatus m_status;

    Private( const QString &name=QString(), RenderStatus status=Complete );
    RenderStatus status() const;
    RenderStatus minimumStatus( RenderStatus a, RenderStatus b ) const;
    QString toString( const RenderState &state, int indent ) const;
};

RenderState::RenderState( const QString &name, RenderStatus status ) :
    d( new Private( name, status ) )
{
    // nothing to do
}

RenderState::RenderState( const RenderState &other ) :
    d( new Private )
{
    *d = *other.d;
}

RenderState &RenderState::operator=( const RenderState &other )
{
    *d = *other.d;
    return *this;
}

RenderState::~RenderState()
{
    delete d;
}

RenderStatus RenderState::status() const
{
    return d->status();
}

QString RenderState::name() const
{
    return d->m_name;
}

int RenderState::children() const
{
    return d->m_children.size();
}

RenderState RenderState::childAt( int index ) const
{
    return d->m_children.at( index );
}

void RenderState::addChild(const RenderState &child)
{
    d->m_children.push_back( child );
}

RenderState::operator QString() const
{
  return d->toString( *this, 0 );
}

RenderState::Private::Private( const QString &name, RenderStatus status ) :
    m_name( name ),
    m_status( status )
{
    // nothing to do
}

RenderStatus RenderState::Private::status() const
{
    RenderStatus status = Complete;
    foreach( const RenderState &child, m_children ) {
        status = minimumStatus( status, child.status() );
    }
    return minimumStatus( status, m_status );
}

RenderStatus RenderState::Private::minimumStatus( RenderStatus a, RenderStatus b ) const
{
    if ( a == Incomplete || b == Incomplete ) {
        return Incomplete;
    }
    if ( a == WaitingForData || b == WaitingForData ) {
        return WaitingForData;
    }
    if ( a == WaitingForUpdate || b == WaitingForUpdate ) {
        return WaitingForUpdate;
    }

    Q_ASSERT( a == Complete || b == Complete );
    return Complete;
}

QString RenderState::Private::toString( const RenderState &state, int level ) const
{
    QString const prefix = level > 0 ? "\n" : "";
    QString const indent(level*2, QLatin1Char(' '));
    QString status;
    switch ( state.status() ) {
    case Marble::Complete:         status = "Complete"; break;
    case Marble::WaitingForUpdate: status = "Waiting for update"; break;
    case Marble::WaitingForData:   status = "Waiting for data"; break;
    case Marble::Incomplete:       status = "Incomplete"; break;
    }
    QString const name = ( state.name().isEmpty() ? "Anonymous renderer" : state.name() );
    QString result = QString("%1%2%3: %4").arg( prefix ).arg( indent ).arg( name ).arg( status );

    foreach( const RenderState &child, state.d->m_children ) {
        result += toString( child, level+1 );
    }
    return result;
}

}
