//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesConfigLeafItem.h"

#include <QVariant>

namespace Marble {

SatellitesConfigLeafItem::SatellitesConfigLeafItem( const QString &name, const QString &id )
    : SatellitesConfigAbstractItem( name ),
      m_id( id ),
      m_url( QString() ),
      m_isChecked( false ),
      m_isOrbitDisplayed( false )
{ 
}

SatellitesConfigLeafItem::~SatellitesConfigLeafItem()
{
}

void SatellitesConfigLeafItem::loadSettings(const QHash<QString, QVariant> &settings)
{
    const QStringList idList = settings.value(QStringLiteral("idList")).toStringList();
    m_isChecked = idList.contains( m_id );
}

QVariant SatellitesConfigLeafItem::data( int column, int role ) const
{
    QVariant base = SatellitesConfigAbstractItem::data( column, role );
    if ( base.isValid() ) {
        return base;
    }

    switch ( role ) {
    case UrlListRole:
        if( !m_url.isNull() && !m_url.isEmpty() ) {
            return QVariant( QStringList() << m_url );
        }
        break;
    case IdListRole:
    case FullIdListRole:
        return QVariant( QStringList() << m_id );
    case Qt::CheckStateRole:
        switch ( column ) {
        case 0:
            return QVariant( m_isChecked ? Qt::Checked : Qt::Unchecked );
        case 1:
            return QVariant( m_isOrbitDisplayed ? Qt::Checked : Qt::Unchecked );
        }
    }

    return QVariant();
}

bool SatellitesConfigLeafItem::setData( int column, int role, const QVariant& data )
{
    switch( role ) {
    case UrlListRole:
        m_url = data.toString();
        return true;
    case Qt::CheckStateRole:
        switch ( column ) {
        case 0:
            m_isChecked = data.toBool();
            return true;
        case 1:
            m_isOrbitDisplayed = data.toBool();
            return true;
        }
    }

    return false;
}

bool SatellitesConfigLeafItem::isLeaf() const
{
    return true;
}

SatellitesConfigAbstractItem *SatellitesConfigLeafItem::childAt( int row ) const
{
    Q_UNUSED( row )
    return 0;
}

int SatellitesConfigLeafItem::indexOf( const SatellitesConfigAbstractItem *child ) const
{
    Q_UNUSED( child )
    return -1;
}

int SatellitesConfigLeafItem::childrenCount() const
{
    return 0;
}

QString SatellitesConfigLeafItem::id() const
{
    return m_id;
}

QString SatellitesConfigLeafItem::url() const
{
    return m_url;
}

} // namespace Marble

