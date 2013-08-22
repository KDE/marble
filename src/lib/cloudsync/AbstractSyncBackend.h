//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef ABSTRACTSYNCBACKEND_H
#define ABSTRACTSYNCBACKEND_H

#include <QUrl>
#include <QDir>
#include <QObject>

namespace Marble {
/**
 * Base class for synchronization backends.
 * It is going to be extended soon.
 */
class AbstractSyncBackend : public QObject
{
    Q_OBJECT
    
public:
    explicit AbstractSyncBackend( const QUrl &apiUrl, QObject *parent = 0 );
    ~AbstractSyncBackend();

    /**
     * Generates an endpoint URL by appending endpoint name to API URL
     * @param endpoint Endpoint name which will be appended to API URL
     * @return QUrl which can be used for ineractions with API
     */
    QUrl endpointUrl( const QString &endpoint );

    /**
     * Generates an endpoint URL by appending endpoint name and parameter to API URL
     * @param endpoint Endpoint name which will be appended to API URL
     * @param parameter Parameter which will be appended to API URL right after endpoint
     * @return QUrl which can be used for ineractions with API
     */
    QUrl endpointUrl( const QString &endpoint, const QString &parameter );

    /**
     * Removes route with given timestamp from cache
     * @param cacheDir Local synchronization cache directory
     * @param timestamp Timestamp of the route which will be deleted
     */
    void removeFromCache( const QDir &cacheDir, const QString &timestamp );

signals:
    void removedFromCache( const QString &timestamp );

private:
    class Private;
    Private *d;
};

}

#endif // ABSTRACTSYNCBACKEND_H
