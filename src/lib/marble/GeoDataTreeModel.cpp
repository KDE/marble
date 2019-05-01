//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Thibaut Gridel <tgridel@free.fr>
// Copyright 2013      Levente Kurusa <levex@linux.com>
//


// Own
#include "GeoDataTreeModel.h"

// Qt
#include <QBrush>
#include <QModelIndex>
#include <QList>
#include <QItemSelectionModel>

// Marble
#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "GeoDataContainer.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLookAt.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"
#include "GeoDataWait.h"
#include "GeoDataFlyTo.h"
#include "GeoDataCamera.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataListStyle.h"
#include "FileManager.h"
#include "MarbleDebug.h"
#include "MarblePlacemarkModel.h"

using namespace Marble;

class Q_DECL_HIDDEN GeoDataTreeModel::Private {
 public:
    Private( QAbstractItemModel* model );
    ~Private();

    static void checkParenting( GeoDataObject *object );

    GeoDataDocument* m_rootDocument;
    bool             m_ownsRootDocument;
    QItemSelectionModel m_selectionModel;
    QHash<int, QByteArray> m_roleNames;
};

GeoDataTreeModel::Private::Private( QAbstractItemModel *model ) :
    m_rootDocument( new GeoDataDocument ),
    m_ownsRootDocument( true ),
    m_selectionModel( model )
{
    m_roleNames[MarblePlacemarkModel::DescriptionRole] = "description";
    m_roleNames[MarblePlacemarkModel::IconPathRole] = "iconPath";
    m_roleNames[MarblePlacemarkModel::PopularityIndexRole] = "zoomLevel";
    m_roleNames[MarblePlacemarkModel::VisualCategoryRole] = "visualCategory";
    m_roleNames[MarblePlacemarkModel::AreaRole] = "area";
    m_roleNames[MarblePlacemarkModel::PopulationRole] = "population";
    m_roleNames[MarblePlacemarkModel::CountryCodeRole] = "countryCode";
    m_roleNames[MarblePlacemarkModel::StateRole] = "state";
    m_roleNames[MarblePlacemarkModel::PopularityRole] = "popularity";
    m_roleNames[MarblePlacemarkModel::GeoTypeRole] = "role";
    m_roleNames[MarblePlacemarkModel::CoordinateRole] = "coordinate";
    m_roleNames[MarblePlacemarkModel::StyleRole] = "style";
    m_roleNames[MarblePlacemarkModel::GmtRole] = "gmt";
    m_roleNames[MarblePlacemarkModel::DstRole] = "dst";
    m_roleNames[MarblePlacemarkModel::GeometryRole] = "geometry";
    m_roleNames[MarblePlacemarkModel::ObjectPointerRole] = "objectPointer";
    m_roleNames[MarblePlacemarkModel::LongitudeRole] = "longitude";
    m_roleNames[MarblePlacemarkModel::LatitudeRole] = "latitude";
}

GeoDataTreeModel::Private::~Private()
{
    if ( m_ownsRootDocument ) {
        delete m_rootDocument;
    }
}

void GeoDataTreeModel::Private::checkParenting( GeoDataObject *object )
{
    if (const auto container = dynamic_cast<const GeoDataContainer *>(object)) {
        for( GeoDataFeature *child: container->featureList() ) {
            if ( child->parent() != container ) {
                qWarning() << "Parenting mismatch for " << child->name();
                Q_ASSERT( 0 );
            }
        }
    }
}

GeoDataTreeModel::GeoDataTreeModel( QObject *parent )
    : QAbstractItemModel( parent ),
      d( new Private( this ) )
{
    auto const roleNames = QAbstractItemModel::roleNames();
    for(auto iter = roleNames.constBegin(); iter != roleNames.constEnd(); ++iter) {
        d->m_roleNames[iter.key()] = iter.value();
    }
}

GeoDataTreeModel::~GeoDataTreeModel()
{
    delete d;
}

