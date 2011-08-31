//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "TrackerPluginModel.h"

#include "CacheStoragePolicy.h"
#include "HttpDownloadManager.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "PluginManager.h"
#include "TrackerPluginItem.h"

#include <QtCore/QTimer>

namespace Marble
{

class TrackerPluginModelPrivate
{
public:
    TrackerPluginModelPrivate( TrackerPluginModel *parent, GeoDataTreeModel *treeModel )
        : m_parent( parent ),
          m_treeModel( treeModel ),
          m_document( new GeoDataDocument() ),
          m_storagePolicy( MarbleDirs::localPath() + "/cache/" ),
          m_timer( new QTimer() )
    {
    }

    void downloaded(const QString &relativeUrlString, const QString &id)
    {
        Q_UNUSED( relativeUrlString );

        m_parent->parseFile( id, m_storagePolicy.data( id ) );
    }

    void update()
    {
        foreach( TrackerPluginItem *item, m_parent->items() ) {
            item->update();
        }
    }

    TrackerPluginModel *m_parent;
    GeoDataTreeModel *m_treeModel;
    GeoDataDocument *m_document;
    QHash<QString, TrackerPluginItem *> m_itemHash;
    CacheStoragePolicy m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
    QTimer *m_timer;
};

TrackerPluginModel::TrackerPluginModel( GeoDataTreeModel *treeModel, const PluginManager *pluginManager )
    : d( new TrackerPluginModelPrivate( this, treeModel ) )
{
    d->m_document->setDocumentRole( TrackingDocument );
    d->m_treeModel->addDocument( d->m_document );

    connect( d->m_timer, SIGNAL(timeout()), this, SLOT(update()) );
    d->update();
    d->m_timer->start( 1000 );
        
    d->m_downloadManager = new HttpDownloadManager( &d->m_storagePolicy, pluginManager );
    connect( d->m_downloadManager, SIGNAL(downloadComplete(QString,QString)),
             this, SLOT(downloaded(QString,QString)) );
}

TrackerPluginItem *TrackerPluginModel::item( const QString &name )
{
    if ( !d->m_itemHash.contains( name ) ) {
        return 0;
    }
    return d->m_itemHash[ name ];
}

QList<TrackerPluginItem *> TrackerPluginModel::items()
{
    return d->m_itemHash.values();
}

void TrackerPluginModel::addItem( TrackerPluginItem *mark )
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

bool TrackerPluginModel::removeItem( const QString &name )
{
    if ( !d->m_itemHash.contains( name ) ) {
        return false;
    }

    d->m_document->remove( d->m_document->childPosition( d->m_itemHash[ name ]->placemark() ) );
    return true;
}

void TrackerPluginModel::beginUpdateItems()
{
    d->m_treeModel->removeDocument( d->m_document );
}

void TrackerPluginModel::endUpdateItems()
{
    d->m_treeModel->addDocument( d->m_document );
}

void TrackerPluginModel::downloadFile(const QUrl &url, const QString &id)
{
    d->m_downloadManager->addJob( url, id, id, DownloadBrowse );
}

void TrackerPluginModel::parseFile( const QString &id, const QByteArray &file )
{
    Q_UNUSED( id );
    Q_UNUSED( file );
}

}

#include "TrackerPluginModel.moc"
