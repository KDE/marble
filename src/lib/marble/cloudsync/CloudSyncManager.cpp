//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "CloudSyncManager.h"
#include "BookmarkSyncManager.h"

namespace Marble
{

class CloudSyncManager::Private {

public:
    Private();
    ~Private();

    bool m_workOffline;

    bool m_syncEnabled;

    QString m_ownloudServer;
    QString m_owncloudUsername;
    QString m_owncloudPassword;

    RouteSyncManager* m_routeSyncManager;
    BookmarkSyncManager* m_bookmarkSyncManager;
};

CloudSyncManager::Private::~Private()
{
    delete m_routeSyncManager;
    delete m_bookmarkSyncManager;
}

CloudSyncManager::Private::Private() :
    m_workOffline( false ),
    m_syncEnabled( false ),
    m_ownloudServer(),
    m_owncloudUsername(),
    m_owncloudPassword(),
    m_routeSyncManager( 0 ),
    m_bookmarkSyncManager( 0 )
{
}

CloudSyncManager::CloudSyncManager( QObject *parent ) :
    QObject( parent ),
    d( new Private )
{
}

CloudSyncManager::~CloudSyncManager()
{
  delete d;
}

bool CloudSyncManager::workOffline() const
{
    return d->m_workOffline;
}

void CloudSyncManager::setWorkOffline( bool offline )
{
    if ( offline != d->m_workOffline ) {
        d->m_workOffline = offline;
        emit workOfflineChanged( d->m_workOffline );
    }
}

bool CloudSyncManager::isSyncEnabled() const
{
    return d->m_syncEnabled;
}

QString CloudSyncManager::owncloudServer() const
{
    return d->m_ownloudServer;
}

QString CloudSyncManager::owncloudUsername() const
{
    return d->m_owncloudUsername;
}

QString CloudSyncManager::owncloudPassword() const
{
    return d->m_owncloudPassword;
}

void CloudSyncManager::setSyncEnabled( bool enabled )
{
    if ( d->m_syncEnabled != enabled ) {
        d->m_syncEnabled = enabled;
        emit syncEnabledChanged( d->m_syncEnabled );
    }
}

void CloudSyncManager::setOwncloudServer( const QString &server )
{
    if ( d->m_ownloudServer != server ) {
        d->m_ownloudServer = server;
        emit owncloudServerChanged( d->m_ownloudServer );
        emit apiUrlChanged( apiUrl() );
    }
}

void CloudSyncManager::setOwncloudUsername( const QString &username )
{
    if ( d->m_owncloudUsername != username ) {
        d->m_owncloudUsername = username;
        emit owncloudUsernameChanged( d->m_owncloudUsername );
        emit apiUrlChanged( apiUrl() );
    }
}

void CloudSyncManager::setOwncloudPassword( const QString &password )
{
    if ( d->m_owncloudPassword != password ) {
        d->m_owncloudPassword = password;
        emit owncloudPasswordChanged( d->m_owncloudPassword );
        emit apiUrlChanged( apiUrl() );
    }
}

QString CloudSyncManager::apiPath() const
{
    return "index.php/apps/marble/api/v1";
}

QUrl CloudSyncManager::apiUrl() const
{
    return QUrl( QString( "http://%0:%1@%2/%3" )
                .arg( owncloudUsername() ).arg( owncloudPassword() )
                 .arg( owncloudServer() ).arg( apiPath() ) );
}

RouteSyncManager *CloudSyncManager::routeSyncManager()
{
    if ( !d->m_routeSyncManager ) {
        d->m_routeSyncManager = new RouteSyncManager( this );
    }

    return d->m_routeSyncManager;
}

BookmarkSyncManager *CloudSyncManager::bookmarkSyncManager()
{
    if ( !d->m_bookmarkSyncManager ) {
        d->m_bookmarkSyncManager = new BookmarkSyncManager( this );
    }

    return d->m_bookmarkSyncManager;
}

}

#include "CloudSyncManager.moc"