int GeoDataTreeModel::rowCount( const QModelIndex &parent ) const
{
//    mDebug() << "rowCount";
    const GeoDataObject *parentItem;
    if ( parent.column() > 0 ) {
//        mDebug() << "rowCount bad column";
        return 0;
    }

    if ( !parent.isValid() ) {
//        mDebug() << "rowCount root parent";
        parentItem = d->m_rootDocument;
    } else {
        parentItem = static_cast<const GeoDataObject *>(parent.internalPointer());
    }

    if ( !parentItem ) {
//        mDebug() << "rowCount bad parent";
        return 0;
    }

    if (const GeoDataContainer *container = dynamic_cast<const GeoDataContainer *>(parentItem)) {
//        mDebug() << "rowCount " << type << "(" << parentItem << ") =" << container->size();
        return container->size();
//    } else {
//        mDebug() << "rowCount bad container " << container;
    }

    if (const auto placemark = geodata_cast<GeoDataPlacemark>(parentItem)) {
        if (geodata_cast<GeoDataMultiGeometry>(placemark->geometry())) {
//            mDebug() << "rowCount " << type << "(" << parentItem << ") = 1";
            return 1;
        }
    }

    if (const auto geometry = geodata_cast<GeoDataMultiGeometry>(parentItem)) {
//        mDebug() << "rowCount " << parent << " " << type << " " << geometry->size();
        return geometry->size();
//    } else {
//        mDebug() << "rowCount bad geometry " << geometry;
    }

    if (const auto tour = geodata_cast<GeoDataTour>(parentItem)) {
        const GeoDataPlaylist *playlist = tour->playlist();
        if ( playlist ) {
//            mDebug() << "rowCount " << parent << " Playlist " << 1;
            return 1;
        }
    }

    if (const auto playlist = geodata_cast<GeoDataPlaylist>(parentItem)) {
//         mDebug() << "rowCount " << parent << " Playlist " << playlist->size();
        return playlist->size();
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
            return tr("Name");
            break;
        case 1:
             return tr("Type");
             break;
        case 2:
            return tr("Popularity");
            break;
        case 3:
            return tr("PopIndex", "Popularity index");
            break;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> GeoDataTreeModel::roleNames() const
{
    return d->m_roleNames;
}

QVariant GeoDataTreeModel::data( const QModelIndex &index, int role ) const
{
//    mDebug() << "data";
    if ( !index.isValid() )
        return QVariant();

    GeoDataObject *object = static_cast<GeoDataObject*>( index.internalPointer() );
    if ( role == Qt::DisplayRole ) {

        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
                if ( index.column() == 0 ){
                    if ( placemark->countryCode().isEmpty() ) {
                        return QVariant( placemark->name() );
                    } else {
                        return QVariant(placemark->name() + QLatin1String(" (") + placemark->countryCode() + QLatin1Char(')'));
                    }

                }
                else if ( index.column() == 1 ){
                    return QVariant( placemark->nodeType() );
                }
                else if ( index.column() == 2 ){
                    return QVariant( placemark->popularity() );
                }
                else if ( index.column() == 3 ){
                    return QVariant( placemark->zoomLevel() );
                }
        }

        if (const auto feature = dynamic_cast<const GeoDataFeature *>(object)) {
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

        GeoDataPlaylist *playlist = geodata_cast<GeoDataPlaylist>(object);
        if ( playlist && index.column() == 0 ) {
            return tr( "Playlist" );
        }

        if (object && index.column() == 1) {
            return QVariant(object->nodeType());
        }

    }
    else if ( role == Qt::CheckStateRole
              && index.column() == 0 ) {
        if (const auto feature = geodata_cast<GeoDataPlacemark>(object)) {
            if (const auto folder = geodata_cast<GeoDataFolder>(feature->parent())) {
                if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder 
                  || folder->style()->listStyle().listItemType() == GeoDataListStyle::CheckOffOnly) {
                    if ( feature->isVisible() ) {
                        return QVariant ( Qt::Checked );
                    } else {
                        return QVariant ( Qt::Unchecked );
                    }
                }
            }

            if (feature->isGloballyVisible()) {
                return QVariant(Qt::Checked);
            }

            if (feature->isVisible()) {
                return QVariant(Qt::PartiallyChecked);
            }

            return QVariant(Qt::Unchecked);
        } else if (auto feature = dynamic_cast<GeoDataContainer *>(object)) {
            if (auto folder = geodata_cast<GeoDataFolder>(object)) {
                if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder) {
                    bool anyVisible = false;
                    QVector<GeoDataFeature *>::Iterator i = folder->begin();
                    for (; i < folder->end(); ++i) {
                        if ((*i)->isVisible()) {
                            anyVisible = true;
                            break;
                        }
                    }
                    if (anyVisible) {
                        return QVariant( Qt::PartiallyChecked );
                    } else {
                        return QVariant( Qt::Unchecked );
                    }
                } else if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::CheckOffOnly) {
                    QVector<GeoDataFeature *>::Iterator i = folder->begin();
                    bool anyVisible = false;
                    bool allVisible = true;
                    for (; i < folder->end(); ++i) {
                        if ((*i)->isVisible()) {
                            anyVisible = true;
                        } else {
                            allVisible = false;
                        }
                    }
                    if (allVisible) {
                        return QVariant( Qt::Checked );
                    } else if (anyVisible) {
                        return QVariant( Qt::PartiallyChecked );
                    } else {
                        return QVariant( Qt::Unchecked );
                    }
                }
            }
            if ( feature->isGloballyVisible() ) {
                return QVariant( Qt::Checked );
            } else if ( feature->isVisible() ) {
                return QVariant( Qt::PartiallyChecked );
            } else {
                return QVariant( Qt::Unchecked );
            }
        }
    }
    else if ( role == Qt::DecorationRole
              && index.column() == 0 ) {
        if (const auto feature = dynamic_cast<const GeoDataFeature *>(object)) {
            if (feature->style()->iconStyle().icon().isNull()) {
                return QImage();
            }

            return QVariant(feature->style()->iconStyle().icon().scaled( QSize(16,16), Qt::KeepAspectRatio, Qt::SmoothTransformation ));
	    }
    } else if ( role == Qt::ToolTipRole
              && index.column() == 0 ) {
        if (const auto feature = dynamic_cast<const GeoDataFeature *>(object)) {
            return QVariant( feature->description() );
        }
    } else if ( role == MarblePlacemarkModel::ObjectPointerRole ) {
        return qVariantFromValue( object );
    } else if ( role == MarblePlacemarkModel::PopularityIndexRole ) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
            return QVariant( placemark->zoomLevel() );
        }
    } else if ( role == MarblePlacemarkModel::PopularityRole ) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
            return QVariant( placemark->popularity() );
        }
    } else if ( role == MarblePlacemarkModel::CoordinateRole ) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
            return qVariantFromValue( placemark->coordinate() );
        } else if (const auto flyTo = geodata_cast<GeoDataFlyTo>(object)) {
            if (const auto camera = geodata_cast<GeoDataCamera>(flyTo->view())) {
                return QVariant::fromValue<GeoDataCoordinates>( camera->coordinates() );
            } else if (const auto lookAt = (flyTo->view() ? geodata_cast<GeoDataLookAt>(flyTo->view()) : nullptr)) {
                return QVariant::fromValue<GeoDataCoordinates>( lookAt->coordinates() );
            }
        }
    } else if ( role == Qt::BackgroundRole ) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
            if (const GeoDataContainer *container = dynamic_cast<const GeoDataContainer *>(placemark->parent())) {
                return container->customStyle() ? QVariant( QBrush( container->customStyle()->listStyle().backgroundColor() )) : QVariant();
            }
        }
    } else if (role == MarblePlacemarkModel::IconPathRole) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
            return placemark->style()->iconStyle().iconPath();
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

    GeoDataObject *childItem = nullptr;


    if (auto container = dynamic_cast<GeoDataContainer *>(parentItem)) {
        childItem = container->child( row );
        return createIndex( row, column, childItem );
    }

    if (const auto placemark = geodata_cast<GeoDataPlacemark>(parentItem)) {
        childItem = placemark->geometry();
        if (geodata_cast<GeoDataMultiGeometry>(childItem)) {
            return createIndex( row, column, childItem );
        }
    }

    if (const auto geometry = geodata_cast<GeoDataMultiGeometry>(parentItem)) {
        childItem = geometry->child( row );
        return createIndex( row, column, childItem );
    }

    if (const auto tour = geodata_cast<GeoDataTour>(parentItem)) {
        childItem = tour->playlist();
        return createIndex( row, column, childItem );
    }

    if (const auto playlist = geodata_cast<GeoDataPlaylist>(parentItem)) {
        childItem = playlist->primitive( row );
        return createIndex(row, column, childItem);
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

        /// parentObject can be a container, placemark, multigeometry or playlist
        GeoDataObject *parentObject = childObject->parent();
        if ( parentObject == d->m_rootDocument )
        {
            return QModelIndex();
        }

        GeoDataObject *greatParentObject = parentObject->parent();

        // Avoid crashing when there is no grandparent
        if ( greatParentObject == nullptr )
        {
            return QModelIndex();
        }

        // greatParent can be a container
        if (auto greatparentContainer = dynamic_cast<GeoDataContainer *>(greatParentObject)) {
            GeoDataFeature *parentFeature = static_cast<GeoDataFeature*>( parentObject );
//            mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                    << parentObject->nodeType() << "[" << greatparentContainer->childPosition( parentFeature ) << "](" << parentObject << ")";
            return createIndex( greatparentContainer->childPosition( parentFeature ), 0, parentObject );
        }

        // greatParent can be a placemark
        if (geodata_cast<GeoDataPlacemark>(greatParentObject)) {
//            GeoDataPlacemark *greatparentPlacemark = static_cast<GeoDataPlacemark*>( greatParentObject );
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[0](" << parentObject << ")";
            return createIndex( 0, 0, parentObject );
        }

        // greatParent can be a multigeometry
        if (GeoDataMultiGeometry *greatparentMultiGeo = geodata_cast<GeoDataMultiGeometry>(greatParentObject)) {
            GeoDataGeometry *parentGeometry = static_cast<GeoDataGeometry*>( parentObject );
//                mDebug() << "parent " << childObject->nodeType() << "(" << childObject << ") = "
//                        << parentObject->nodeType() << "[" << greatParentItem->childPosition( parentGeometry ) << "](" << parentObject << ")";
            return createIndex( greatparentMultiGeo->childPosition( parentGeometry ), 0, parentObject );
        }

        if (GeoDataTour *tour = geodata_cast<GeoDataTour>(greatParentObject)) {
            return createIndex( 0, 0, tour->playlist() );
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
        if (auto feature = dynamic_cast<GeoDataFeature *>(object)) {
            bool bValue = value.toBool();
            if (auto pfolder = geodata_cast<GeoDataFolder>(feature->parent())) {
                if ( pfolder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder) {
                    if ( bValue ) {
                        QVector< GeoDataFeature * >::Iterator i = pfolder->begin();
                        for(; i < pfolder->end(); ++i) {
                            (*i)->setVisible( false );
                        }
                    }
                }
            }
            if (auto folder = geodata_cast<GeoDataFolder>(object)) {
                if ( bValue ) {
                } else {
                    if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder 
                      || folder->style()->listStyle().listItemType() == GeoDataListStyle::CheckOffOnly ) {
                        QVector< GeoDataFeature * >::Iterator i = folder->begin();
                        for(; i < folder->end(); ++i) {
                            (*i)->setVisible( false );
                        }
                        folder->setVisible( false );
                    }
                }
            }
            feature->setVisible( bValue );
            mDebug() << "setData " << feature->name();
            updateFeature( feature );
            return true;
        }
    } else if ( role == Qt::EditRole ) {
        if (auto feature = dynamic_cast<GeoDataFeature *>(object)) {
            feature->setName( value.toString() );
            mDebug() << "setData " << feature->name() << " " << value.toString();
            updateFeature( feature );
            return true;
        }
    }

    return false;
}

