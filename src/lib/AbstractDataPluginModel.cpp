//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractDataPluginModel.h"

// Qt
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QtAlgorithms>

// Marble
#include "AbstractDataPluginItem.h"
#include "CacheStoragePolicy.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "HttpDownloadManager.h"
#include "MarbleDataFacade.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

namespace Marble {

const QString descriptionPrefix( "description_" );

// Time between to new description file downloads in ms
const int timeBetweenDownloads = 500;

// Separator to separate the id of the item from the file type
const char fileIdSeparator = '_';
    
class AbstractDataPluginModelPrivate {
 public:
    AbstractDataPluginModelPrivate( const QString& name, AbstractDataPluginModel * parent )
        : m_parent( parent ),
          m_name( name ),
          m_lastBox(),
          m_downloadedBox(),
          m_lastNumber( 0 ),
          m_downloadedNumber( 0 ),
          m_lastDataFacade( 0 ),
          m_downloadTimer( new QTimer( m_parent ) ),
          m_descriptionFileNumber( 0 )
    {
    }
    
    ~AbstractDataPluginModelPrivate() {
        m_itemSet.removeAll( 0 );
        QHash<QString, AbstractDataPluginItem*>::iterator it = m_downloadingItems.begin();
        while( it != m_downloadingItems.end() ) {
            if( !(*it) ) {
                it = m_downloadingItems.erase( it );
            }
            else {
                ++it;
            }
        }
        qDeleteAll( m_itemSet );
        qDeleteAll( m_downloadingItems );
        m_storagePolicy->clearCache();
        delete m_storagePolicy;
    }
    
    AbstractDataPluginModel *m_parent;
    QString m_name;
    GeoDataLatLonAltBox m_lastBox;
    GeoDataLatLonAltBox m_downloadedBox;
    qint32 m_lastNumber;
    qint32 m_downloadedNumber;
    MarbleDataFacade *m_lastDataFacade;
    QString m_downloadedTarget;
    QList<AbstractDataPluginItem*> m_itemSet;
    QHash<QString, AbstractDataPluginItem*> m_downloadingItems;
    QList<AbstractDataPluginItem*> m_displayedItems;
    QTimer *m_downloadTimer;
    quint32 m_descriptionFileNumber;
    
    CacheStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
};

AbstractDataPluginModel::AbstractDataPluginModel( const QString& name, QObject *parent )
    : QObject(  parent ),
      d( new AbstractDataPluginModelPrivate( name, this ) )
{
    // Initializing file and download System
    d->m_storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath()
                                                 + "/cache/" + d->m_name + '/' );
    d->m_downloadManager = new HttpDownloadManager( QUrl(),
                                                    d->m_storagePolicy );
    connect( d->m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
             this,                 SLOT( processFinishedJob( QString , QString ) ) );
    
    // We want to download a new description file every timeBetweenDownloads ms
    connect( d->m_downloadTimer, SIGNAL( timeout() ),
             this,               SLOT( handleChangedViewport() ),
             Qt::QueuedConnection );
    d->m_downloadTimer->start( timeBetweenDownloads );
}

AbstractDataPluginModel::~AbstractDataPluginModel() {
    delete d;
}

QList<AbstractDataPluginItem*> AbstractDataPluginModel::items( ViewportParams *viewport,
                                                               MarbleDataFacade *facade,
                                                               qint32 number )
{
    GeoDataLatLonAltBox currentBox = viewport->viewLatLonAltBox();
    QString target = facade->target();
    QList<AbstractDataPluginItem*> list;
    
    QList<AbstractDataPluginItem*>::iterator i;
    
    d->m_displayedItems.removeAll( 0 );
    
    // Items that are already shown have the highest priority
    for ( i = d->m_displayedItems.begin();
          i != d->m_displayedItems.end() && list.size() < number;
          ++i )
    {
        // Don't try to access an object that doesn't exist
        if( !*i ) {
            continue;
        }
    
        // Only show items that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // Only show items that are on the current planet
        if( (*i)->target() != target ) {
            continue;
        }
        
        if( !currentBox.contains( (*i)->coordinates() ) ) {
            continue;
        }
        
        // If the item was added initially at a nearer position, they don't have priority,
        // because we zoomed out since then.
        if( (*i)->addedAngularResolution() >= viewport->angularResolution() ) {
            list.append( *i );
        }
    }
        
    d->m_itemSet.removeAll( 0 );
    
    for ( i = d->m_itemSet.begin(); i != d->m_itemSet.end() && list.size() < number; ++i ) {
        // Don't try to access an object that doesn't exist
        if( !*i ) {
            qDebug() << "Warning: Null pointer in m_itemSet";
            continue;
        }
        
        // Only show items that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // Only show items that are on the current planet
        if( (*i)->target() != target ) {
            continue;
        }
        
        // If the item is on the viewport, we want to return it
        if( currentBox.contains( (*i)->coordinates() )
            && !list.contains( *i ) )
        {
            list.append( *i );
            
            // We want to save the angular resolution of the first time the item got added.
            // If it is in the list of displayedItems, it was added before
            if( !d->m_displayedItems.contains( *i ) ) {
                (*i)->setAddedAngularResolution( viewport->angularResolution() );
            }
        }
        // FIXME: We have to do something if the item that is not on the viewport.
    }
    
    d->m_lastDataFacade = facade;
    
    if( (!(currentBox == d->m_lastBox)
          || number != d->m_lastNumber ) )
    {
        d->m_lastBox = currentBox;
        d->m_lastNumber = number;
        d->m_lastDataFacade = facade;
    }
    
    d->m_displayedItems = list;
    return list;
}

