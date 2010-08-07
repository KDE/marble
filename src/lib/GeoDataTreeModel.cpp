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
    ~Private() { delete m_rootDocument; }

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

bool GeoDataTreeModel::hasChildren( const QModelIndex &parent ) const
{
    GeoDataObject *parentItem;
    if ( parent.column() > 0 ) {
        return false;
    }

    if ( !parent.isValid() ) {
        parentItem = d->m_rootDocument;
    } else {
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );
    }

    if ( !parentItem ) {
        return false;
    }

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( parentItem );
    if ( container ) {
        return container->size();
    }

    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( parentItem );
    if ( placemark ) {
        return placemark->geometry();
    }

    GeoDataMultiGeometry *geometry = dynamic_cast<GeoDataMultiGeometry*>( parentItem );
    if ( geometry ) {
        return geometry->size();
    }

    return false;
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

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( parentItem );
    if ( container ) {
//        mDebug() << "rowCount " << type << "(" << parentItem << ") =" << container->size();
        return container->size();
//    } else {
//        mDebug() << "rowCount bad container " << container;
    }

    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( parentItem );
    if ( placemark ) {
        if ( placemark->geometry() ) {
//            mDebug() << "rowCount " << type << "(" << parentItem << ") = 1";
            return 1;
        }
//        mDebug() << "rowCount " << type << "(" << parentItem << ") = 0";
        return 0;
    }

    GeoDataMultiGeometry *geometry = dynamic_cast<GeoDataMultiGeometry*>( parentItem );
    if ( geometry ) {
//        mDebug() << "rowCount " << parent << " " << type << " " << geometry->size();
        return geometry->size();
//    } else {
//        mDebug() << "rowCount bad geometry " << geometry;
    }

//    mDebug() << "rowcount end";
    return 0;//parentItem->childCount();
}

QVariant GeoDataTreeModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if ( role == Qt::DisplayRole && orientation == Qt::Horizontal )
    {
        switch ( section ) {
        case 0:
            return QString("Name");
            break;
        case 1:
             return QString("Type");
             break;
        case 2:
            return QString("Popularity");
            break;
        case 3:
            return QString("PopIndex");
            break;
        }
    }
    return QVariant();
}

QVariant GeoDataTreeModel::data( const QModelIndex &index, int role ) const
{
//    mDebug() << "data";
    if ( !index.isValid() )
        return QVariant();

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( role == Qt::DisplayRole ) {

        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
        if ( placemark ) {
            if ( index.column() == 0 )
                return QVariant( placemark->name() );
            else if ( index.column() == 1 )
                return QVariant( placemark->nodeType() );
            else if ( index.column() == 2 )
                return QVariant( placemark->popularity() );
            else if ( index.column() == 3 )
                return QVariant( placemark->popularityIndex() );
        }
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature ) {
            if ( index.column() == 0 )
                return QVariant( feature->name() );
            else if ( index.column() == 1 )
                return QVariant( feature->nodeType() );
        }

        GeoDataGeometry *geometry = dynamic_cast<GeoDataGeometry*>( object );
        if ( geometry && index.column() == 1 )
            return QVariant( geometry->nodeType() );

        GeoDataObject *item = dynamic_cast<GeoDataObject*>( object );
        if ( item && index.column() == 1 )
            return QVariant( item->nodeType() );

    }
    else if ( role == Qt::CheckStateRole
              && index.column() == 0 ) {
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
        if ( feature ) {
            if ( feature->isVisible() ) {
                return QVariant( Qt::Checked );
            }
            else {
                return QVariant( Qt::Unchecked );
            }
        }
    }
    else if ( role == Qt::DecorationRole
              && index.column() == 0 ) {
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

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( parentItem );
    if ( container ) {
        childItem = container->child( row );
    }

    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( parentItem );
    if ( placemark ) {
        childItem = placemark->geometry();
    }

    GeoDataMultiGeometry *geometry = dynamic_cast<GeoDataMultiGeometry*>( parentItem );
    if ( geometry ) {
        childItem = geometry->child( row );
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

        // greatParent can be a container
        GeoDataContainer *greatparentContainer = dynamic_cast<GeoDataContainer*>( greatParentObject );
        if ( greatparentContainer ) {
            GeoDataFeature *parentFeature = static_cast<GeoDataFeature*>( parentObject );
//            mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                    << parentObject->nodeType() << "[" << greatparentContainer->childPosition( parentFeature ) << "](" << parentObject << ")";
            return createIndex( greatparentContainer->childPosition( parentFeature ), 0, parentObject );
        }

        // greatParent can be a placemark
        GeoDataPlacemark *greatparentPlacemark = dynamic_cast<GeoDataPlacemark*>( greatParentObject );
        if ( greatparentPlacemark ) {
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[0](" << parentObject << ")";
            return createIndex( 0, 0, parentObject );
        }

        // greatParent can be a multigeometry
        GeoDataMultiGeometry *greatparentMultiGeo = dynamic_cast<GeoDataMultiGeometry*>( greatParentObject );
        if ( greatparentMultiGeo ) {
            GeoDataGeometry *parentGeometry = static_cast<GeoDataGeometry*>( parentObject );
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[" << greatParentItem->childPosition( parentGeometry ) << "](" << parentObject << ")";
            return createIndex( greatparentMultiGeo->childPosition( parentGeometry ), 0, parentObject );
        }

    }

//    mDebug() << "parent unknown index";
    return QModelIndex();
}

int GeoDataTreeModel::columnCount( const QModelIndex & ) const
{
    return 4;
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

    return false;
}

Qt::ItemFlags GeoDataTreeModel::flags ( const QModelIndex & index ) const
{
    if ( !index.isValid() )
        return Qt::NoItemFlags;

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
    if ( feature ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


void GeoDataTreeModel::setFileManager( FileManager *fileManager )
{
    disconnect( this, SLOT(addDocument(int)) );
    disconnect( this, SLOT(removeDocument(int)) );
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
