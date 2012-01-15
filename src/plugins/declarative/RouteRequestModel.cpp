//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RouteRequestModel.h"

#include "routing/RouteRequest.h"

RouteRequestModel::RouteRequestModel( Marble::RouteRequest* request, QObject *parent ) :
    QAbstractListModel( parent ), m_request( request )
{
    if ( m_request ) {
        connect( m_request, SIGNAL( positionChanged( int, GeoDataCoordinates ) ),
                 this, SLOT( updateData( int ) ) );
        connect( m_request, SIGNAL( positionAdded( int ) ),
                 this, SLOT( updateAfterAddition( int ) ) );
        connect( m_request, SIGNAL( positionRemoved( int) ),
                 this, SLOT( updateAfterRemoval( int ) ) );
    }

    QHash<int,QByteArray> roles = roleNames();
    roles[LongitudeRole] = "longitude";
    roles[LatitudeRole] = "latitude";
    setRoleNames( roles );
}

RouteRequestModel::~RouteRequestModel()
{
    // nothing to do
}

int RouteRequestModel::rowCount ( const QModelIndex &parent ) const
{
    if ( !parent.isValid() && m_request ) {
        return m_request->size();
    }

    return 0;
}

QVariant RouteRequestModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0 ) {
        return QString( "Waypoint" );
    }

    return QVariant();
}

QVariant RouteRequestModel::data ( const QModelIndex &index, int role ) const
{
    if ( index.isValid() && m_request && index.row() >= 0 && index.row() < m_request->size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return m_request->name( index.row() );
        case LongitudeRole: return m_request->at( index.row() ).longitude( Marble::GeoDataCoordinates::Degree );
        case LatitudeRole: return m_request->at( index.row() ).latitude( Marble::GeoDataCoordinates::Degree );
        }
    }

    return QVariant();
}

void RouteRequestModel::updateData( int idx )
{
    QModelIndex affected = index( idx );
    emit dataChanged( affected, affected );
}

void RouteRequestModel::updateAfterRemoval( int idx )
{
    beginRemoveRows( QModelIndex(), idx, idx );
    removeRow( idx );
    endRemoveRows();
}

void RouteRequestModel::updateAfterAddition( int idx )
{
    beginInsertRows( QModelIndex(), idx, idx );
    insertRow( idx );
    endInsertRows();
}

void RouteRequestModel::setPosition ( int index, qreal longitude, qreal latitude )
{
    if ( index >= 0 && index < m_request->size() ) {
        m_request->setPosition( index, Marble::GeoDataCoordinates( longitude, latitude, 0.0, Marble::GeoDataCoordinates::Degree ) );
    }
}

#include "RouteRequestModel.moc"