QList<AbstractDataPluginItem*> AbstractDataPluginModel::displayedItems() {
    return d->m_displayedItems;
}

QList<AbstractDataPluginItem *> AbstractDataPluginModel::whichItemAt( const QPoint& curpos ) {
    QList<AbstractDataPluginItem *> itemsAt;
    
    foreach( AbstractDataPluginItem* item, d->m_displayedItems ) {
        if( item && item->isItemAt( curpos ) )
            itemsAt.append( item );
    }
    
    return itemsAt;
}

void AbstractDataPluginModel::downloadItemData( const QUrl& url,
                                                const QString& type,
                                                AbstractDataPluginItem *item )
{
    if( !item ) {
        return;
    }
    QString id = generateFilename( item->id(), type );
    
    d->m_downloadManager->addJob( url, id, id );
    d->m_downloadingItems.insert( id, item );
}

void AbstractDataPluginModel::downloadDescriptionFile( const QUrl& url ) {    
    if( !url.isEmpty() ) {  
        QString name( descriptionPrefix );
        name += QString::number( d->m_descriptionFileNumber );
        
        d->m_downloadManager->addJob( url, name, name );
        d->m_descriptionFileNumber++;
    }
}

static bool lessThanByPointer( const AbstractDataPluginItem *item1,
                               const AbstractDataPluginItem *item2 )
{
    if( item1 != 0 && item2 != 0 ) {
        return item1->operator<( item2 );
    }
    else {
        return false;
    }
}

void AbstractDataPluginModel::addItemToList( AbstractDataPluginItem *item ) {
    qDebug() << "New item " << item->id();
    
    if( !item ) {
        return;
    }
    
    // This find the right position in the sorted to insert the new item 
    QList<AbstractDataPluginItem*>::iterator i = qLowerBound( d->m_itemSet.begin(),
                                                                d->m_itemSet.end(),
                                                                item,
                                                                lessThanByPointer );
    // Insert the item on the right position in the list
    d->m_itemSet.insert( i, item );
}

QString AbstractDataPluginModel::name() const {
    return d->m_name;
}

void AbstractDataPluginModel::setName( const QString& name ) {
    d->m_name = name;
}

QString AbstractDataPluginModel::generateFilename( const QString& id, const QString& type ) const {
    QString name;
    name += id;
    name += fileIdSeparator;
    name += type;
    
    return name;
}

QString AbstractDataPluginModel::generateFilepath( const QString& id, const QString& type ) const {
    return MarbleDirs::localPath() + "/cache/" + d->m_name + '/' + generateFilename( id, type );
}
    
bool AbstractDataPluginModel::fileExists( const QString& fileName ) const {
    return d->m_storagePolicy->fileExists( fileName );
}

bool AbstractDataPluginModel::fileExists( const QString& id, const QString& type ) const {
    return fileExists( generateFilename( id, type ) );
}

bool AbstractDataPluginModel::itemExists( const QString& id ) {
    QList<AbstractDataPluginItem*>::iterator listIt;
    
    for( listIt = d->m_itemSet.begin();
         listIt != d->m_itemSet.end();
         ++listIt )
    {
        if( (*listIt)->id() == id ) {
            return true;
        }
    }
    
    QHash<QString,AbstractDataPluginItem*>::iterator hashIt;
    
    for( hashIt = d->m_downloadingItems.begin();
         hashIt != d->m_downloadingItems.end();
         ++hashIt )
    {
        if( (*hashIt)->id() == id ) {
            return true;
        }
    }
    
    return false;
}

void AbstractDataPluginModel::handleChangedViewport() {
    if( !d->m_lastDataFacade ) {
        return;
    }
    
    if( ( !( d->m_downloadedBox == d->m_lastBox )
          || d->m_downloadedNumber != d->m_lastNumber
          || d->m_downloadedTarget != d->m_lastDataFacade->target() )
        && d->m_lastNumber != 0 )
    {
        // Save the box we want to download.
        // We don't want to download too often.
        d->m_downloadedBox = d->m_lastBox;
        d->m_downloadedNumber = d->m_lastNumber;
        d->m_downloadedTarget = d->m_lastDataFacade->target();
        
        getAdditionalItems( d->m_lastBox, d->m_lastDataFacade, d->m_lastNumber );
    }
}

void AbstractDataPluginModel::processFinishedJob( const QString& relativeUrlString,
                                                  const QString& id ) {
    Q_UNUSED( relativeUrlString );
    
    if( id.startsWith( descriptionPrefix ) ) {
        parseFile( d->m_storagePolicy->data( id ) );
    }
    else {
        // The downloaded file contains item data.
        
        // Splitting the id in itemId and fileType
        QStringList fileInformation = id.split( fileIdSeparator );
        
        if( fileInformation.size() < 2) {
            qDebug() << "Strange file information " << id;
            return;
        }
        QString itemId = fileInformation.at( 0 );
        fileInformation.removeAt( 0 );
        QString fileType = fileInformation.join( QString( fileIdSeparator ) );
        
        // Searching for the right item in m_downloadingItems
        QHash<QString, AbstractDataPluginItem *>::iterator i = d->m_downloadingItems.find( id );
        if( i != d->m_downloadingItems.end() ) {
            if( itemId != (*i)->id() ) {
                qDebug() << "Different id";
                return;
            }
            
            (*i)->addDownloadedFile( generateFilepath( itemId, fileType ), 
                                     fileType );
            
            // If the file is ready for displaying, it can be added to the list of
            // initialized items
            if( (*i)->initialized() ) {
                addItemToList( *i );
            }

            d->m_downloadingItems.erase( i );
        }
    }
}

} // namespace Marble

#include "AbstractDataPluginModel.moc"
