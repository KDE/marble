//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "RouteSyncManager.h"

#include "GeoDataParser.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "GeoDataFolder.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "CloudRouteModel.h"
#include "CloudRoutesDialog.h"
#include "CloudSyncManager.h"
#include "OwncloudSyncBackend.h"
#include "RouteItem.h"
#include "RoutingManager.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QIcon>
#include <QPointer>

namespace Marble
{

/**
 * Private class for RouteSyncManager.
 */
class Q_DECL_HIDDEN RouteSyncManager::Private {
public:
    Private( CloudSyncManager *cloudSyncManager );

    bool m_routeSyncEnabled;
    CloudSyncManager *m_cloudSyncManager;
    RoutingManager *m_routingManager;
    CloudRouteModel *m_model;

    QDir m_cacheDir;
    OwncloudSyncBackend m_owncloudBackend;
    QVector<RouteItem> m_routeList;
};

RouteSyncManager::Private::Private( CloudSyncManager *cloudSyncManager ) :
    m_routeSyncEnabled( false ),
    m_cloudSyncManager( cloudSyncManager ),
    m_routingManager( 0 ),
    m_model( new CloudRouteModel() ),
    m_owncloudBackend( cloudSyncManager )
{
    m_cacheDir = QDir(MarbleDirs::localPath() + QLatin1String("/cloudsync/cache/routes/"));
}

RouteSyncManager::RouteSyncManager(CloudSyncManager *cloudSyncManager) :
    d( new Private( cloudSyncManager ) )
{
    connect( &d->m_owncloudBackend, SIGNAL(routeUploadProgress(qint64,qint64)), this, SLOT(updateUploadProgressbar(qint64,qint64)) );
    connect( &d->m_owncloudBackend, SIGNAL(routeListDownloaded(QVector<RouteItem>)), this, SLOT(setRouteModelItems(QVector<RouteItem>)) );
    connect( &d->m_owncloudBackend, SIGNAL(routeListDownloadProgress(qint64,qint64)), this, SIGNAL(routeListDownloadProgress(qint64,qint64)) );
    connect( &d->m_owncloudBackend, SIGNAL(routeDownloadProgress(qint64,qint64)), d->m_model, SLOT(updateProgress(qint64,qint64)) );
    connect( &d->m_owncloudBackend, SIGNAL(routeDownloaded()), this, SLOT(prepareRouteList()) );
    connect( &d->m_owncloudBackend, SIGNAL(routeDeleted()), this, SLOT(prepareRouteList()) );
    connect( &d->m_owncloudBackend, SIGNAL(removedFromCache(QString)), this, SLOT(prepareRouteList()) );
}

RouteSyncManager::~RouteSyncManager()
{
    delete d;
}

void RouteSyncManager::setRoutingManager(RoutingManager *routingManager)
{
    d->m_routingManager = routingManager;
}

bool RouteSyncManager::isRouteSyncEnabled() const
{
    return d->m_routeSyncEnabled && d->m_cloudSyncManager && d->m_cloudSyncManager->isSyncEnabled();
}

void RouteSyncManager::setRouteSyncEnabled( bool enabled )
{
    if ( d->m_routeSyncEnabled != enabled ) {
        d->m_routeSyncEnabled = enabled;
        emit routeSyncEnabledChanged( d->m_routeSyncEnabled );
    }
}

CloudRouteModel* RouteSyncManager::model()
{
    return d->m_model;
}

QString RouteSyncManager::generateTimestamp() const
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    return QString::number( timestamp );
}

QString RouteSyncManager::saveDisplayedToCache() const
{
    if ( !d->m_routingManager ) {
        qWarning() << "RoutingManager instance not set in RouteSyncManager. Cannot save current route.";
        return QString();
    }

    d->m_cacheDir.mkpath( d->m_cacheDir.absolutePath() );
    
    const QString timestamp = generateTimestamp();
    const QString filename = d->m_cacheDir.absolutePath() + QLatin1Char('/') + timestamp + QLatin1String(".kml");
    d->m_routingManager->saveRoute( filename );
    return timestamp;
}

void RouteSyncManager::uploadRoute()
{
    if( !d->m_cloudSyncManager->workOffline() ) {
        d->m_owncloudBackend.uploadRoute( saveDisplayedToCache() );
    }
}

