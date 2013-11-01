//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "OwncloudSyncBackend.h"

#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"
#include "GeoDocument.h"
#include "MarbleWidget.h"
#include "RenderPlugin.h"
#include "RoutingModel.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "RoutingManager.h"
#include "GeoDataDocument.h"
#include "CloudRouteModel.h"
#include "GeoDataPlacemark.h"
#include "CloudSyncManager.h"

#include <QNetworkAccessManager>
#include <QScriptValueIterator>
#include <QNetworkRequest>
#include <QScriptEngine>
#include <QFileInfo>
#include <QBuffer>
#include <QTimer>
#include <QDir>

namespace Marble
{

class OwncloudSyncBackend::Private {
    
    public:
        Private( CloudSyncManager* cloudSyncManager );

        QDir m_cacheDir;
        QNetworkAccessManager m_network;
        QNetworkReply *m_routeUploadReply;
        QNetworkReply *m_routeListReply;
        QNetworkReply *m_routeDownloadReply;
        QNetworkReply *m_routeDeleteReply;

        QVector<RouteItem> m_routeList;

        QString m_routeUploadEndpoint;
        QString m_routeListEndpoint;
        QString m_routeDownloadEndpoint;
        QString m_routeDeleteEndpoint;
        QString m_routePreviewEndpoint;

