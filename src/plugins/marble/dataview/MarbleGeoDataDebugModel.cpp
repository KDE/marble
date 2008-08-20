//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Simon Schmeisser <mail_to_wrt@gmx.de>
//


// Own
#include "MarbleGeoDataDebugModel.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataDocument.h"       // In geodata/data/
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

using namespace Marble;

class MarbleGeoDataDebugModel::Private {
 public:
    Private(GeoDataDocument* rootDocument) : m_rootDocument( rootDocument ) {};
    GeoDataDocument* m_rootDocument;
    //QVector<GeoDataDocument*> m_documents;
};

MarbleGeoDataDebugModel::MarbleGeoDataDebugModel( GeoDataDocument *rootDocument, QObject *parent )
    : QAbstractItemModel(), d( new Private( rootDocument ) )
{
}

MarbleGeoDataDebugModel::~MarbleGeoDataDebugModel()
{
    //delete d->m_rootDocument;
    delete d;
}

int MarbleGeoDataDebugModel::rowCount( const QModelIndex &parent ) const
{
    GeoDataObject *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = d->m_rootDocument;
    else
        parentItem = static_cast<GeoDataObject*>(parent.internalPointer());

    return parentItem->childCount();
}


QVariant MarbleGeoDataDebugModel::data( const QModelIndex &index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    GeoDataObject *item = static_cast<GeoDataObject*>(index.internalPointer());

    if( GeoDataFeature* feature = dynamic_cast<GeoDataFeature*>( item ) ){
        switch( feature->featureId() ) {
            case InvalidFeatureId:
                return QVariant("InvalidFeature");
            case GeoDataDocumentId:
                return QVariant("GeoDataDocument");
            case GeoDataFolderId:
                return QVariant("GeoDataFolder");
            case GeoDataPlacemarkId:
                return QVariant("GeoDataPlacemark");
            case GeoDataNetworkLinkId:
                return QVariant("GeoDataNetworkLink");
            case GeoDataScreenOverlayId:
                return QVariant("GeoDataScreenOverlay");
            case GeoDataGroundOverlayId:
                return QVariant("GeoDataGroundOverlay");
           }
    }
    
    return QVariant("GeoDataObject"); 
    // TODO the data is hardcoded and very limited right now,
    // the logic what data is returned might be moved to the items
    // themselves later. Use the following code then:
    //return item->data(index.column());

}

QModelIndex MarbleGeoDataDebugModel::index(int row, int column, const QModelIndex &parent)             const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    GeoDataObject *parentItem;
    
    if (!parent.isValid())
        parentItem = d->m_rootDocument;
    else
        parentItem = static_cast<GeoDataObject*>(parent.internalPointer());

    GeoDataObject *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex MarbleGeoDataDebugModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GeoDataObject *childItem = static_cast<GeoDataObject*>(index.internalPointer());
    GeoDataObject *parentItem = childItem->parent();

    if (parentItem == d->m_rootDocument)
        return QModelIndex();

    if(parentItem == 0){
        // this shouldn't happen
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int MarbleGeoDataDebugModel::columnCount(const QModelIndex &node) const
{
    if (node.isValid())
        return 1;
        // TODO when nodes handle their information themselves they will have
        // varying amounts of properties (or columns ). Right now we only have
        // one information (type ) per node, so it's hardcoded to "1".
        //static_cast<GeoDataObject*>(node.internalPointer())->columnCount();
    else
        return 1;
        // the rootItem has an invalid QModelIndex to mark it as the root item.
        // so we ask it directly
        //rootItem->columnCount();
}

GeoDataDocument* MarbleGeoDataDebugModel::geoDataRoot()
{
    return d->m_rootDocument;
}

void MarbleGeoDataDebugModel::update()
{
    QAbstractItemModel::reset();
}

#include "MarbleGeoDataDebugModel.moc"
