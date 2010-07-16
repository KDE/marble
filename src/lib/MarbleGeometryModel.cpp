//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Simon Schmeisser <mail_to_wrt@gmx.de>
//


// Own
#include "MarbleGeometryModel.h"

// Qt
#include <QtCore/QModelIndex>
#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataDocument.h"       // In geodata/data/
#include "GeoDataContainer.h"
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataGeometry.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataObject.h"
#include "GeoDataParser.h"
#include "MarbleDebug.h"

using namespace Marble;

class MarbleGeometryModel::Private
{
 public:
    Private( GeoDataDocument* rootDocument ) : m_rootDocument( rootDocument )
    {
        mapFeature( m_rootDocument );
    };

    Private() : m_rootDocument( 0 ) {};

    void mapGeometry( GeoDataGeometry* geometry )
    {
        /*
        * This function iterates over all geometry objects in a MultiGeometry.
        * This is needed to build up the parent Hash.
        */
        if( !geometry ) return;

        GeoDataMultiGeometry* multiGeometry = static_cast<GeoDataMultiGeometry*>( geometry );
        QVector<GeoDataGeometry> vec = multiGeometry->vector();
        QVector<GeoDataGeometry>::iterator iterator = vec.begin();
        QVector<GeoDataGeometry>::iterator end = vec.end();
        for(; iterator != end; ++iterator ) {
            m_parent.insert( &(*iterator), &(*geometry) );
            if( iterator->geometryId() == GeoDataMultiGeometryId ) mapGeometry( &(*iterator) );
        }
    };

    void mapFeature( GeoDataFeature* feature )
    {
        if( !feature ) return;

        if( feature->featureId() == GeoDataDocumentId || feature->featureId() == GeoDataFolderId ) {
            QVector<GeoDataFeature> featureList = static_cast<const GeoDataContainer*>( feature )->features();
            QVector<GeoDataFeature>::iterator iterator = featureList.begin();
            QVector<GeoDataFeature>::iterator end = featureList.end();
            for(; iterator != end; ++iterator ) {
                m_parent.insert( &( *iterator ), feature );
                mapFeature( &( *iterator ) );
            }
        }

        if( feature->featureId() == GeoDataPlacemarkId ) {
            GeoDataPlacemark placemark = *feature;
            if( placemark.geometry() && placemark.geometry()->geometryId() == GeoDataMultiGeometryId ) {
                m_parent[ placemark.geometry() ] = feature;
                mapGeometry( placemark.geometry() );
            }
        }
    };

    GeoDataDocument* m_rootDocument;
    QHash<GeoDataObject*,GeoDataObject*> m_parent;
};

MarbleGeometryModel::MarbleGeometryModel( QObject *parent )
    : QAbstractItemModel( parent ), d( new Private() )
{
}

MarbleGeometryModel::MarbleGeometryModel( GeoDataDocument *rootDocument, QObject *parent )
    : QAbstractItemModel( parent ), d( new Private( rootDocument ) )
{
}

MarbleGeometryModel::~MarbleGeometryModel()
{
    delete d;
}

void MarbleGeometryModel::setGeoDataRoot( GeoDataDocument* root )
{
    if( d->m_rootDocument != root ) {
        reset();
        d->m_rootDocument = root;
        d->mapFeature( d->m_rootDocument );
    }
}

GeoDataDocument* MarbleGeometryModel::geoDataRoot() const
{
    return d->m_rootDocument;
}

int MarbleGeometryModel::rowCount( const QModelIndex &parent ) const
{
    /* For now we will simply take over the mechanism of TreeModels:
     * each node gives the number of subnodes it has as its rowCount
     */
    GeoDataObject *parentItem;
    if( parent.column() > 0 )
        return 0;

    if( !parent.isValid() ) {
        parentItem = d->m_rootDocument;
    } else {
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );
    }

    int size = 0;
    if( dynamic_cast<GeoDataFeature*>( parentItem ) &&
       (static_cast<GeoDataFeature*>( parentItem )->featureId() == GeoDataDocumentId ||
        static_cast<GeoDataFeature*>( parentItem )->featureId() == GeoDataFolderId ) )
    {
        GeoDataFolder folder( *static_cast<GeoDataFeature*>( parentItem ) );
        size = folder.features().size();
    }

    if( dynamic_cast<GeoDataFeature*>( parentItem ) &&
       static_cast<GeoDataFeature*>( parentItem )->featureId() == GeoDataPlacemarkId )
    {
        /* there is only one GeoDataGeometry Object per Placemark; if Styles
        * are added later, add them here */
        size = 1;
    }

    if( dynamic_cast<GeoDataGeometry*>( parentItem ) &&
        static_cast<GeoDataGeometry*>( parentItem )->geometryId() == GeoDataMultiGeometryId )
    {
        size = static_cast<GeoDataMultiGeometry*>( parentItem )->size();
    }
    return size;
}

