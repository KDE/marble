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
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "Routing.h"
#include <GeoDataPlacemark.h>

RouteRequestModel::RouteRequestModel( QObject *parent ) :
    QAbstractListModel( parent ),
    m_request( 0 ),
    m_routing( 0 )
{
    QHash<int,QByteArray> roles;
    roles[Qt::DisplayRole] = "name";
    roles[LongitudeRole] = "longitude";
    roles[LatitudeRole] = "latitude";
    m_roleNames = roles;
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

QHash<int, QByteArray> RouteRequestModel::roleNames() const
{
    return m_roleNames;
}

QVariant RouteRequestModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0 ) {
        return QStringLiteral("Waypoint");
    }

    return QVariant();
}

QVariant RouteRequestModel::data ( const QModelIndex &index, int role ) const
{
    if ( index.isValid() && m_request && index.row() >= 0 && index.row() < m_request->size() ) {
        switch ( role ) {
        case Qt::DisplayRole: {
            Marble::GeoDataPlacemark const & placemark = (*m_request)[index.row()];
            if (!placemark.name().isEmpty()) {
                return placemark.name();
            }

            if (!placemark.address().isEmpty()) {
                return placemark.address();
            }

            return placemark.coordinate().toString(Marble::GeoDataCoordinates::Decimal).trimmed();
        }
        case LongitudeRole: return m_request->at( index.row() ).longitude( Marble::GeoDataCoordinates::Degree );
        case LatitudeRole: return m_request->at( index.row() ).latitude( Marble::GeoDataCoordinates::Degree );
        }
    }

    return QVariant();
}

Marble::Routing *RouteRequestModel::routing()
{
    return m_routing;
}

void RouteRequestModel::setRouting( Marble::Routing *routing )
{
    if ( routing != m_routing ) {
        m_routing = routing;
        updateMap();
        connect( m_routing, SIGNAL(marbleMapChanged()), this, SLOT(updateMap()) );
        emit routingChanged();
    }
}

void RouteRequestModel::updateMap()
{
    if ( m_routing && m_routing->marbleMap() ) {
        m_request = m_routing->marbleMap()->model()->routingManager()->routeRequest();

        connect( m_request, SIGNAL(positionChanged(int,GeoDataCoordinates)),
                 this, SLOT(updateData(int)),  Qt::UniqueConnection );
        connect( m_request, SIGNAL(positionAdded(int)),
                 this, SLOT(updateAfterAddition(int)),  Qt::UniqueConnection );
        connect( m_request, SIGNAL(positionRemoved(int)),
                 this, SLOT(updateAfterRemoval(int)),  Qt::UniqueConnection );

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
    emit rowCountChanged();
}

void RouteRequestModel::updateAfterAddition( int idx )
{
    beginInsertRows( QModelIndex(), idx, idx );
    insertRow( idx );
    endInsertRows();
    emit rowCountChanged();
}

void RouteRequestModel::setPosition ( int index, qreal longitude, qreal latitude )
{
    if ( index >= 0 && index < m_request->size() ) {
        m_request->setPosition( index, Marble::GeoDataCoordinates( longitude, latitude, 0.0, Marble::GeoDataCoordinates::Degree ) );
    }
}

#include "moc_RouteRequestModel.cpp"
