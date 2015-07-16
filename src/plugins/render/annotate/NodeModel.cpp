//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Mikhail Ivchenko  <ematirov@gmail.com>
//

#include "NodeModel.h"

#include "MarbleLocale.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

NodeModel::NodeModel( QObject *parent ) : QAbstractListModel( parent )
{

}

int NodeModel::rowCount( const QModelIndex &parent ) const
{
    if( parent == QModelIndex() ) {
        return m_nodes.size();
    } else {
        return 0;
    }
}

int NodeModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent )
    return 4;
}

QVariant NodeModel::data( const QModelIndex &index, int role ) const
{
    if( !index.isValid() ) {
        return QVariant();
    }
    if( role == Qt::DisplayRole ) {
        switch( index.column() ) {
        case 0:
            return index.row();
        case 1: {
            return m_nodes[ index.row() ].lonToString();
        }
        case 2: {
            return m_nodes[ index.row() ].latToString();
        }
        case 3: {
            qreal altitude = m_nodes[ index.row() ].altitude();
            MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
            qreal convertedAltitude;
            const MarbleLocale::MeasurementSystem currentSystem = locale->measurementSystem();
            MarbleLocale::MeasureUnit elevationUnit;
            switch ( locale->measurementSystem() ) {
            case MarbleLocale::MetricSystem:
                elevationUnit = MarbleLocale::Meter;
                break;
            case MarbleLocale::ImperialSystem:
                elevationUnit = MarbleLocale::Foot;
                break;
            case MarbleLocale::NauticalSystem:
                elevationUnit = MarbleLocale::NauticalMile;
                break;
            }
            locale->meterToTargetUnit( altitude, currentSystem, convertedAltitude, elevationUnit );
            return convertedAltitude;
        }
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant NodeModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        switch( section ) {
        case 0:
            return tr( "No." );
        case 1:
            return tr( "Longitude" );
        case 2:
            return tr( "Latitude" );
        case 3:
            return tr( "Elevation" );
        }
    }
    return QAbstractListModel::headerData( section, orientation, role );
}

void NodeModel::clear()
{
    int last = rowCount( QModelIndex() ) - 1;
    beginRemoveRows( QModelIndex(), 0, last );
    m_nodes.clear();
    endRemoveRows();
}

int NodeModel::addNode( const GeoDataCoordinates &node )
{
    int row = rowCount( QModelIndex() );
    beginInsertRows( QModelIndex(), row, row );
    m_nodes.append( node );
    endInsertRows();
    return row;
}

Qt::ItemFlags NodeModel::flags(const QModelIndex & index) const
{
    if ( index.column() == 1 || index.column() == 2 ) {
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

}

#include "moc_NodeModel.cpp"
