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
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataDocument.h"
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "FileManager.h"
#include "KmlFileViewItem.h"
#include "MarbleDebug.h"

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
//    mDebug() << "rowCount";
    GeoDataObject *parentItem;
    if ( parent.column() > 0 ) {
//        mDebug() << "rowCount bad column";
        return 0;
    }

    if ( !parent.isValid() ) {
//        mDebug() << "rowCount root parent";
        parentItem = d->m_rootDocument;
    } else {
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );
    }

    if ( !parentItem ) {
//        mDebug() << "rowCount bad parent";
        return 0;
    }

    QString type = parentItem->nodeType();
    if ( type == GeoDataTypes::GeoDataContainerType
        || type == GeoDataTypes::GeoDataDocumentType
        || type == GeoDataTypes::GeoDataFolderType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
        if ( container ) {
//            mDebug() << "rowCount " << type << "(" << parentItem << ") =" << container->size();
            return container->size();
        } else {
//            mDebug() << "rowCount bad container " << container;
        }
    }

    if ( type == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        if ( placemark ) {
            if ( placemark->geometry() ) {
//                mDebug() << "rowCount " << type << "(" << parentItem << ") = 1";
                return 1;
            }
//            mDebug() << "rowCount " << type << "(" << parentItem << ") = 0";
            return 0;
        }
    }

    if ( type == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
        if ( geometry ) {
//            mDebug() << "rowCount " << parent << " " << type << " " << geometry->size();
            return geometry->size();
        } else {
//            mDebug() << "rowCount bad geometry " << geometry;
        }
    }

//    mDebug() << "rowcount end";
    return 0;//parentItem->childCount();
}


QVariant GeoDataTreeModel::data( const QModelIndex &index, int role ) const
{
//    mDebug() << "data";
    if ( !index.isValid() )
        return QVariant();

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( role == Qt::DisplayRole ) {

        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature )
            return QVariant( feature->nodeType().append("-").append(feature->name()) );

        GeoDataGeometry *geometry = dynamic_cast<GeoDataGeometry*>( object );
        if ( geometry )
            return QVariant( geometry->nodeType() );

        GeoDataObject *item = dynamic_cast<GeoDataObject*>( object );
        if ( item )
            return QVariant( item->nodeType() );

    }
    else if ( role == Qt::CheckStateRole ) {
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature )
            if ( feature->isVisible() )
                return QVariant( Qt::Checked );
            else
                return QVariant( Qt::Unchecked );
    }
    else if ( role == Qt::DecorationRole ) {
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature )
            return QVariant(feature->style()->iconStyle().icon());
    }

    return QVariant();
}

QModelIndex GeoDataTreeModel::index( int row, int column, const QModelIndex &parent )             const
{
//    mDebug() << "index";
    if ( !hasIndex( row, column, parent ) ) {
//        mDebug() << "index bad index";
        return QModelIndex();
    }
    
    GeoDataObject *parentItem;
    
    if ( !parent.isValid() )
        parentItem = d->m_rootDocument;
    else
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );

    if ( !parentItem ) {
//        mDebug() << "index bad parent";
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
//        mDebug() << "index " << type << "[" << row << "](" << parentItem << ") ="
//                << childItem->nodeType() << "(" << childItem << ")";
        return createIndex( row, column, childItem );
    }
    return QModelIndex();
}

QModelIndex GeoDataTreeModel::parent( const QModelIndex &index ) const
{
//    mDebug() << "parent";
    if ( !index.isValid() ) {
//        mDebug() << "parent bad index";
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
//            mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                    << parentObject->nodeType() << "[" << greatparentContainer->childPosition( parentFeature ) << "](" << parentObject << ")";
            return createIndex( greatparentContainer->childPosition( parentFeature ), 0, parentObject );
        }

        // greatParent can be a placemark
        if ( greatParentType == GeoDataTypes::GeoDataPlacemarkType ) {
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[0](" << parentObject << ")";
            return createIndex( 0, 0, parentObject );
        }

        // greatParent can be a multigeometry
        if ( greatParentType == GeoDataTypes::GeoDataMultiGeometryType ) {
            GeoDataGeometry *parentGeometry = static_cast<GeoDataGeometry*>( parentObject );
            GeoDataMultiGeometry * greatParentItem = static_cast<GeoDataMultiGeometry*>( greatParentObject );
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[" << greatParentItem->childPosition( parentGeometry ) << "](" << parentObject << ")";
            return createIndex( greatParentItem->childPosition( parentGeometry ), 0, parentObject );
        }

    }

//    mDebug() << "parent unknown index";
    return QModelIndex();
}

int GeoDataTreeModel::columnCount( const QModelIndex &node ) const
{
//    mDebug() << "columncount";
    if ( node.isValid() ) {
//        mDebug() << "columncount valid";
        return 1;
    } else {
//        mDebug() << "columncount invalid";
        return 1;
    }
}

bool GeoDataTreeModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if ( !index.isValid() )
        return false;

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( role == Qt::CheckStateRole ) {
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature ) {
            feature->setVisible( value.toBool() );
            mDebug() << "setData " << feature->name() << " " << value.toBool();
            emit dataChanged( index, index );
            return true;
        }
    }
}

Qt::ItemFlags GeoDataTreeModel::flags ( const QModelIndex & index ) const
{
    if ( !index.isValid() )
        return Qt::NoItemFlags;

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
    if ( feature ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
//    mDebug() << "updating GeoDataTreeModel";
    reset();
}

#include "GeoDataTreeModel.moc"
