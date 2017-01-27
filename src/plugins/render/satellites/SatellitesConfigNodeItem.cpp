//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesConfigNodeItem.h"

#include <QStringList>
#include <QVariant>

#include "MarbleDebug.h"

namespace Marble {

SatellitesConfigNodeItem::SatellitesConfigNodeItem( const QString &name )
    : SatellitesConfigAbstractItem( name )
{
}

SatellitesConfigNodeItem::~SatellitesConfigNodeItem()
{
}

void SatellitesConfigNodeItem::loadSettings(const QHash<QString, QVariant> &settings)
{
    for( SatellitesConfigAbstractItem *item: m_children ) {
        item->loadSettings( settings );
    }
}

QVariant SatellitesConfigNodeItem::data( int column, int role ) const
{
    QVariant base = SatellitesConfigAbstractItem::data( column, role );
    if ( base.isValid() ) {
        return base;
    }

    switch ( role ) {
    case IdListRole:
    case UrlListRole: {
        QStringList list;
        for( SatellitesConfigAbstractItem *item: m_children ) {
            if ( item->data( column, Qt::CheckStateRole ).toInt() != Qt::Unchecked ) {
                list.append( item->data( column, role).toStringList() );
            }
        }
        return list;
    }
    case FullIdListRole: {
        QStringList fullIdList;
        for( SatellitesConfigAbstractItem *item: m_children ) {
            fullIdList.append( item->data( column, role ).toStringList() );
        }
        return fullIdList;
    }
    case Qt::CheckStateRole: {
        bool oneChecked = false;
        bool oneUnchecked = false;
        for( SatellitesConfigAbstractItem *item: m_children ) {         
            switch ( item->data( column, Qt::CheckStateRole ).toInt() ) {
            case Qt::Checked:
                oneChecked = true;
                if ( oneUnchecked ) {
                    return Qt::PartiallyChecked;
                }
                break;
            case Qt::PartiallyChecked:
                return Qt::PartiallyChecked;
            case Qt::Unchecked:
                oneUnchecked = true;
                if ( oneChecked ) {
                    return Qt::PartiallyChecked;
                }
            }
        }

        return QVariant( oneChecked ? Qt::Checked : Qt::Unchecked );
    }
    }

    return QVariant();
}

bool SatellitesConfigNodeItem::setData( int column, int role, const QVariant &data )
{
    if ( role == Qt::CheckStateRole ) {
        switch ( column ) {
        case 0:
            // fall through
        case 1:
            for( SatellitesConfigAbstractItem *item: m_children ) {
                item->setData( column, role, data );
            }
            return true;
        }
    }

    return false;
}

bool SatellitesConfigNodeItem::isLeaf() const
{
    return false;
}

SatellitesConfigAbstractItem *SatellitesConfigNodeItem::childAt( int row ) const
{
    if ( m_children.size() <= row ) {
        return 0;
    }

    return m_children.at( row );
}

int SatellitesConfigNodeItem::indexOf( const SatellitesConfigAbstractItem *child ) const
{
    //TODO: find out if we can avoid the const_cast
    return m_children.indexOf( const_cast<SatellitesConfigAbstractItem *>( child ) );
}

int SatellitesConfigNodeItem::childrenCount() const
{
    return m_children.size();
}

void SatellitesConfigNodeItem::appendChild( SatellitesConfigAbstractItem *item )
{
    item->setParent( this );
    m_children.append( item );
}

void SatellitesConfigNodeItem::clear()
{
    for( int i = childrenCount(); i > 0; --i ) {
        SatellitesConfigAbstractItem *item = m_children.at( i - 1 );
        item->clear();
        m_children.remove( i - 1 );
        delete item;
    }
}

} // namespace Marble

