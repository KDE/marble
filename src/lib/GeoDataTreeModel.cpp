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
#include <QtCore/QList>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "GeoDataContainer.h"
#include "GeoDataExtendedData.h"
#include "GeoDataPlacemark.h"
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


QModelIndex GeoDataTreeModel::index( GeoDataObject *object )
{
    //It first runs bottom-top, storing every ancestor of the object, and
    //then goes top-down retrieving the QModelIndex of every ancestor until reaching the
    //index of the requested object.
    //The TreeModel contains: Documents, Folders, Placemarks, MultiGeometries
    //and Geometries that are children of MultiGeometries
    //You can not call this function with an element that does not belong to the tree

    Q_ASSERT( ( object->nodeType() == GeoDataTypes::GeoDataFolderType )
              || ( object->nodeType() == GeoDataTypes::GeoDataDocumentType )
              || ( object->nodeType() == GeoDataTypes::GeoDataPlacemarkType )
              || ( ( object->nodeType() == GeoDataTypes::GeoDataLineStringType )
                   && ( object->parent()->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) )
              || ( ( object->nodeType() == GeoDataTypes::GeoDataLinearRingType )
                   && ( object->parent()->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) )
              || ( ( object->nodeType() == GeoDataTypes::GeoDataPointType )
                   && ( object->parent()->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) )
              || ( ( object->nodeType() == GeoDataTypes::GeoDataPolygonType )
                   && ( object->parent()->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) )
              || ( object->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) );


    QList< GeoDataObject* > ancestors;

    GeoDataObject *itup = object; //Iterator to reach the top of the GeoDataDocument (bottom-up)

    while ( itup && ( itup != d->m_rootDocument ) ) {//We reach up to the rootDocument

        ancestors.append( itup );
        itup = itup->parent() ;
    }

    QModelIndex itdown;
    if ( !ancestors.isEmpty() ) {

        itdown = index( d->m_rootDocument->childPosition( static_cast<GeoDataFeature*>( ancestors.last() ) ),0,QModelIndex());//Iterator to go top down

        GeoDataObject *parent;

        while ( ( ancestors.size() > 1 ) ) {

            parent = static_cast<GeoDataObject*>( ancestors.last() );

            if ( ( parent->nodeType() == GeoDataTypes::GeoDataFolderType )
                || ( parent->nodeType() == GeoDataTypes::GeoDataDocumentType ) ) {

                ancestors.removeLast();
                itdown = index( static_cast<GeoDataContainer*>(parent)->childPosition( static_cast<GeoDataFeature*>( ancestors.last() ) ) , 0, itdown );
            } else if ( ( parent->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) ) {
                //The only child of the model is a Geometry or MultiGeometry object
                //If it is a geometry object, we should be on the bottom of the list
                ancestors.removeLast();
                if( ancestors.last()->nodeType() == GeoDataTypes::GeoDataMultiGeometryType )
                    itdown = index( 0 , 0, itdown );
                else
                    itdown = QModelIndex();

            }  else if ( ( parent->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) ) {
                //The child is one of the geometry children of MultiGeometry
                ancestors.removeLast();
                itdown = index( static_cast<GeoDataMultiGeometry*>(parent)->childPosition( static_cast<GeoDataGeometry*>(ancestors.last()) ) , 0, itdown );
            }
            else  {   //If the element is not found on the tree, it will be added under m_rootDocument
                itdown = QModelIndex();
                break;
            }
        }
    }
    return itdown;
}

int GeoDataTreeModel::addFeature( GeoDataContainer *parent, GeoDataFeature *feature )
{
    int row = -1;
    if ( parent && feature ) {

        QModelIndex modelindex = index( parent );
            //index(GeoDataObject*) returns QModelIndex() if parent == m_rootDocument
            //or if parent is not found on the tree.
            //We must check that we are in top of the tree (then QModelIndex() is
            //the right parent to insert the child object) or that we have a valid QModelIndex

        if( ( parent == d->m_rootDocument ) || modelindex.isValid() )
        {
            row = parent->size();
            beginInsertRows( modelindex , row , row );
            parent->append( feature );
            endInsertRows();
            emit treeChanged();
            emit added(feature);
        }
        else
            mDebug() << "GeoDataTreeModel::addFeature (parent " << parent << " - feature" << feature << ") : parent not found on the TreeModel";
    }
    else
        mDebug() << "Null pointer in call to GeoDataTreeModel::addFeature (parent " << parent << " - feature" << feature << ")";
    return row; //-1 if it failed, the relative index otherwise.
}

int GeoDataTreeModel::addDocument( GeoDataDocument *document )
{
    return addFeature( d->m_rootDocument, document );
}

bool GeoDataTreeModel::removeFeature( GeoDataContainer *parent, int row )
{
    if ( row<parent->size() ) {
        beginRemoveRows( index( parent ), row , row );
        parent->remove( row );
        endRemoveRows();
        emit treeChanged();
        return true;
    }
    return false; //Tried to remove a row that is not contained in the parent.
}

bool GeoDataTreeModel::removeFeature( GeoDataFeature *feature )
{
    if ( feature && ( feature!=d->m_rootDocument ) )  {//We check to see we are not removing the
                                                      //top level element m_rootDocument
        GeoDataObject *parent = static_cast< GeoDataObject* >( feature->parent() );

        if ( ( parent->nodeType() == GeoDataTypes::GeoDataFolderType )
            || ( parent->nodeType() == GeoDataTypes::GeoDataDocumentType ) ) {

            int row = static_cast< GeoDataContainer* >( feature->parent() )->childPosition( feature );
            if ( row != -1 ) {
                if ( removeFeature( static_cast< GeoDataContainer* >( feature->parent() ) , row ) ) {
                    emit removed(feature);
                    return true;
                }
                else
                    return false;
            }
            else
                return false; //The feature is not contained in the parent it points to
        }
    }
    return false; //We can not remove the rootDocument
}

void GeoDataTreeModel::removeDocument( int index )
{
    removeFeature( d->m_rootDocument, index );
}

void GeoDataTreeModel::removeDocument( GeoDataDocument *document )
{
    removeFeature( document );
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