QVariant MarbleGeometryModel::data( const QModelIndex &index, int role ) const
{
    GeoDataObject *item;
    if( index.isValid() ) {
        item = static_cast<GeoDataObject*>( index.internalPointer() );
    }
    else /* the rootIndex is invalid */
    {
        item = static_cast<GeoDataObject*>( d->m_rootDocument );
    }

    /* for now simply give back the type of the GeoDataObject
    * there might be a need to extend this, but usually we want
    */
    if( role == Qt::DisplayRole ) {
        if( dynamic_cast<GeoDataFeature*>( item ) ) {
            switch( dynamic_cast<GeoDataFeature*>( item )->featureId() ) {
                case InvalidFeatureId:
                    return QVariant( "InvalidFeature" );
                case GeoDataDocumentId:
                    return QVariant( "GeoDataDocument" );
                case GeoDataFolderId:
                    return QVariant( "GeoDataFolder" );
                case GeoDataPlacemarkId:
                    return QVariant( "GeoDataPlacemark" );
                case GeoDataNetworkLinkId:
                    return QVariant( "GeoDataNetworkLink" );
                case GeoDataScreenOverlayId:
                    return QVariant( "GeoDataScreenOverlay" );
                case GeoDataGroundOverlayId:
                    return QVariant( "GeoDataGroundOverlay" );
            }
        }
        if( dynamic_cast<GeoDataGeometry*>( item ) ) {
            switch( dynamic_cast<GeoDataGeometry*>( item )->geometryId() ) {
                case InvalidGeometryId:
                    return QVariant( "InvalidGeometry" );
                case GeoDataMultiGeometryId:
                    return QVariant( "GeoDataMultiGeometry" );
                case GeoDataLineStringId:
                    return QVariant( "GeoDataLineString" );
                case GeoDataLinearRingId:
                    return QVariant( "GeoDataLinearRing" );
                case GeoDataPolygonId:
                    return QVariant( "GeoDataPolygon" );
                case GeoDataPointId:
                    return QVariant( "GeoDataPoint" );
                case GeoDataModelId:
                    return QVariant( "GeoDataModelId" );
            }
        }
    }

    if( role == Qt::UserRole + 11 ) //equivalent to MarblePlacemarkModel::ObjectPointerRole
    {
        /* return the pointer item as a QVariant of (meta-)type
         * GeoDataObject* */
        QVariant v;
        v.setValue( item );
        return v;
    }

    return QVariant( "GeoDataObject" );
    /* TODO the data is hardcoded and very limited right now,
     * the logic what data is returned might be moved to the items
     * themselves later. Use the following code then:
     * return item->data( index.column() ); */

}

QModelIndex MarbleGeometryModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( !hasIndex( row, column, parent ) )
        return QModelIndex();

    GeoDataObject *parentItem;

    if ( !parent.isValid() )
        parentItem = d->m_rootDocument;
    else
        parentItem = static_cast<GeoDataObject*>( parent.internalPointer() );

    GeoDataObject *childItem = 0;

    /* go down into GeoDataContainers */
    if( dynamic_cast<GeoDataFeature*>( parentItem ) ) {
        switch( dynamic_cast<GeoDataFeature*>( parentItem )->featureId() ) {
            case GeoDataDocumentId:
            case GeoDataFolderId: {
                GeoDataContainer container( *static_cast<GeoDataFeature*>( parentItem ) );
                childItem = &container.features()[ row ];
            }; break;
            case GeoDataPlacemarkId: {
                GeoDataPlacemark placemark( *static_cast<GeoDataPlacemark*>( parentItem ) );
                childItem = placemark.geometry();
            }; break;
            default: break;
        };
    } else if( dynamic_cast<GeoDataGeometry*>( parentItem ) ) {
        if( dynamic_cast<GeoDataGeometry*>( parentItem )->geometryId() == GeoDataMultiGeometryId ) {
            GeoDataMultiGeometry geom = *static_cast<GeoDataGeometry*>( parentItem );
            childItem = &geom.vector()[ row ];
        }
    }

    if ( childItem )
        return createIndex( row, column, childItem );
    else
        return QModelIndex();
}

QModelIndex MarbleGeometryModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return QModelIndex();

    GeoDataObject *childItem = static_cast<GeoDataObject*>( index.internalPointer() );
    GeoDataObject *parentItem = d->m_parent[ childItem ];

    if ( parentItem == d->m_rootDocument )
        return QModelIndex();

    if( parentItem == 0 ){
        /* this shouldn't happen */
        return QModelIndex();
    }

    /* for now leave that as 0, 0 -> this needs some more thoughts as
     * it is simply wrong */
    return createIndex( 0, 0, parentItem );
}

int MarbleGeometryModel::columnCount( const QModelIndex &node ) const
{
    if ( node.isValid() )
        return 1;
        /* TODO when nodes handle their information themselves they will have
         * varying amounts of properties ( or columns ). Right now we only have
         * one information ( type ) per node, so it's hardcoded to "1".
         * static_cast<GeoDataObject*>( node.internalPointer() )->columnCount(); */
    else
        return 1;
}

void MarbleGeometryModel::update()
{
    QAbstractItemModel::reset();
}

#include "MarbleGeometryModel.moc"