Qt::ItemFlags GeoDataTreeModel::flags ( const QModelIndex & index ) const
{
    if ( !index.isValid() )
        return Qt::NoItemFlags;

    const GeoDataObject *object = static_cast<const GeoDataObject *>(index.internalPointer());

    if (const auto feature = geodata_cast<GeoDataPlacemark>(object)) {
        const GeoDataObject *parent = feature->parent();

        if (const auto parentfolder = geodata_cast<GeoDataFolder>(parent)) {
            if ( parentfolder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder ) {
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
            } else if ( parentfolder->style()->listStyle().listItemType() == GeoDataListStyle::CheckHideChildren ) {
                return nullptr;
            }
        }
    }

    if (const auto folder = geodata_cast<GeoDataFolder>(object)) {
        if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::RadioFolder) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable; 
        } else if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::CheckOffOnly ) {
            QVector<GeoDataFeature *>::ConstIterator i = folder->constBegin();
            bool allVisible = true;
            for (; i < folder->constEnd(); ++i) {
                if( ! (*i)->isVisible() ) {
                    allVisible = false;
                    break;
                }
            }
            if ( allVisible ) {
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable; 
            } else {
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
            }
        } else if ( folder->style()->listStyle().listItemType() == GeoDataListStyle::CheckHideChildren) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable; 
        }
    }

    if (geodata_cast<GeoDataTour>(object)) {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    if (dynamic_cast<const GeoDataFeature *>(object)) {
        const GeoDataObject *parent = object;
        while (!geodata_cast<GeoDataDocument>(parent)) {
            parent = parent->parent();
        }
        const GeoDataDocument *document = static_cast<const GeoDataDocument *>(parent);
        if( document->documentRole() == UserDocument ) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
        }

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }

    if (geodata_cast<GeoDataWait>(object)
         || geodata_cast<GeoDataFlyTo>(object)
         || geodata_cast<GeoDataPlaylist>(object)) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QModelIndex GeoDataTreeModel::index(const GeoDataObject *object) const
{
    if ( object == nullptr )
        return QModelIndex();

    //It first runs bottom-top, storing every ancestor of the object, and
    //then goes top-down retrieving the QModelIndex of every ancestor until reaching the
    //index of the requested object.
    //The TreeModel contains: Documents, Folders, Placemarks, MultiGeometries
    //and Geometries that are children of MultiGeometries
    //You can not call this function with an element that does not belong to the tree

    Q_ASSERT(geodata_cast<GeoDataFolder>(object)
              || geodata_cast<GeoDataDocument>(object)
              || geodata_cast<GeoDataPlacemark>(object)
              || geodata_cast<GeoDataTour>(object)
              || ( geodata_cast<GeoDataPlaylist>(object)
                   && geodata_cast<GeoDataTour>(object->parent()))
              || (geodata_cast<GeoDataWait>(object)
                   && geodata_cast<GeoDataPlaylist>(object->parent()))
              || ( geodata_cast<GeoDataFlyTo>(object)
                   && geodata_cast<GeoDataPlaylist>(object->parent()))
              || (geodata_cast<GeoDataLineString>(object)
                   && geodata_cast<GeoDataMultiGeometry>(object->parent()))
              || (geodata_cast<GeoDataLinearRing>(object)
                   && geodata_cast<GeoDataMultiGeometry>(object->parent()))
              || (geodata_cast<GeoDataPoint>(object)
                   && geodata_cast<GeoDataMultiGeometry>(object->parent()))
              || (geodata_cast<GeoDataPolygon>(object)
                   && geodata_cast<GeoDataMultiGeometry>(object->parent()))
              || geodata_cast<GeoDataMultiGeometry>(object));


    QList<const GeoDataObject *> ancestors;

    const GeoDataObject *itup = object; //Iterator to reach the top of the GeoDataDocument (bottom-up)

    while ( itup && ( itup != d->m_rootDocument ) ) {//We reach up to the rootDocument

        ancestors.append( itup );
        itup = itup->parent() ;
    }

    QModelIndex itdown;
    if ( !ancestors.isEmpty() ) {

        itdown = index(d->m_rootDocument->childPosition(static_cast<const GeoDataFeature *>(ancestors.last())), 0, QModelIndex());//Iterator to go top down

        while ( ( ancestors.size() > 1 ) ) {

            const GeoDataObject *parent = static_cast<const GeoDataObject*>(ancestors.last());

            if (const auto container = dynamic_cast<const GeoDataContainer *>(parent)) {

                ancestors.removeLast();
                itdown = index(container->childPosition(static_cast<const GeoDataFeature *>(ancestors.last())), 0, itdown);
            } else if (geodata_cast<GeoDataPlacemark>(parent)) {
                //The only child of the model is a Geometry or MultiGeometry object
                //If it is a geometry object, we should be on the bottom of the list
                ancestors.removeLast();
                if (geodata_cast<GeoDataMultiGeometry>(ancestors.last()))
                    itdown = index( 0 , 0, itdown );
                else
                    itdown = QModelIndex();

            }  else if (auto multiGeometry = geodata_cast<GeoDataMultiGeometry>(parent)) {
                //The child is one of the geometry children of MultiGeometry
                ancestors.removeLast();
                itdown = index(multiGeometry->childPosition(static_cast<const GeoDataGeometry *>(ancestors.last())), 0, itdown);
            } else if (geodata_cast<GeoDataTour>(parent)) {
                ancestors.removeLast();
                itdown = index( 0, 0, itdown );
            } else if (auto playlist = geodata_cast<GeoDataPlaylist>(parent)) {
                for ( int i=0; i< playlist->size(); i++ )
                {
                    if ( playlist->primitive(i) == ancestors.last() )
                    {
                        ancestors.removeLast();
                        itdown = index( i, 0, itdown );
                        break;
                    }
                }
            }
            else  {   //If the element is not found on the tree, it will be added under m_rootDocument
                itdown = QModelIndex();
                break;
            }
        }
    }
    return itdown;
}

