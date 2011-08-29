//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "TrackerPlugin.h"

#include "CacheStoragePolicy.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "TrackerPluginItem.h"
#include "ViewportParams.h"

#include <QtCore/QTimer>

namespace Marble
{

class TrackerPluginPrivate
{
public:
    TrackerPluginPrivate( TrackerPlugin *parent )
        : m_parent( parent ),
          m_document( new GeoDataDocument() ),
          m_timer( new QTimer() )
    {
        m_document->setDocumentRole( TrackingDocument );
    }

    void downloaded(const QString& relativeUrlString, const QString& id)
    {
        Q_UNUSED( relativeUrlString );

        CacheStoragePolicy *storagePolicy
            = qobject_cast<CacheStoragePolicy*>( m_downloadManager->storagePolicy() );
        if ( storagePolicy ) {
            m_parent->parseFile( id, storagePolicy->data( id ) );
        }
    }

    TrackerPlugin *m_parent;
    GeoDataDocument *m_document;
    QHash<QString, TrackerPluginItem *> m_itemHash;
    HttpDownloadManager *m_downloadManager;
    QTimer *m_timer;
};

TrackerPlugin::TrackerPlugin()
    : d( new TrackerPluginPrivate( this ) )
{
}

void TrackerPlugin::initialize()
{
    CacheStoragePolicy *storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath()
                                                                + "/cache/" );
    d->m_downloadManager = new HttpDownloadManager( storagePolicy, marbleModel()->pluginManager() );
    connect( d->m_downloadManager, SIGNAL(downloadComplete(QString,QString)),
             this, SLOT(downloaded(QString,QString)) );
    d->m_timer->setInterval( 1000 );
    connect( d->m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    update();
    d->m_timer->start();
}


bool TrackerPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    foreach( TrackerPluginItem *item, items() ) {
        if ( viewport->viewLatLonAltBox().contains( item->placemark()->coordinate() ) ) {
            item->render( painter, viewport, renderPos, layer );
        }
    }

    return true;
}

TrackerPluginItem *TrackerPlugin::item( const QString &name )
{
    if ( !d->m_itemHash.contains( name ) ) {
        return 0;
    }
    return d->m_itemHash[ name ];
}

QList<TrackerPluginItem *> TrackerPlugin::items()
{
    return d->m_itemHash.values();
}

void TrackerPlugin::addItem( TrackerPluginItem *mark )
{
    if ( mark == 0 ) {
        return;
    }
    if ( d->m_itemHash.contains( mark->placemark()->name() ) ) {
        d->m_document->remove( d->m_document->childPosition( d->m_itemHash[ mark->placemark()->name() ]->placemark() ) );
    }

    d->m_document->append( mark->placemark() );
    d->m_itemHash[ mark->placemark()->name() ] = mark;
}

bool TrackerPlugin::removeItem( const QString &name )
{
    if ( !d->m_itemHash.contains( name ) ) {
        return false;
    }

    d->m_document->remove( d->m_document->childPosition( d->m_itemHash[ name ]->placemark() ) );
    return true;
}

void TrackerPlugin::update()
{
    foreach( TrackerPluginItem *item, items() ) {
        item->update();
    }
}

void TrackerPlugin::beginUpdatePlacemarks()
{
    marbleModel()->treeModel()->removeDocument( d->m_document );
}

void TrackerPlugin::endUpdatePlacemarks()
{
    marbleModel()->treeModel()->addDocument( d->m_document );
}

void TrackerPlugin::downloadFile(const QUrl &url, const QString &id)
{
    d->m_downloadManager->addJob( url, id, id, DownloadBrowse );
}

void TrackerPlugin::parseFile( const QString &id, const QByteArray &file )
{
    Q_UNUSED( id );
    Q_UNUSED( file );
}

}

#include "TrackerPlugin.moc"
