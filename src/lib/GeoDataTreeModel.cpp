//
// This file is part of the Marble Virtual Globe.
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
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "FileManager.h"
#include "MarbleDebug.h"
#include "MarblePlacemarkModel.h"

using namespace Marble;

class GeoDataTreeModel::Private {
 public:
    Private();
    ~Private();

    GeoDataDocument* m_rootDocument;
    bool             m_ownsRootDocument;
};

GeoDataTreeModel::Private::Private() :
    m_rootDocument( new GeoDataDocument ),
    m_ownsRootDocument( true )
{
    // nothing to do
}

GeoDataTreeModel::Private::~Private()
{
    if ( m_ownsRootDocument ) {
        delete m_rootDocument;
    }
}

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

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        return dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataFolderType
         || parentItem->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
        return container->size();
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
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

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataFolderType
         || parentItem->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
//        mDebug() << "rowCount " << type << "(" << parentItem << ") =" << container->size();
        return container->size();
//    } else {
//        mDebug() << "rowCount bad container " << container;
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        if ( dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() ) ) {
//            mDebug() << "rowCount " << type << "(" << parentItem << ") = 1";
            return 1;
        }
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
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

        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
                if ( index.column() == 0 ){
                    return QVariant( placemark->name() );
                }
                else if ( index.column() == 1 ){
                    return QVariant( placemark->nodeType() );
                }
                else if ( index.column() == 2 ){
                    return QVariant( placemark->popularity() );
                }
                else if ( index.column() == 3 ){
                    return QVariant( placemark->popularityIndex() );
                }
        }
        if ( object->nodeType() == GeoDataTypes::GeoDataFolderType
             || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataFeature *feature = static_cast<GeoDataFeature*>( object );
            if ( index.column() == 0 ){
                return QVariant( feature->name() );
            }
            else if ( index.column() == 1 ){
                return QVariant( feature->nodeType() );
            }
        }

        GeoDataGeometry *geometry = dynamic_cast<GeoDataGeometry*>( object );
        if ( geometry && index.column() == 1 ){
            return QVariant( geometry->nodeType() );
        }

        GeoDataObject *item = dynamic_cast<GeoDataObject*>( object );
        if ( item && index.column() == 1 ){
            return QVariant( item->nodeType() );
        }

    }
    else if ( role == Qt::CheckStateRole
              && index.column() == 0 ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *feature = static_cast<GeoDataPlacemark*>( object );
            const char* type = feature->geometry()->nodeType();
            if ( type == GeoDataTypes::GeoDataLineStringType
                 || type == GeoDataTypes::GeoDataPolygonType
                 || type == GeoDataTypes::GeoDataLinearRingType
                 || type == GeoDataTypes::GeoDataMultiGeometryType
                 ) {
                if ( feature->isVisible() ) {
                    return QVariant( Qt::Checked );
                }
                else {
                    return QVariant( Qt::Unchecked );
                }
            }
        }
    }
    else if ( role == Qt::DecorationRole
              && index.column() == 0 ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType
             || object->nodeType() == GeoDataTypes::GeoDataFolderType
             || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataFeature *feature = static_cast<GeoDataFeature*>( object );
            return QVariant(feature->style()->iconStyle().icon());
        }
    } else if ( role == Qt::ToolTipRole
              && index.column() == 0 ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType
             || object->nodeType() == GeoDataTypes::GeoDataFolderType
             || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataFeature *feature = static_cast<GeoDataFeature*>( object );
            return QVariant( feature->description() );
        }
    } else if ( role == MarblePlacemarkModel::ObjectPointerRole ) {
        return qVariantFromValue( object );
    } else if ( role == MarblePlacemarkModel::PopularityIndexRole ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
            return QVariant( placemark->popularityIndex() );
        }
    } else if ( role == MarblePlacemarkModel::PopularityRole ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
            return QVariant( placemark->popularity() );
        }
    } else if ( role == MarblePlacemarkModel::CoordinateRole ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
            return qVariantFromValue( placemark->coordinate() );
        }
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


    if ( parentItem->nodeType() == GeoDataTypes::GeoDataFolderType
         || parentItem->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        GeoDataContainer *container = static_cast<GeoDataContainer*>( parentItem );
        childItem = container->child( row );
        return createIndex( row, column, childItem );
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( parentItem );
        childItem = placemark->geometry();
        if ( dynamic_cast<GeoDataMultiGeometry*>( childItem ) ) {
            return createIndex( row, column, childItem );
        }
    }

    if ( parentItem->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>( parentItem );
        childItem = geometry->child( row );
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
        if ( greatParentObject->nodeType() == GeoDataTypes::GeoDataFolderType
             || greatParentObject->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataContainer *greatparentContainer = static_cast<GeoDataContainer*>( greatParentObject );
            GeoDataFeature *parentFeature = static_cast<GeoDataFeature*>( parentObject );
//            mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                    << parentObject->nodeType() << "[" << greatparentContainer->childPosition( parentFeature ) << "](" << parentObject << ")";
            return createIndex( greatparentContainer->childPosition( parentFeature ), 0, parentObject );
        }

        // greatParent can be a placemark
        if ( greatParentObject->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
//            GeoDataPlacemark *greatparentPlacemark = static_cast<GeoDataPlacemark*>( greatParentObject );
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[0](" << parentObject << ")";
            return createIndex( 0, 0, parentObject );
        }

        // greatParent can be a multigeometry
        if ( greatParentObject->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
            GeoDataMultiGeometry *greatparentMultiGeo = static_cast<GeoDataMultiGeometry*>( greatParentObject );
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
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType
             || object->nodeType() == GeoDataTypes::GeoDataFolderType
             || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataFeature *feature = static_cast<GeoDataFeature*>( object );
            feature->setVisible( value.toBool() );
            mDebug() << "setData " << feature->name() << " " << value.toBool();
            emit dataChanged( index, index );
            return true;
        }
    } else if ( role == Qt::EditRole ) {
        if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType
             || object->nodeType() == GeoDataTypes::GeoDataFolderType
             || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataFeature *feature = static_cast<GeoDataFeature*>( object );
            feature->setName( value.toString() );
            mDebug() << "setData " << feature->name() << " " << value.toString();
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
    if ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType
         || object->nodeType() == GeoDataTypes::GeoDataFolderType
         || object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


int GeoDataTreeModel::addDocument( GeoDataDocument *document )
{
    beginInsertRows( QModelIndex(), d->m_rootDocument->size(), d->m_rootDocument->size() );
    d->m_rootDocument->append( document ) ;
    endInsertRows();
    emit treeChanged();
    return d->m_rootDocument->childPosition( document );
}

void GeoDataTreeModel::removeDocument( int index )
{
    beginRemoveRows( QModelIndex(), index, index);
    d->m_rootDocument->remove( index );
    endRemoveRows();
    emit treeChanged();
}

void GeoDataTreeModel::removeDocument( GeoDataDocument *document )
{
    int index = d->m_rootDocument->childPosition( document );
    if ( index != -1 ) {
        removeDocument( index );
    }
}

void GeoDataTreeModel::update()
{
//    mDebug() << "updating GeoDataTreeModel";
    reset();
}

void GeoDataTreeModel::setRootDocument( GeoDataDocument* document )
{
    beginResetModel();
    if ( d->m_ownsRootDocument ) {
        delete d->m_rootDocument;
    }

    d->m_ownsRootDocument = ( document == 0 );
    d->m_rootDocument = document ? document : new GeoDataDocument;
    endResetModel();
}

GeoDataDocument * GeoDataTreeModel::rootDocument()
{
    return d->m_rootDocument;
}

#include "GeoDataTreeModel.moc"
