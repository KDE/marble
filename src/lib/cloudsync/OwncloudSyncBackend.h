//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OWNCLOUDSYNCBACKEND_H
#define OWNCLOUDSYNCBACKEND_H

#include "CloudRouteModel.h"
#include "GeoDataDocument.h"
#include "AbstractSyncBackend.h"

#include <QUrl>
#include <QVector>
#include <QNetworkReply>

namespace Marble {

class CloudSyncManager;

class OwncloudSyncBackend : public AbstractSyncBackend
{
    Q_OBJECT
    
public:
    OwncloudSyncBackend( CloudSyncManager* cloudSyncManager );
    ~OwncloudSyncBackend();

    void uploadRoute( const QString &timestamp );
    void downloadRouteList();
    void downloadRoute( const QString &timestamp );
    void deleteRoute( const QString &timestamp );
    QPixmap createPreview( const QString &timestamp );
    QString routeName( const QString &timestamp );

public slots:
    void cancelUpload();

private slots:
    void prepareRouteList();
    void saveDownloadedRoute();
signals:
    void routeListDownloaded( const QVector<RouteItem> &routeList );
    void routeDownloaded();
    void routeDeleted();
    void routeUploadProgress( qint64 sent, qint64 total );
    void routeDownloadProgress( qint64 received, qint64 total );
    void routeListDownloadProgress( qint64 received, qint64 total );
    
private:
    class Private;
    Private *d;
};

}

#endif // OWNCLOUDSYNCBACKEND_H
