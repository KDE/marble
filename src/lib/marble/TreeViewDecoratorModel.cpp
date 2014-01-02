//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Levente Kurusa <levex@linux.com>

#include "TreeViewDecoratorModel.h"
#include "MarbleDebug.h"
#include "GeoDataTypes.h"
#include "GeoDataObject.h"
#include "GeoDataContainer.h"
#include "GeoDataStyle.h"
#include "GeoDataTour.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{

TreeViewDecoratorModel::TreeViewDecoratorModel( QObject *parent ) :
    QSortFilterProxyModel( parent )
{
    // nothing to do
}

bool TreeViewDecoratorModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
    QModelIndex rowIndex = sourceModel()->index( sourceRow, 0, sourceParent );

    GeoDataObject* object = qvariant_cast<GeoDataObject*>( rowIndex.data( MarblePlacemarkModel::ObjectPointerRole ) );
    GeoDataObject* parent = object->parent();
    if ( parent->nodeType() == GeoDataTypes::GeoDataFolderType ||
         parent->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer *>( parent );
        if ( container->style()->listStyle().listItemType() == GeoDataListStyle::CheckHideChildren ) {
            return false;
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow( sourceRow, sourceParent );
}

}