QItemSelectionModel *GeoDataTreeModel::selectionModel()
{
    return &d->m_selectionModel;
}

int GeoDataTreeModel::addFeature( GeoDataContainer *parent, GeoDataFeature *feature, int row )
{
    if ( parent && feature ) {

        QModelIndex modelindex = index( parent );
            //index(GeoDataObject*) returns QModelIndex() if parent == m_rootDocument
            //or if parent is not found on the tree.
            //We must check that we are in top of the tree (then QModelIndex() is
            //the right parent to insert the child object) or that we have a valid QModelIndex

        if( ( parent == d->m_rootDocument ) || modelindex.isValid() )
        {
            if( row < 0 || row > parent->size()) {
                row = parent->size();
            }
            beginInsertRows( modelindex , row , row );
            parent->insert( row, feature );
            d->checkParenting( parent );
            endInsertRows();
            emit added(feature);
        }
        else
            qWarning() << "GeoDataTreeModel::addFeature (parent " << parent << " - feature" << feature << ") : parent not found on the TreeModel";
    }
    else
        qWarning() << "Null pointer in call to GeoDataTreeModel::addFeature (parent " << parent << " - feature" << feature << ")";
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
        GeoDataFeature *feature = parent->child( row );
        parent->remove( row );
        emit removed(feature);
        endRemoveRows();
        return true;
    }
    return false; //Tried to remove a row that is not contained in the parent.
}

