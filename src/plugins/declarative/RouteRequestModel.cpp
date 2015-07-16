//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RouteRequestModel.h"

#include "routing/RoutingManager.h"
#include "routing/RouteRequest.h"
#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "Routing.h"

RouteRequestModel::RouteRequestModel( QObject *parent ) :
    QAbstractListModel( parent ),
    m_request( 0 ),
    m_routing( 0 )
{
    QHash<int,QByteArray> roles;
    roles[LongitudeRole] = "longitude";
    roles[LatitudeRole] = "latitude";
#if QT_VERSION < 0x050000
    setRoleNames( roles );
#else
    m_roleNames = roles;
#endif
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

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> RouteRequestModel::roleNames() const
{
    return m_roleNames;
}
#endif

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

Routing *RouteRequestModel::routing()
{
    return m_routing;
}

void RouteRequestModel::setRouting( Routing *routing )
{
    if ( routing != m_routing ) {
        m_routing = routing;
        updateMap();
        connect( m_routing, SIGNAL(mapChanged()), this, SLOT(updateMap()) );
        emit routingChanged();
    }
}

void RouteRequestModel::updateMap()
{
    if ( m_routing && m_routing->map() ) {
        m_request = m_routing->map()->model()->routingManager()->routeRequest();

        connect( m_request, SIGNAL(positionChanged(int,GeoDataCoordinates)),
                 this, SLOT(updateData(int)) );
        connect( m_request, SIGNAL(positionAdded(int)),
                 this, SLOT(updateAfterAddition(int)) );
        connect( m_request, SIGNAL(positionRemoved(int)),
                 this, SLOT(updateAfterRemoval(int)) );

        emit layoutChanged();
    }
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

#include "moc_RouteRequestModel.cpp"