        CloudSyncManager* m_cloudSyncManager;
        QUrl m_apiUrl;
};

OwncloudSyncBackend::Private::Private( CloudSyncManager* cloudSyncManager ) :
    m_cacheDir( MarbleDirs::localPath() + "/cloudsync/cache/routes/" ),
    m_network(),
    m_routeUploadReply(),
    m_routeListReply(),
    m_routeDownloadReply(),
    m_routeDeleteReply(),
    m_routeList(),
    // Route API endpoints
    m_routeUploadEndpoint( "routes/create" ),
    m_routeListEndpoint( "routes" ),
    m_routeDownloadEndpoint( "routes" ),
    m_routeDeleteEndpoint( "routes/delete" ),
    m_routePreviewEndpoint( "routes/preview" ),
    m_cloudSyncManager( cloudSyncManager )
{
}

OwncloudSyncBackend::OwncloudSyncBackend( CloudSyncManager* cloudSyncManager ) :
    d( new Private( cloudSyncManager ) )
{
}

OwncloudSyncBackend::~OwncloudSyncBackend()
{
    delete d;
}

void OwncloudSyncBackend::uploadRoute( const QString &timestamp )
{
    QString word = "----MarbleCloudBoundary";
    QString boundary = QString( "--%0" ).arg( word );
    QNetworkRequest request( endpointUrl( d->m_routeUploadEndpoint ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QString( "multipart/form-data; boundary=%0" ).arg( word ) );

    QByteArray data;
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    // Timestamp part
    data.append( "Content-Disposition: form-data; name=\"timestamp\"" );
    data.append( "\r\n\r\n" );
    data.append( QString( timestamp + "\r\n" ).toUtf8() );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    // Name part
    data.append( "Content-Disposition: form-data; name=\"name\"" );
    data.append( "\r\n\r\n" );
    data.append( routeName( timestamp ).toUtf8() );
    data.append( "\r\n" );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    // Duration part
    data.append( "Content-Disposition: form-data; name=\"duration\"" );
    data.append( "\r\n\r\n" );
    data.append( "0\r\n" );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    // Distance part
    data.append( "Content-Disposition: form-data; name=\"distance\"" );
    data.append( "\r\n\r\n" );
    data.append( "0\r\n" );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    // KML part
    data.append( QString( "Content-Disposition: form-data; name=\"kml\"; filename=\"%0.kml\"" ).arg( timestamp ).toUtf8() );
    data.append( "\r\n" );
    data.append( "Content-Type: application/vnd.google-earth.kml+xml" );
    data.append( "\r\n\r\n" );

    QFile kmlFile( d->m_cacheDir.absolutePath() + QString( "/%0.kml" ).arg( timestamp ) );

    if( !kmlFile.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open " << timestamp << ".kml. Either it has not been saved" <<
                    " to cache for upload or another application removed it from there.";
        return;
    }

    data.append( kmlFile.readAll() );
    data.append( "\r\n" );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    kmlFile.close();

    // Preview part
    data.append( QString( "Content-Disposition: form-data; name=\"preview\"; filename=\"%0.jpg\"" ).arg( timestamp ).toUtf8() );
    data.append( "\r\n" );
    data.append( "Content-Type: image/jpg" );
    data.append( "\r\n\r\n" );

    QByteArray previewBytes;
    QBuffer previewBuffer( &previewBytes );
    QPixmap preview = createPreview( timestamp );
    preview.save( &previewBuffer, "JPG" );

    data.append( previewBytes );
    data.append( "\r\n" );
    data.append( QString( boundary + "\r\n" ).toUtf8() );

    d->m_routeUploadReply = d->m_network.post( request, data );
    connect( d->m_routeUploadReply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(routeUploadProgress(qint64,qint64)) );
}

void OwncloudSyncBackend::downloadRouteList()
{    
    QNetworkRequest request( endpointUrl( d->m_routeListEndpoint ) );
    d->m_routeListReply = d->m_network.get( request );
    connect( d->m_routeListReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(routeListDownloadProgress(qint64,qint64)) );
    connect( d->m_routeListReply, SIGNAL(finished()), this, SLOT(prepareRouteList()) );
}

void OwncloudSyncBackend::downloadRoute( const QString &timestamp )
{
    QNetworkRequest routeRequest( endpointUrl( d->m_routeDownloadEndpoint, timestamp ) );
    d->m_routeDownloadReply = d->m_network.get( routeRequest );
    connect( d->m_routeDownloadReply, SIGNAL(finished()), this, SLOT(saveDownloadedRoute()) );
    connect( d->m_routeDownloadReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(routeDownloadProgress(qint64,qint64)) );
}

void OwncloudSyncBackend::deleteRoute( const QString &timestamp )
{
    QUrl url( endpointUrl( d->m_routeDeleteEndpoint, timestamp ) );
    QNetworkRequest request( url );
    d->m_routeDeleteReply = d->m_network.deleteResource( request );
    connect( d->m_routeDeleteReply, SIGNAL(finished()), this, SIGNAL(routeDeleted()) );
}

QPixmap OwncloudSyncBackend::createPreview( const QString &timestamp )
{
    MarbleWidget *mapWidget = new MarbleWidget;
    foreach( RenderPlugin* plugin, mapWidget->renderPlugins() ) {
        plugin->setEnabled( false );
    }

    mapWidget->setProjection( Mercator );
    mapWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    mapWidget->resize( 512, 512 );

    RoutingManager* manager = mapWidget->model()->routingManager();
    manager->loadRoute( d->m_cacheDir.absolutePath() + QString( "/%0.kml" ).arg( timestamp ) );
    GeoDataLatLonBox const bbox = manager->routingModel()->route().bounds();

    if ( !bbox.isEmpty() ) {
        mapWidget->centerOn( bbox );
    }

    QPixmap pixmap = QPixmap::grabWidget( mapWidget );
    QDir( d->m_cacheDir.absolutePath() ).mkpath( "preview" );
    pixmap.save( d->m_cacheDir.absolutePath() + "/preview/" + timestamp + ".jpg" );

    delete mapWidget;
    return pixmap;
}

QString OwncloudSyncBackend::routeName( const QString &timestamp )
{
    QFile file( d->m_cacheDir.absolutePath() + QString( "/%0.kml" ).arg( timestamp ) );
    file.open( QFile::ReadOnly );

    GeoDataParser parser( GeoData_KML );
    if( !parser.read( &file ) ) {
        mDebug() << "Could not read " << timestamp << ".kml. Timestamp will be used as "
                    << "route name because of the problem";
        return timestamp;
    }
    file.close();

    QString routeName;
    GeoDocument *geoDoc = parser.releaseDocument();
    GeoDataDocument *container = dynamic_cast<GeoDataDocument*>( geoDoc );
    if ( container && container->size() > 0 ) {
        GeoDataFolder *folder = container->folderList().at( 0 );
        foreach ( GeoDataPlacemark *placemark, folder->placemarkList() ) {
            routeName.append( placemark->name() );
            routeName.append( " - " );
        }
    }

    return routeName.left( routeName.length() - 3 );
}

void OwncloudSyncBackend::cancelUpload()
{
    d->m_routeUploadReply->abort();
}

void OwncloudSyncBackend::prepareRouteList()
{
    QString result = d->m_routeListReply->readAll();

    QScriptEngine engine;
    QScriptValue response = engine.evaluate( QString( "(%0)" ).arg( result ) );
    QScriptValue routes = response.property( "data" );

    d->m_routeList.clear();
    
    if( routes.isArray() ) {
        QScriptValueIterator iterator( routes );
        
        while( iterator.hasNext() ) {
            iterator.next();
            
            RouteItem route;
            route.setIdentifier( iterator.value().property( "timestamp" ).toString() );
            route.setName ( iterator.value().property( "name" ).toString() );
            route.setDistance( iterator.value().property( "distance" ).toString() );
            route.setDuration( iterator.value().property( "duration" ).toString() );
            route.setPreviewUrl( endpointUrl( d->m_routePreviewEndpoint, route.identifier() ) );
            route.setOnCloud( true );
            
            d->m_routeList.append( route );
        }
    }
    
    // FIXME Find why an empty item added to the end.
    if( !d->m_routeList.isEmpty() ) {
        d->m_routeList.remove( d->m_routeList.count() - 1 );
    }

    emit routeListDownloaded( d->m_routeList );
}

void OwncloudSyncBackend::saveDownloadedRoute()
{
    QString timestamp = QFileInfo( d->m_routeDownloadReply->url().toString() ).fileName();
    
    bool pathCreated = d->m_cacheDir.mkpath( d->m_cacheDir.absolutePath() );
    if ( !pathCreated ) {
        mDebug() << "Couldn't create the path " << d->m_cacheDir.absolutePath() <<
                    ". Check if your user has sufficent permissions for this operation.";
    }
    
    QString kmlFilePath = QString( "%0/%1.kml").arg( d->m_cacheDir.absolutePath(), timestamp );
    QFile kmlFile( kmlFilePath );
    bool fileOpened = kmlFile.open( QFile::ReadWrite );

    if ( !fileOpened ) {
        mDebug() << "Failed to open file" << kmlFilePath << " for writing."
                 <<  " Its directory either is missing or is not writable.";
        return;
    }

    kmlFile.write( d->m_routeDownloadReply->readAll() );
    kmlFile.close();

    QString previewPath = QString( "%0/preview/" ).arg( d->m_cacheDir.absolutePath() );
    bool previewPathCreated = d->m_cacheDir.mkpath( previewPath );
    if ( !previewPathCreated ) {
        mDebug() << "Couldn't create the path " << previewPath <<
                    ". Check if your user has sufficent permissions for this operation.";
    }

    QString previewFilePath = QString( "%0/preview/%1.jpg").arg( d->m_cacheDir.absolutePath(), timestamp );
    QFile previewFile( previewFilePath );
    bool previewFileOpened = previewFile.open( QFile::ReadWrite );

    if ( !previewFileOpened ) {
        mDebug() << "Failed to open file" << previewFilePath << "for writing."
                 <<  " Its directory either is missing or is not writable.";
        return;
    }

    QPixmap preview = createPreview( timestamp );
    preview.save( &previewFile, "JPG" );
    previewFile.close();

    emit routeDownloaded();
}

void OwncloudSyncBackend::setApiUrl( const QUrl &apiUrl )
{
    d->m_apiUrl = apiUrl;
}

QUrl OwncloudSyncBackend::endpointUrl( const QString &endpoint )
{
    QString endpointUrl = QString( "%0/%1" ).arg( d->m_apiUrl.toString() ).arg( endpoint );
    return QUrl( endpointUrl );
}

QUrl OwncloudSyncBackend::endpointUrl( const QString &endpoint, const QString &parameter )
{
    QString endpointUrl = QString( "%0/%1/%2" ).arg( d->m_apiUrl.toString() ).arg( endpoint ).arg( parameter );
    return QUrl( endpointUrl );
}

void OwncloudSyncBackend::removeFromCache( const QDir &cacheDir, const QString &timestamp )
{
    bool fileRemoved = QFile( QString( "%0/%1.kml" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    bool previewRemoved = QFile( QString( "%0/preview/%1.jpg" ).arg( cacheDir.absolutePath(), timestamp ) ).remove();
    if ( !fileRemoved || !previewRemoved ) {
        mDebug() << "Failed to remove locally cached route " << timestamp << ". It might "
                    "have been removed already, or its directory is missing / not writable.";
    }

    emit removedFromCache( timestamp );
}

}

#include "OwncloudSyncBackend.moc"