int GeoDataTreeModel::removeFeature(GeoDataFeature *feature)
{
    if ( feature && ( feature!=d->m_rootDocument ) )  {

        if (!feature->parent()) {
            return -1;
        }

        //We check to see we are not removing the
        //top level element m_rootDocument

        GeoDataObject *parent = static_cast< GeoDataObject* >( feature->parent() );

        if (dynamic_cast<const GeoDataContainer *>(parent)) {

            int row = static_cast< GeoDataContainer* >( feature->parent() )->childPosition( feature );
            if ( row != -1 ) {
                bool removed = removeFeature( static_cast< GeoDataContainer* >( feature->parent() ) , row );
                if( removed ) {
                    return row;
                }
            }
            //The feature is not contained in the parent it points to
        }
    }
    return -1; //We can not remove the rootDocument
}

void GeoDataTreeModel::updateFeature( GeoDataFeature *feature )
{
    GeoDataContainer *container = static_cast<GeoDataContainer*>( feature->parent() );
    int index = removeFeature( feature );
    Q_ASSERT( index != -1 );
    addFeature( container, feature, index );
}

void GeoDataTreeModel::removeDocument( int index )
{
    removeFeature( d->m_rootDocument, index );
}

void GeoDataTreeModel::removeDocument( GeoDataDocument *document )
{
    removeFeature( document );
}

