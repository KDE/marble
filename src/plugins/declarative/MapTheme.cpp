//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MapTheme.h"

namespace Marble
{
namespace Declarative
{

MapTheme::MapTheme( const QString &id, const QString &name, QObject *parent ) :
    QObject( parent ), m_id( id ), m_name( name )
{
    // nothing to do
}

QString MapTheme::id() const
{
    return m_id;
}

void MapTheme::setId( const QString &id )
{
    m_id = id;
    emit idChanged();
}

QString MapTheme::name() const
{
    return m_name;
}

void MapTheme::setName( const QString &name )
{
    m_name = name;
    emit nameChanged();
}

}
}

#include "MapTheme.moc"
