//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "CloudSync.h"

#include "MarbleModel.h"
#include "MarbleDeclarativeWidget.h"
#include "cloudsync/CloudRouteModel.h"
#include "cloudsync/CloudSyncManager.h"
#include "cloudsync/RouteSyncManager.h"
#include "cloudsync/BookmarkSyncManager.h"

class CloudSync::Private
{
public:
    Private();

    MarbleWidget *m_map;
    Marble::CloudRouteModel *m_routeModel;
    Marble::CloudSyncManager m_cloudSyncManager;
};

CloudSync::Private::Private() :
    m_map( 0 ),
    m_routeModel( 0 ),
    m_cloudSyncManager()
{
}

CloudSync::CloudSync( QObject *parent ) :
    QObject( parent ),
    d( new Private() )
{
    connect( &d->m_cloudSyncManager, SIGNAL(owncloudServerChanged(QString)),
             this, SIGNAL(owncloudServerChanged()) );
    connect( &d->m_cloudSyncManager, SIGNAL(owncloudUsernameChanged(QString)),
             this, SIGNAL(owncloudUsernameChanged()) );
    connect( &d->m_cloudSyncManager, SIGNAL(owncloudPasswordChanged(QString)),
             this, SIGNAL(owncloudPasswordChanged()) );
    connect( d->m_cloudSyncManager.bookmarkSyncManager(), SIGNAL(mergeConflict(Marble::MergeItem*)),
             this, SIGNAL(mergeConflict(Marble::MergeItem*)));
}

CloudSync::~CloudSync()
{
    delete d;
}

QObject* CloudSync::routeModel()
{
    return qobject_cast<Marble::CloudRouteModel*>( d->m_cloudSyncManager.routeSyncManager()->model() );
}

MarbleWidget* CloudSync::map()
{
    return d->m_map;
}

void CloudSync::setMap( MarbleWidget *map )
{
    if( d->m_map != map ) {
        d->m_map = map;

        d->m_cloudSyncManager.routeSyncManager()->setRoutingManager( map->model()->routingManager() );
        d->m_cloudSyncManager.bookmarkSyncManager()->setBookmarkManager( map->model()->bookmarkManager() );
        d->m_cloudSyncManager.routeSyncManager()->prepareRouteList();
        emit mapChanged();
    }
}

QString CloudSync::owncloudServer()
{
    return d->m_cloudSyncManager.owncloudServer();
}

void CloudSync::setOwncloudServer( const QString &server )
{
    d->m_cloudSyncManager.setOwncloudServer( server );
}

QString CloudSync::owncloudUsername()
{
    return d->m_cloudSyncManager.owncloudUsername();
}

void CloudSync::setOwncloudUsername( const QString &username )
{
    d->m_cloudSyncManager.setOwncloudUsername( username );
}

QString CloudSync::owncloudPassword()
{
    return d->m_cloudSyncManager.owncloudPassword();
}

void CloudSync::setOwncloudPassword( const QString &password )
{
    d->m_cloudSyncManager.setOwncloudPassword( password );
}

void CloudSync::uploadRoute()
{
    d->m_cloudSyncManager.routeSyncManager()->uploadRoute();
}

void CloudSync::uploadRoute( const QString &identifier )
{
    d->m_cloudSyncManager.routeSyncManager()->uploadRoute( identifier );
}

void CloudSync::openRoute( const QString &identifier )
{
    d->m_cloudSyncManager.routeSyncManager()->openRoute( identifier );
}

void CloudSync::downloadRoute( const QString &identifier )
{
    d->m_cloudSyncManager.routeSyncManager()->downloadRoute( identifier );
}

void CloudSync::removeRouteFromDevice( const QString &identifier )
{
    d->m_cloudSyncManager.routeSyncManager()->removeRouteFromCache( identifier );
}

void CloudSync::deleteRouteFromCloud( const QString &identifier )
{
    d->m_cloudSyncManager.routeSyncManager()->deleteRoute( identifier );
}

void CloudSync::syncBookmarks()
{
    d->m_cloudSyncManager.bookmarkSyncManager()->startBookmarkSync();
}

void CloudSync::resolveConflict( Marble::MergeItem *item )
{
    d->m_cloudSyncManager.bookmarkSyncManager()->resolveConflict( item );
}

#include "CloudSync.moc"