void GeoDataTreeModel::setRootDocument( GeoDataDocument* document )
{
    beginResetModel();
    if ( d->m_ownsRootDocument ) {
        delete d->m_rootDocument;
    }

    d->m_ownsRootDocument = ( document == nullptr );
    d->m_rootDocument = document ? document : new GeoDataDocument;
    endResetModel();
}

GeoDataDocument * GeoDataTreeModel::rootDocument()
{
    return d->m_rootDocument;
}

int GeoDataTreeModel::addTourPrimitive( const QModelIndex &parent, GeoDataTourPrimitive *primitive, int row )
{
    GeoDataObject *parentObject = static_cast<GeoDataObject*>( parent.internalPointer() );
    if (auto playlist = geodata_cast<GeoDataPlaylist>(parentObject)) {
        if( row == -1 ) {
            row = playlist->size();
        }
        beginInsertRows( parent, row, row );
        playlist->insertPrimitive( row, primitive );
        endInsertRows();
        return row;
    }
    return -1;
}

bool GeoDataTreeModel::removeTourPrimitive( const QModelIndex &parent , int index)
{
    GeoDataObject *parentObject = static_cast<GeoDataObject*>( parent.internalPointer() );
    if (auto playlist = (parent.isValid() ? geodata_cast<GeoDataPlaylist>(parentObject) : nullptr)) {
        if( playlist->size() > index ) {
            beginRemoveRows( parent, index, index );
            playlist->removePrimitiveAt( index );
            endRemoveRows();
            return true;
        }
    }
    return false;
}

bool GeoDataTreeModel::swapTourPrimitives( const QModelIndex &parent, int indexA, int indexB )
{
    GeoDataObject *parentObject = static_cast<GeoDataObject*>( parent.internalPointer() );
    if (auto playlist = (parent.isValid() ? geodata_cast<GeoDataPlaylist>(parentObject) : nullptr)) {
        if( indexA > indexB ) {
            qSwap(indexA, indexB);
        }
        if ( indexB - indexA == 1 ) {
            beginMoveRows( parent, indexA, indexA, parent, indexB+1 );
        } else {
            beginMoveRows( parent, indexA, indexA, parent, indexB );
            beginMoveRows( parent, indexB, indexB, parent, indexA );
        }
        playlist->swapPrimitives( indexA, indexB );
        if( indexB - indexA != 1 ) {
            endMoveRows();
        }
        endMoveRows();
        return true;
    }
    return false;
}

#include "moc_GeoDataTreeModel.cpp"
