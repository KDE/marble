//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingProxyModel.h"

#include "RoutingModel.h"

namespace Marble
{

RoutingProxyModel::RoutingProxyModel( QObject *parent ) :
        QSortFilterProxyModel( parent )
{
    // nothing to do
}

bool RoutingProxyModel::filterAcceptsRow ( int source_row, const QModelIndex &source_parent ) const
{
    Q_UNUSED( source_parent )

    if ( !sourceModel() ) {
        return false;
    }

    QModelIndex index = sourceModel()->index( source_row, 0 );
    RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>( index.data( RoutingModel::TypeRole ) );
    return type == RoutingModel::Instruction || type == RoutingModel::Error;
}

} // namespace Marble