QVector<RouteItem> RouteSyncManager::cachedRouteList() const
{
    QVector<RouteItem> routeList;
    QStringList cachedRoutes = d->m_cacheDir.entryList( QStringList() << "*.kml", QDir::Files );
    foreach ( const QString &routeFilename, cachedRoutes ) {
        QFile file(d->m_cacheDir.absolutePath() + QLatin1Char('/') + routeFilename);
        file.open( QFile::ReadOnly );

        GeoDataParser parser( GeoData_KML );
        if( !parser.read( &file ) ) {
            mDebug() << QLatin1String("Could not read ") + routeFilename;
        }

        file.close();

        QString routeName;
        GeoDocument *geoDoc = parser.releaseDocument();
        GeoDataDocument *container = dynamic_cast<GeoDataDocument*>( geoDoc );
        if ( container && container->size() > 0 ) {
            GeoDataFolder *folder = container->folderList().at( 0 );
            foreach ( GeoDataPlacemark *placemark, folder->placemarkList() ) {
                routeName += placemark->name() + QLatin1String(" - ");
            }
        }

        routeName = routeName.left( routeName.length() - 3 );
        QString timestamp = routeFilename.left( routeFilename.length() - 4 );
        QString distance(QLatin1Char('0'));
        QString duration(QLatin1Char('0'));

        QString previewPath = QString( "%0/preview/%1.jpg" ).arg( d->m_cacheDir.absolutePath(), timestamp );
        QIcon preview;

        if( QFile( previewPath ).exists() ) {
            preview = QIcon( previewPath );
        }

        // Would that work on Windows?
        QUrl previewUrl( QString( "file://%0" ).arg( previewPath ) );

        RouteItem item;
        item.setIdentifier( timestamp );
        item.setName( routeName );
        item.setDistance( distance );
        item.setDistance( duration );
        item.setPreview( preview );
        item.setPreviewUrl( previewUrl );
        item.setOnCloud( false );
        routeList.append( item );
    }

    return routeList;
}

void RouteSyncManager::uploadRoute( const QString &timestamp )
{
    if( !d->m_cloudSyncManager->workOffline() ) {
        d->m_owncloudBackend.uploadRoute( timestamp );
    }
}

void RouteSyncManager::prepareRouteList()
{
    d->m_routeList.clear();

    QVector<RouteItem> cachedRoutes = cachedRouteList();
    foreach( const RouteItem &item, cachedRoutes ) {
        d->m_routeList.append( item );
    }

    if( !d->m_cloudSyncManager->workOffline() ) {
        d->m_owncloudBackend.downloadRouteList();
    } else {
        // If not offline, setRouteModelItems() does this after
        // appending downloaded items to the list.
        d->m_model->setItems( d->m_routeList );
    }
}

void RouteSyncManager::downloadRoute( const QString &timestamp )
{
    d->m_owncloudBackend.downloadRoute( timestamp );
}

void RouteSyncManager::openRoute(const QString &timestamp )
{
    if ( !d->m_routingManager ) {
        qWarning() << "RoutingManager instance not set in RouteSyncManager. Cannot open route " << timestamp;
        return;
    }

    d->m_routingManager->loadRoute( QString( "%0/%1.kml" )
                                    .arg( d->m_cacheDir.absolutePath() )
                                    .arg( timestamp ) );
}

void RouteSyncManager::deleteRoute(const QString &timestamp )
{
    d->m_owncloudBackend.deleteRoute( timestamp );
}

void RouteSyncManager::removeRouteFromCache( const QString &timestamp )
{
    d->m_owncloudBackend.removeFromCache( d->m_cacheDir, timestamp );
}

void RouteSyncManager::updateUploadProgressbar( qint64 sent, qint64 total )
{
    emit routeUploadProgress( sent, total );
    if( sent == total ) {
        prepareRouteList();
    }
}

void RouteSyncManager::setRouteModelItems( const QVector<RouteItem> &routeList )
{
    if( d->m_routeList.count() > 0 ) {
        QStringList cloudRoutes;
        foreach( const RouteItem &item, routeList ) {
            cloudRoutes.append( item.identifier() );
        }

        for( int position = 0; position < d->m_routeList.count(); position++ ) {
            if( cloudRoutes.contains( d->m_routeList.at( position ).identifier() ) ) {
                d->m_routeList[ position ].setOnCloud( true );
            }
        }

        QStringList cachedRoutes;
        foreach( const RouteItem &item, d->m_routeList ) {
            cachedRoutes.append( item.identifier() );
        }

        foreach( const RouteItem &item, routeList ) {
            if( !cachedRoutes.contains( item.identifier() ) ) {
                d->m_routeList.append( item );
            }
        }
    } else {
        foreach( const RouteItem &item, routeList ) {
            d->m_routeList.append( item );
        }
    }

    d->m_model->setItems( d->m_routeList );
}

}

#include "moc_RouteSyncManager.cpp"
