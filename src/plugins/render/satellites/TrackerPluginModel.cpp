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
#include "TrackerPluginItem.h"

namespace Marble
{

class TrackerPluginModelPrivate
{
public:
    TrackerPluginModelPrivate( TrackerPluginModel *parent, GeoDataTreeModel *treeModel )
        : m_parent( parent ),
          m_enabled( false ),
          m_treeModel( treeModel ),
          m_document( new GeoDataDocument() ),
          m_storagePolicy(MarbleDirs::localPath() + QLatin1String("/cache/")),
          m_downloadManager( 0 )
    {
    }

    ~TrackerPluginModelPrivate()
    {
        delete m_document;
        qDeleteAll( m_itemVector );
        delete m_downloadManager;
    }

    void downloaded(const QString &relativeUrlString, const QString &id)
    {
        Q_UNUSED( relativeUrlString );

        m_parent->parseFile( id, m_storagePolicy.data( id ) );
    }

    void update()
    {
        for( TrackerPluginItem *item: m_itemVector ) {
            item->update();
        }
    }

    void updateDocument()
    {
        // we cannot use ->clear() since its implementation
        // will delete all items
        for( TrackerPluginItem *item: m_itemVector ) {
            int idx = m_document->childPosition( item->placemark() );
            if( item->isEnabled() && idx == -1 ) {
                m_document->append( item->placemark() );
            }
            if( !item->isEnabled() && idx > -1 ) {
                m_document->remove( idx );
            }
        }
    }

    TrackerPluginModel *m_parent;
    bool m_enabled;
    GeoDataTreeModel *m_treeModel;
    GeoDataDocument *m_document;
    CacheStoragePolicy m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
    QVector<TrackerPluginItem *> m_itemVector;
};

TrackerPluginModel::TrackerPluginModel( GeoDataTreeModel *treeModel )
    : d( new TrackerPluginModelPrivate( this, treeModel ) )
{
    d->m_document->setDocumentRole( TrackingDocument );
    d->m_document->setName(QStringLiteral("Satellites"));
    if( d->m_enabled ) {
        d->m_treeModel->addDocument( d->m_document );
    }

    d->m_downloadManager = new HttpDownloadManager( &d->m_storagePolicy );
    connect( d->m_downloadManager, SIGNAL(downloadComplete(QString,QString)),
             this, SLOT(downloaded(QString,QString)) );
}

TrackerPluginModel::~TrackerPluginModel()
{
    if( d->m_enabled ) {
        d->m_treeModel->removeDocument( d->m_document );
    }
    delete d;
}

void TrackerPluginModel::enable( bool enabled )
{
    if( enabled == d->m_enabled ) {
        return;
    }
    if( enabled ) {
        d->m_treeModel->addDocument( d->m_document );
    } else {
        d->m_treeModel->removeDocument( d->m_document );
    }
    d->m_enabled = enabled;
}

void TrackerPluginModel::addItem( TrackerPluginItem *mark )
{
    d->m_document->append( mark->placemark() );
    d->m_itemVector.append( mark );
}

QVector<TrackerPluginItem*> TrackerPluginModel::items() const
{
    return d->m_itemVector;
}

void TrackerPluginModel::clear()
{
    beginUpdateItems();
    qDeleteAll( d->m_itemVector );
    d->m_itemVector.clear();
    d->m_itemVector.squeeze();
    d->m_document->clear();
    endUpdateItems();
}

void TrackerPluginModel::beginUpdateItems()
{
    if( d->m_enabled ) {
        d->m_treeModel->removeDocument( d->m_document );
    }

    emit itemUpdateStarted();
}

void TrackerPluginModel::endUpdateItems()
{
    if( d->m_enabled ) {
        d->updateDocument();
        d->m_treeModel->addDocument( d->m_document );
    }

    emit itemUpdateEnded();
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

} // namespace Marble

#include "moc_TrackerPluginModel.cpp"
