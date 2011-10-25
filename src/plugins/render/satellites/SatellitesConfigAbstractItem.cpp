//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesConfigAbstractItem.h"

#include <QtCore/QVariant>

using namespace Marble;

SatellitesConfigAbstractItem::SatellitesConfigAbstractItem( const QString &name )
    : m_name( name ),
      m_parent( 0 )
{
}

SatellitesConfigAbstractItem::~SatellitesConfigAbstractItem()
{
}

QString SatellitesConfigAbstractItem::name() const
{
    return m_name;
}

int SatellitesConfigAbstractItem::row() const
{
    if ( m_parent != 0 ) {
        return m_parent->indexOf( this );
    }

    return 0;
}

SatellitesConfigAbstractItem *SatellitesConfigAbstractItem::parent() const
{
    return m_parent;
}

void SatellitesConfigAbstractItem::setParent( SatellitesConfigAbstractItem *parent )
{
    m_parent = parent;
}

void SatellitesConfigAbstractItem::loadSettings( QHash<QString, QVariant> settings )
{
}

QVariant SatellitesConfigAbstractItem::data( int column, int role ) const
{
    if ( column != 0 ) {
        return QVariant();
    }

    switch ( role ) {
        case Qt::DisplayRole:
            return QVariant( name() );
        default:
            return QVariant();
    }
}

bool SatellitesConfigAbstractItem::setData( int column, int role, const QVariant &data )
{
    return false;
}
