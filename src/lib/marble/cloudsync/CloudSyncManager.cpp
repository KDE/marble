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
#include "RouteSyncManager.h"

namespace Marble
{

class Q_DECL_HIDDEN CloudSyncManager::Private {

public:
    Private();
    ~Private();

    bool m_workOffline;

    bool m_syncEnabled;

    QString m_owncloudProtocol;
    QString m_ownloudServer;
    QString m_owncloudUsername;
    QString m_owncloudPassword;
    QString m_status;
    CloudSyncManager::Status m_status_type;

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
    m_owncloudProtocol( "http" ),
    m_ownloudServer(),
    m_owncloudUsername(),
    m_owncloudPassword(),
    m_status(),
    m_status_type( CloudSyncManager::Unknown),
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
    return d->m_owncloudProtocol + d->m_ownloudServer;
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

void CloudSyncManager::setOwncloudCredentials( const QString &server, const QString &user, const QString &password )
{
    QUrl const oldApiUrl = apiUrl();
    blockSignals( true );
    setOwncloudServer( server );
    setOwncloudUsername( user );
    setOwncloudPassword( password );
    blockSignals( false );
    if ( oldApiUrl != apiUrl() ) {
        emit apiUrlChanged( apiUrl() );
    }
}

void CloudSyncManager::setOwncloudServer( const QString &server )
{
    QString const oldProtocol = d->m_owncloudProtocol;
    QString const oldServer = d->m_ownloudServer;

    if (server.startsWith(QLatin1String("http://"))) {
        d->m_owncloudProtocol = "http://";
        d->m_ownloudServer = server.mid( 7 );
    } else if (server.startsWith(QLatin1String("https://"))) {
        d->m_owncloudProtocol = "https://";
        d->m_ownloudServer = server.mid ( 8 );
    } else {
        d->m_owncloudProtocol = "http://";
        d->m_ownloudServer = server;
    }

    if ( oldServer != d->m_ownloudServer ) {
        emit owncloudServerChanged( owncloudServer() );
        emit apiUrlChanged( apiUrl() );
    } else if ( oldProtocol != d->m_owncloudProtocol ) {
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

void CloudSyncManager::setStatus( const QString &status, CloudSyncManager::Status status_type )
{
    if( d->m_status != status && d->m_status_type != status_type) {
        d->m_status = status;
        d->m_status_type = status_type;
        emit statusChanged( d->m_status );
    }
}

CloudSyncManager::Status CloudSyncManager::status() const
{
    return d->m_status_type;
}

QString CloudSyncManager::statusDescription() const
{
    return d->m_status;
}

QString CloudSyncManager::apiPath() const
{
    return "index.php/apps/marble/api/v1";
}

QUrl CloudSyncManager::apiUrl() const
{
    return QUrl( QString( "%0%1:%2@%3/%4" )
                 .arg( d->m_owncloudProtocol )
                 .arg( d->m_owncloudUsername ).arg( d->m_owncloudPassword )
                 .arg( d->m_ownloudServer ).arg( apiPath() ) );
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

#include "moc_CloudSyncManager.cpp"
