//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavMapsModel.h"

namespace Marble
{

void MonavMapsModel::deleteMapFiles( int row )
{
    if ( row >= 0 && row < m_data.size() ) {
        m_data.at( row ).remove();
        beginRemoveRows( QModelIndex(), row, row );
        m_data.remove( row );
        endRemoveRows();
    }
}

void MonavMapsModel::setInstallableVersions( const QMap<QString, QString> remoteMaps )
{
    m_remoteMaps = remoteMaps;
    reset();
}

MonavMapsModel::MonavMapsModel( const QVector<MonavMap> &data, QObject * parent ) :
    QAbstractTableModel( parent ), m_data( data )
{
    qSort( m_data.begin(), m_data.end(), &MonavMap::nameLessThan );
}

QVariant MonavMapsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
        switch( section ) {
        case 0:
            return tr( "Name" );
        case 1:
            return tr( "Transport" );
        case 2:
            return tr( "Size" );
        case 3:
            return tr( "Update" );
        case 4:
            return tr( "Delete" );
        }
    }

    return QVariant();
}

QVariant MonavMapsModel::data ( const QModelIndex & index, int role ) const
{
    if ( index.isValid() && role == Qt::DisplayRole ) {
        int const row = index.row();
        if ( row >= 0 && row < m_data.size() )
            switch( index.column() ) {
            case 0:
                return m_data.at( row ).name();
            case 1:
                return m_data.at( row ).transport();
            case 2:
                return QString( "%1 MB" ).arg( 1 + m_data.at( row ).size() / 1024 / 1024 );
            case 3: {
                QString payload = m_data.at( row ).payload();
                payload = payload.mid( payload.lastIndexOf( "/" ) + 1 );
                if ( m_remoteMaps.contains( payload ) ) {
                    return m_remoteMaps[payload] > m_data.at( row ).date();
                }

                return false;
            }
            case 4:
                return m_data.at( row ).date();
            }
    }

    return QVariant();
}

int MonavMapsModel::rowCount ( const QModelIndex & parent ) const
{
    return parent.isValid() ? 0 : m_data.size();
}

int MonavMapsModel::columnCount ( const QModelIndex & parent ) const
{
    return parent.isValid() ? 0 : 5;
}

QString MonavMapsModel::payload( int index ) const
{
    if ( index >= 0 && index <= m_data.size() ) {
        return m_data.at( index ).payload();
    }

    return QString();
}

}
