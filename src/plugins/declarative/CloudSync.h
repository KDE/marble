//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CLOUDSYNC_H
#define CLOUDSYNC_H

#include "cloudsync/CloudSyncManager.h"
#include "cloudsync/MergeItem.h"

#include <QObject>
#include <QVector>

class MarbleWidget;

class CloudSync : public QObject
{

    Q_OBJECT
    Q_PROPERTY( QObject* routeModel READ routeModel NOTIFY routeModelChanged )
    Q_PROPERTY( MarbleWidget* map READ map WRITE setMap NOTIFY mapChanged )
    Q_PROPERTY( QString owncloudServer READ owncloudServer WRITE setOwncloudServer NOTIFY owncloudServerChanged )
    Q_PROPERTY( QString owncloudUsername READ owncloudUsername WRITE setOwncloudUsername NOTIFY owncloudUsernameChanged )
    Q_PROPERTY( QString owncloudPassword READ owncloudPassword WRITE setOwncloudPassword NOTIFY owncloudPasswordChanged )

public:
    explicit CloudSync( QObject *parent = 0 );
    ~CloudSync();

    QObject* routeModel();

    MarbleWidget* map();
    void setMap( MarbleWidget *map );

    QString owncloudServer();
    void setOwncloudServer( const QString &server );

    QString owncloudUsername();
    void setOwncloudUsername( const QString &username );

    QString owncloudPassword();
    void setOwncloudPassword( const QString &password );

public slots:
    void uploadRoute();
    void uploadRoute( const QString &identifier );
    void openRoute( const QString &identifier );
    void downloadRoute( const QString &identifier );
    void removeRouteFromDevice( const QString &identifier );
    void deleteRouteFromCloud( const QString &identifier );
    void syncBookmarks();
    void resolveConflict( Marble::MergeItem *item );

signals:
    void mapChanged();
    void routeModelChanged();
    void cloudSyncManagerChanged();
    void owncloudServerChanged();
    void owncloudUsernameChanged();
    void owncloudPasswordChanged();
    void mergeConflict( Marble::MergeItem *item );

private:
    class Private;
    Private *d;
};

#endif // CLOUDSYNC_H
