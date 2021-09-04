// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesConfigAbstractItem.h"

#include "MarbleDebug.h"

#include <QVariant>

namespace Marble {

SatellitesConfigAbstractItem::SatellitesConfigAbstractItem( const QString &name )
    : m_name( name ),
      m_parent( nullptr ),
      m_flags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable )
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
    if ( m_parent != nullptr ) {
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

void SatellitesConfigAbstractItem::loadSettings(const QHash<QString, QVariant> &settings)
{
    Q_UNUSED( settings );
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
    Q_UNUSED( column );
    Q_UNUSED( role );
    Q_UNUSED( data );
    return false;
}

Qt::ItemFlags SatellitesConfigAbstractItem::flags() const
{
    return m_flags;
}

void SatellitesConfigAbstractItem::setFlags( Qt::ItemFlags flags )
{
    m_flags = flags;
}

void SatellitesConfigAbstractItem::clear()
{
}

} // namespace Marble

