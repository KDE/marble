//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Thibaut Gridel <tgridel@free.fr>
//


// Own
#include "GeoDataTreeModel.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataDocument.h"
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataTypes.h"
#include "FileManager.h"
#include "KmlFileViewItem.h"

using namespace Marble;

class GeoDataTreeModel::Private {
 public:
    Private() : m_rootDocument( new GeoDataDocument ) {}
    GeoDataDocument* m_rootDocument;
    FileManager     *m_fileManager;
};

GeoDataTreeModel::GeoDataTreeModel( QObject *parent )
    : QAbstractItemModel( parent ),
    d( new Private )
{
}

GeoDataTreeModel::~GeoDataTreeModel()
{
    delete d;
}

int GeoDataTreeModel::rowCount( const QModelIndex &parent ) const
{
//    qDebug() << "rowCount";
    GeoDataObject *parentItem;
    if ( parent.column() > 0 ) {
//        qDebug() << "rowCount bad column";
        return 0;
    }

    if ( !parent.isValid() ) {
//        qDebug() << "rowCount root parent";
        parentItem = d->m_rootDocument;
    } else {
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );
    }

    if ( !parentItem ) {
//        qDebug() << "rowCount bad parent";
        return 0;
    }

    QString type = parentItem->nodeType();
    if ( type == GeoDataTypes::GeoDataContainerType
        || type == GeoDataTypes::GeoDataDocumentType
        || type == GeoDataTypes::GeoDataFolderType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
        if ( container ) {
//            qDebug() << "rowCount " << type << "(" << parentItem << ") =" << container->size();
            return container->size();
        } else {
//            qDebug() << "rowCount bad container " << container;
        }
    }

    if ( type == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        if ( placemark ) {
            if ( placemark->geometry() ) {
//                qDebug() << "rowCount " << type << "(" << parentItem << ") = 1";
                return 1;
            }
//            qDebug() << "rowCount " << type << "(" << parentItem << ") = 0";
            return 0;
        }
    }

    if ( type == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
        if ( geometry ) {
//            qDebug() << "rowCount " << parent << " " << type << " " << geometry->size();
            return geometry->size();
        } else {
//            qDebug() << "rowCount bad geometry " << geometry;
        }
    }

//    qDebug() << "rowcount end";
    return 0;//parentItem->childCount();
}


QVariant GeoDataTreeModel::data( const QModelIndex &index, int role ) const
{
//    qDebug() << "data";
    if ( !index.isValid() )
        return QVariant();

    if ( role != Qt::DisplayRole )
        return QVariant();

    GeoDataFeature *feature = static_cast<GeoDataFeature*>( index.internalPointer() );
    if ( feature )
        return QVariant( feature->nodeType() );

    GeoDataGeometry *geometry = static_cast<GeoDataGeometry*>( index.internalPointer() );
    if ( geometry )
        return QVariant( geometry->nodeType() );

    GeoDataObject *item = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( item )
        return QVariant( item->nodeType() );

    return QVariant();
}

QModelIndex GeoDataTreeModel::index( int row, int column, const QModelIndex &parent )             const
{
//    qDebug() << "index";
    if ( !hasIndex( row, column, parent ) ) {
//        qDebug() << "index bad index";
        return QModelIndex();
    }
    
    GeoDataObject *parentItem;
    
    if ( !parent.isValid() )
        parentItem = d->m_rootDocument;
    else
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );

    if ( !parentItem ) {
//        qDebug() << "index bad parent";
        return QModelIndex();
    }

    GeoDataObject *childItem = 0;

    QString type = parentItem->nodeType();
    if ( type == GeoDataTypes::GeoDataContainerType
        || type == GeoDataTypes::GeoDataDocumentType
        || type == GeoDataTypes::GeoDataFolderType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
        childItem = container->child( row );
    }

    if ( type == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        childItem = placemark->geometry();
        // TODO: this parenting is needed because the parser doesn't use
        // the pointer-based API. This should happen there.
        if (childItem->parent() != placemark) {
            childItem->setParent(placemark);
        }
    }

    if ( type == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
        childItem = geometry->child( row );
        // TODO: this parenting is needed because the parser doesn't use
        // the pointer-based API. This should happen there.
        if (childItem->parent() != geometry) {
            childItem->setParent(geometry);
        }
    }


    if ( childItem ) {
//        qDebug() << "index " << type << "[" << row << "](" << parentItem << ") ="
//                << childItem->nodeType() << "(" << childItem << ")";
        return createIndex( row, column, childItem );
    }
    return QModelIndex();
}

