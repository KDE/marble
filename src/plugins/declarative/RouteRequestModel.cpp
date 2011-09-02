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

namespace Marble
{
namespace Declarative
{

RouteRequestModel::RouteRequestModel( Marble::RouteRequest* request, QObject *parent ) :
    QAbstractListModel( parent ), m_request( request )
{
    if ( m_request ) {
        connect( m_request, SIGNAL( positionChanged( int, GeoDataCoordinates ) ),
                 this, SLOT( updateData( int ) ) );
        connect( m_request, SIGNAL( positionAdded( int ) ),
                 this, SLOT( updateAfterRemoval( int ) ) );
        connect( m_request, SIGNAL( positionRemoved( int) ),
                 this, SLOT( updateAfterAddition( int ) ) );
    }
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
    if ( index.isValid() && role == Qt::DisplayRole && m_request ) {
        return m_request->name( index.row() );
    }

    return QVariant();
}

void RouteRequestModel::updateData( int )
{
    /** @todo: do that properly */
    reset();
}

void RouteRequestModel::updateAfterRemoval( int )
{
    /** @todo: do that properly */
    reset();
}

void RouteRequestModel::updateAfterAddition( int )
{
    /** @todo: do that properly */
    reset();
}

}
}

#include "RouteRequestModel.moc"
