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
    Private( CloudSyncManager *parent );

    bool m_workOffline;

    bool m_syncEnabled;
    bool m_routeSyncEnabled;
    bool m_bookmarkSyncEnabled;

    QString m_ownloudServer;
    QString m_owncloudUsername;
    QString m_owncloudPassword;

    BookmarkSyncManager m_bookmarkSyncManager;
};

CloudSyncManager::Private::Private( CloudSyncManager* parent ) :
    m_workOffline( false ),
    m_syncEnabled( false ),
    m_routeSyncEnabled( true ),
    m_bookmarkSyncEnabled( true ),
    m_ownloudServer(),
    m_owncloudUsername(),
    m_owncloudPassword(),
    m_bookmarkSyncManager( parent )
{
}

CloudSyncManager::CloudSyncManager( QObject *parent ) :
    QObject( parent ),
    d( new Private( this ) )
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

CloudSyncManager::Backend CloudSyncManager::backend() const
{
    return Owncloud;
}

bool CloudSyncManager::isSyncEnabled() const
{
    return d->m_syncEnabled;
}

bool CloudSyncManager::isRouteSyncEnabled() const
{
    return d->m_routeSyncEnabled;
}

bool CloudSyncManager::isBookmarkSyncEnabled() const
{
    return d->m_bookmarkSyncEnabled;
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

void CloudSyncManager::setRouteSyncEnabled( bool enabled )
{
    if ( d->m_routeSyncEnabled != enabled ) {
        d->m_routeSyncEnabled = enabled;
        emit routeSyncEnabledChanged( d->m_routeSyncEnabled );
    }
}

void CloudSyncManager::setBookmarkSyncEnabled( bool enabled )
{
    if ( d->m_bookmarkSyncEnabled != enabled ) {
        d->m_bookmarkSyncEnabled = enabled;
        emit bookmarkSyncEnabledChanged( d->m_bookmarkSyncEnabled );
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

BookmarkSyncManager *CloudSyncManager::bookmarkSyncManager()
{
  return &d->m_bookmarkSyncManager;
}

}

#include "CloudSyncManager.moc"
