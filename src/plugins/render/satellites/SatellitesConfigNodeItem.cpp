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

#include <QtCore/QStringList>
#include <QtCore/QVariant>

namespace Marble {

SatellitesConfigNodeItem::SatellitesConfigNodeItem( const QString &name )
    : SatellitesConfigAbstractItem( name )
{
}

SatellitesConfigNodeItem::~SatellitesConfigNodeItem()
{
}

void SatellitesConfigNodeItem::loadSettings(QHash< QString, QVariant > settings)
{
    foreach( SatellitesConfigAbstractItem *item, m_children ) {
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
    case UrlListRole: {
        QStringList urlList;
        foreach( SatellitesConfigAbstractItem *item, m_children ) {
            if ( item->data( column, Qt::CheckStateRole ).toInt() != Qt::Unchecked ) {
                urlList.append( item->data( column, role).toStringList() );
            }
        }
        return urlList;
    }
    case OrbiterDataListRole: {
        QStringList orbiterDataList;
        foreach( SatellitesConfigAbstractItem *item, m_children ) {
            if ( item->data( column, Qt::CheckStateRole ).toInt() != Qt::Unchecked ) {
                orbiterDataList.append( item->data( column, role ).toStringList() );
            }
        }
        return orbiterDataList;
    }
    case Qt::CheckStateRole: {
        bool oneChecked = false;
        bool oneUnchecked = false;
        foreach( SatellitesConfigAbstractItem *item, m_children ) {         
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
                break;
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
            foreach( SatellitesConfigAbstractItem *item, m_children ) {
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

} // namespace Marble