QModelIndex GeoDataTreeModel::parent( const QModelIndex &index ) const
{
//    qDebug() << "parent";
    if ( !index.isValid() ) {
//        qDebug() << "parent bad index";
        return QModelIndex();
    }


    GeoDataObject *childObject = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( childObject ) {

        /// parentObject can be a container, placemark or multigeometry
        GeoDataObject *parentObject = childObject->parent();
        if ( parentObject == d->m_rootDocument )
        {
            return QModelIndex();
        }

        GeoDataObject *greatParentObject = parentObject->parent();
        QString greatParentType = greatParentObject->nodeType();

        // greatParent can be a container
        if ( greatParentType == GeoDataTypes::GeoDataContainerType
             || greatParentType == GeoDataTypes::GeoDataDocumentType
             || greatParentType == GeoDataTypes::GeoDataFolderType ) {
            GeoDataFeature *parentFeature = static_cast<GeoDataFeature*>( parentObject );
            GeoDataContainer * greatparentContainer = static_cast<GeoDataContainer*>( greatParentObject );
//            qDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                    << parentObject->nodeType() << "[" << greatparentContainer->childPosition( parentFeature ) << "](" << parentObject << ")";
            return createIndex( greatparentContainer->childPosition( parentFeature ), 0, parentObject );
        }

        // greatParent can be a placemark
        if ( greatParentType == GeoDataTypes::GeoDataPlacemarkType ) {
//                qDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[0](" << parentObject << ")";
            return createIndex( 0, 0, parentObject );
        }

        // greatParent can be a multigeometry
        if ( greatParentType == GeoDataTypes::GeoDataMultiGeometryType ) {
            GeoDataGeometry *parentGeometry = static_cast<GeoDataGeometry*>( parentObject );
            GeoDataMultiGeometry * greatParentItem = static_cast<GeoDataMultiGeometry*>( greatParentObject );
//                qDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[" << greatParentItem->childPosition( parentGeometry ) << "](" << parentObject << ")";
            return createIndex( greatParentItem->childPosition( parentGeometry ), 0, parentObject );
        }

    }

//    qDebug() << "parent unknown index";
    return QModelIndex();
}

int GeoDataTreeModel::columnCount( const QModelIndex &node ) const
{
//    qDebug() << "columncount";
    if ( node.isValid() ) {
//        qDebug() << "columncount valid";
        return 1;
    } else {
//        qDebug() << "columncount invalid";
        return 1;
    }
}

void GeoDataTreeModel::setFileManager( FileManager *fileManager )
{
    d->m_fileManager = fileManager;
    connect( d->m_fileManager, SIGNAL( fileAdded(int)),
             this,          SLOT(addDocument(int)) );
    connect( d->m_fileManager, SIGNAL( fileRemoved(int)),
             this,          SLOT(removeDocument(int)) );
}

void GeoDataTreeModel::addDocument( int index )
{
    KmlFileViewItem *file =
            static_cast<KmlFileViewItem*>(d->m_fileManager->at(index));
    if (file)
    {
        beginResetModel();
        d->m_rootDocument->append( file->document() ) ;
        endResetModel();
    }
}

void GeoDataTreeModel::removeDocument( int index )
{
    beginResetModel();
    d->m_rootDocument->remove( index );
    endResetModel();
}

void GeoDataTreeModel::update()
{
//    qDebug() << "updating GeoDataTreeModel";
    reset();
}

#include "GeoDataTreeModel.moc"
