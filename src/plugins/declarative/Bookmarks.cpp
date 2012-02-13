//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Bookmarks.h"

#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "MarbleMath.h"
#include "MarblePlacemarkModel.h"
#include "BookmarkManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFolder.h"
#include "GeoDataTypes.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTreeModel.h"
#include "kdescendantsproxymodel.h"

#include <QtGui/QSortFilterProxyModel>

using namespace Marble;

Bookmarks::Bookmarks( QObject* parent ) : QObject( parent ),
    m_marbleWidget( 0 ), m_proxyModel( 0 )
{
    // nothing to do
}

void Bookmarks::setMarbleWidget( ::MarbleWidget* widget )
{
    m_marbleWidget = widget;
}

bool Bookmarks::isBookmark( qreal longitude, qreal latitude )
{
    if ( !m_marbleWidget || !m_marbleWidget->model()->bookmarkManager() ) {
        return false;
    }

    BookmarkManager* manager = m_marbleWidget->model()->bookmarkManager();
    GeoDataDocument *bookmarks = manager->document();
    GeoDataCoordinates const compareTo( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
    foreach( const GeoDataFolder* folder, bookmarks->folderList() ) {
        foreach( const GeoDataPlacemark * const placemark, folder->placemarkList() ) {
            if ( distanceSphere( placemark->coordinate(), compareTo ) * EARTH_RADIUS < 5 ) {
                return true;
            }
        }
    }

    return false;
}

void Bookmarks::addBookmark( qreal longitude, qreal latitude, const QString &name, const QString &folderName )
{
    if ( !m_marbleWidget || !m_marbleWidget->model()->bookmarkManager() ) {
        return;
    }

    BookmarkManager* manager = m_marbleWidget->model()->bookmarkManager();
    GeoDataDocument *bookmarks = manager->document();
    GeoDataContainer *target = 0;
    foreach( GeoDataFolder* const folder, bookmarks->folderList() ) {
        if ( folder->name() == folderName ) {
            target = folder;
            break;
        }
    }

    if ( !target ) {
        manager->addNewBookmarkFolder( bookmarks, folderName );

        foreach( GeoDataFolder* const folder, bookmarks->folderList() ) {
            if ( folder->name() == folderName ) {
                target = folder;
                break;
            }
        }

        Q_ASSERT( target );
    }

    GeoDataPlacemark placemark;
    GeoDataCoordinates coordinate( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
    placemark.setCoordinate( coordinate );
    placemark.setName( name );
    GeoDataLookAt* lookat = new GeoDataLookAt;
    lookat->setCoordinates( coordinate );
    lookat->setRange( 750 );
    placemark.extendedData().addValue( GeoDataData( "isBookmark", true ) );

    manager->addBookmark( target, placemark );
}

void Bookmarks::removeBookmark( qreal longitude, qreal latitude )
{
    if ( !m_marbleWidget || !m_marbleWidget->model()->bookmarkManager() ) {
        return;
    }

    BookmarkManager* manager = m_marbleWidget->model()->bookmarkManager();
    GeoDataDocument *bookmarks = manager->document();
    GeoDataCoordinates const compareTo( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
    foreach( const GeoDataFolder* folder, bookmarks->folderList() ) {
        foreach( GeoDataPlacemark * placemark, folder->placemarkList() ) {
            if ( distanceSphere( placemark->coordinate(), compareTo ) * EARTH_RADIUS < 5 ) {
                manager->removeBookmark( placemark );
                return;
            }
        }
    }
}

BookmarksModel *Bookmarks::model()
{
    if ( !m_proxyModel && m_marbleWidget && m_marbleWidget->model()->bookmarkManager() ) {
        BookmarkManager* manager = m_marbleWidget->model()->bookmarkManager();
        GeoDataTreeModel* model = new GeoDataTreeModel( this );
        model->setRootDocument( manager->document() );

        KDescendantsProxyModel* flattener = new KDescendantsProxyModel( this );
        flattener->setSourceModel( model );

        m_proxyModel = new BookmarksModel( this );
        m_proxyModel->setFilterFixedString( GeoDataTypes::GeoDataPlacemarkType );
        m_proxyModel->setFilterKeyColumn( 1 );
        m_proxyModel->setSourceModel( flattener );
    }

    return m_proxyModel;
}

BookmarksModel::BookmarksModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
    // Workaround for https://bugreports.qt-project.org/browse/QTCOMPONENTS-1206
    QHash<int,QByteArray> roles = roleNames();
    roles[Qt::DisplayRole] = "name";
    setRoleNames( roles );

    connect( this, SIGNAL( layoutChanged() ), this, SIGNAL( countChanged() ) );
    connect( this, SIGNAL( modelReset() ), this, SIGNAL( countChanged() ) );
    connect( this, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SIGNAL( countChanged() ) );
    connect( this, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SIGNAL( countChanged() ) );
}

int BookmarksModel::count() const
{
    return rowCount();
}

qreal BookmarksModel::longitude( int idx )
{
    if ( idx >= 0 && idx < rowCount() ) {
        QVariant const value = data( index( idx, 0 ), MarblePlacemarkModel::CoordinateRole );
        GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( value );
        return coordinates.longitude( GeoDataCoordinates::Degree );
    }
    return 0.0;
}

qreal BookmarksModel::latitude( int idx )
{
    if ( idx >= 0 && idx < rowCount() ) {
        QVariant const value = data( index( idx, 0 ), MarblePlacemarkModel::CoordinateRole );
        GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( value );
        return coordinates.latitude( GeoDataCoordinates::Degree );
    }
    return 0.0;
}

QString BookmarksModel::name( int idx )
{
    if ( idx >= 0 && idx < rowCount() ) {
        return data( index( idx, 0 ) ).toString();
    }
    return QString();
}

#include "Bookmarks.moc"
