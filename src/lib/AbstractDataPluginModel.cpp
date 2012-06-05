//
// This file is part of the Marble Virtual Globe.
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
#include <QtCore/QTimer>
#include <QtCore/QPointF>
#include <QtCore/QtAlgorithms>
#include <QtCore/QVariant>
#include <QtCore/QAbstractListModel>
#include <QtCore/QMetaProperty>

// Marble
#include "MarbleDebug.h"
#include "AbstractDataPluginItem.h"
#include "CacheStoragePolicy.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "HttpDownloadManager.h"
#include "MarbleModel.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

#include <cmath>

namespace Marble
{

const QString descriptionPrefix( "description_" );

// Time between two tried description file downloads (we decided not to download anything) in ms
const int timeBetweenTriedDownloads = 500;
// Time between two real description file downloads in ms
const int timeBetweenDownloads = 1500;

// The factor describing how much the box has to be changed to download a new description file.
// A higher factor means more downloads.
const qreal boxComparisonFactor = 16.0;

// Separator to separate the id of the item from the file type
const char fileIdSeparator = '_';

class FavoritesModel;

class AbstractDataPluginModelPrivate
{
public:
    AbstractDataPluginModelPrivate( const QString& name,
                                    const PluginManager *pluginManager,
                                    AbstractDataPluginModel * parent );
    
    ~AbstractDataPluginModelPrivate();

    void updateFavoriteItems();
    
    AbstractDataPluginModel *m_parent;
    const QString m_name;
    GeoDataLatLonAltBox m_lastBox;
    GeoDataLatLonAltBox m_downloadedBox;
    qint32 m_lastNumber;
    qint32 m_downloadedNumber;
    const MarbleModel *m_lastMarbleModel;
    QString m_downloadedTarget;
    QList<AbstractDataPluginItem*> m_itemSet;
    QHash<QString, AbstractDataPluginItem*> m_downloadingItems;
    QList<AbstractDataPluginItem*> m_displayedItems;
    QTimer m_downloadTimer;
    quint32 m_descriptionFileNumber;
    QHash<QString, QVariant> m_itemSettings;
    QStringList m_favoriteItems;
    bool m_favoriteItemsOnly;

    CacheStoragePolicy m_storagePolicy;
    HttpDownloadManager m_downloadManager;
    FavoritesModel* m_favoritesModel;
    QMetaObject m_metaObject;
};

class FavoritesModel : public QAbstractListModel
{
public:
    AbstractDataPluginModelPrivate* d;

    explicit FavoritesModel( AbstractDataPluginModelPrivate* d, QObject* parent = 0 );

    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    void reset();
};

AbstractDataPluginModelPrivate::AbstractDataPluginModelPrivate( const QString& name,
                                const PluginManager *pluginManager,
                                AbstractDataPluginModel * parent )
    : m_parent( parent ),
      m_name( name ),
      m_lastBox(),
      m_downloadedBox(),
      m_lastNumber( 0 ),
      m_downloadedNumber( 0 ),
      m_lastMarbleModel( 0 ),
      m_downloadTimer( m_parent ),
      m_descriptionFileNumber( 0 ),
      m_itemSettings(),
      m_favoriteItemsOnly( false ),
      m_storagePolicy( MarbleDirs::localPath() + "/cache/" + m_name + '/' ),
      m_downloadManager( &m_storagePolicy, pluginManager ),
      m_favoritesModel( 0 )
{
}

AbstractDataPluginModelPrivate::~AbstractDataPluginModelPrivate() {
    QList<AbstractDataPluginItem*>::iterator lIt = m_itemSet.begin();
    QList<AbstractDataPluginItem*>::iterator const lItEnd = m_itemSet.end();
    for (; lIt != lItEnd; ++lIt ) {
        (*lIt)->deleteLater();
    }

    QHash<QString,AbstractDataPluginItem*>::iterator hIt = m_downloadingItems.begin();
    QHash<QString,AbstractDataPluginItem*>::iterator const hItEnd = m_downloadingItems.end();
    for (; hIt != hItEnd; ++hIt ) {
        (*hIt)->deleteLater();
    }

    m_storagePolicy.clearCache();
}

void AbstractDataPluginModelPrivate::updateFavoriteItems()
{
    if ( m_lastMarbleModel && m_favoriteItemsOnly ) {
        foreach( const QString &id, m_favoriteItems ) {
            if ( !m_parent->findItem( id ) ) {
                m_parent->getItem( id, m_lastMarbleModel );
            }
        }
    }
}

FavoritesModel::FavoritesModel( AbstractDataPluginModelPrivate *_d, QObject* parent ) :
    QAbstractListModel( parent ), d(_d)
{
    QHash<int,QByteArray> roles = roleNames();
    int const size = d->m_metaObject.propertyCount();
    for ( int i=0; i<size; ++i ) {
        QMetaProperty property = d->m_metaObject.property( i );
        roles[Qt::UserRole+i] = property.name();
    }
    roles[Qt::DisplayRole] = "display";
    roles[Qt::DecorationRole] = "decoration";
    setRoleNames( roles );
}

int FavoritesModel::rowCount ( const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return 0;
    }

    int count = 0;
    foreach( AbstractDataPluginItem* item, d->m_itemSet ) {
        if ( item->initialized() && item->isFavorite() ) {
            ++count;
        }
    }

    return count;
}

QVariant FavoritesModel::data( const QModelIndex &index, int role ) const
{
    int const row = index.row();
    if ( row >= 0 && row < rowCount() ) {
        int count = 0;
        foreach( AbstractDataPluginItem* item, d->m_itemSet ) {
            if ( item->initialized() && item->isFavorite() ) {
                if ( count == row ) {
                    QString const roleName = roleNames().value( role, int( Qt::DisplayRole ) );
                    return item->property( roleName.toAscii() );
                }
                ++count;
            }
        }
    }

    return QVariant();
}

void FavoritesModel::reset()
{
    QAbstractListModel::reset();
}

AbstractDataPluginModel::AbstractDataPluginModel( const QString& name,
                                                  const PluginManager *pluginManager,
                                                  QObject *parent )
    : QObject(  parent ),
      d( new AbstractDataPluginModelPrivate( name, pluginManager, this ) )
{
    // Initializing file and download System
    connect( &d->m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
             this ,                 SLOT( processFinishedJob( QString , QString ) ) );
    
    // We want to download a new description file every timeBetweenDownloads ms
    connect( &d->m_downloadTimer, SIGNAL( timeout() ),
             this,               SLOT( handleChangedViewport() ),
             Qt::QueuedConnection );
    d->m_downloadTimer.start( timeBetweenDownloads );
}

AbstractDataPluginModel::~AbstractDataPluginModel()
{
    delete d;
}

QList<AbstractDataPluginItem*> AbstractDataPluginModel::items( const ViewportParams *viewport,
                                                               const MarbleModel *model,
                                                               qint32 number )
{
    GeoDataLatLonAltBox currentBox = viewport->viewLatLonAltBox();
    QString target = model->planetId();
    QList<AbstractDataPluginItem*> list;
    
    int const removed = d->m_displayedItems.removeAll( 0 ) + d->m_itemSet.removeAll( 0 );
    // I don't see why the removeAll( 0 ) calls above should be needed. The assert below will trigger
    // if I'm wrong.
    Q_ASSERT( !removed && "Null item in item list. Please report a bug to marble-devel@kde.org" );

    QList<AbstractDataPluginItem*> const candidates = d->m_displayedItems + d->m_itemSet;
    QList<AbstractDataPluginItem*>::const_iterator i = candidates.begin();
    QList<AbstractDataPluginItem*>::const_iterator end = candidates.end();

    // Items that are already shown have the highest priority
    for (; i != end && list.size() < number; ++i ) {
        Q_ASSERT( *i ); // checked when adding items, also removeAll(0) above

        // Only show items that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // Only show items that are on the current planet
        if( (*i)->target() != target ) {
            continue;
        }

        // Hide non-favorite items if necessary
        if( d->m_favoriteItemsOnly && !(*i)->isFavorite() ) {
            continue;
        }
        
        if( !currentBox.contains( (*i)->coordinate() ) ) {
            continue;
        }
        
        // If the item was added initially at a nearer position, they don't have priority,
        // because we zoomed out since then.
        bool const alreadyDisplayed = d->m_displayedItems.contains( *i );
        if( !list.contains( *i ) && ( !alreadyDisplayed || (*i)->addedAngularResolution() >= viewport->angularResolution() ) ) {
            bool collides = false;
            int const length = list.length();
            for ( int j=0; !collides && j<length; ++j ) {
                foreach( const QRectF &rect, list[j]->boundingRects() ) {
                    foreach( const QRectF &itemRect, (*i)->boundingRects() ) {
                        if ( rect.intersects( itemRect ) )
                            collides = true;
                    }
                }
            }

            if ( !collides ) {
                list.append( *i );
                (*i)->setSettings( d->m_itemSettings );

                // We want to save the angular resolution of the first time the item got added.
                if( !alreadyDisplayed ) {
                    (*i)->setAddedAngularResolution( viewport->angularResolution() );
                }
            }
        }
        // TODO: Do we have to cleanup at some point? The list of all items keeps growing
    }

    d->m_lastBox = currentBox;
    d->m_lastNumber = number;
    if ( d->m_lastMarbleModel != model ) {
        d->m_lastMarbleModel = model;
        d->updateFavoriteItems();
    }
    d->m_displayedItems = list;
    return list;
}

QList<AbstractDataPluginItem *> AbstractDataPluginModel::whichItemAt( const QPoint& curpos )
{
    QList<AbstractDataPluginItem *> itemsAt;
    
    foreach( AbstractDataPluginItem* item, d->m_displayedItems ) {
        if( item && item->contains( QPointF( curpos ) ) )
            itemsAt.append( item );
    }
    
    return itemsAt;
}

void AbstractDataPluginModel::parseFile( const QByteArray& file )
{
    Q_UNUSED( file );
}

void AbstractDataPluginModel::downloadItemData( const QUrl& url,
                                                const QString& type,
                                                AbstractDataPluginItem *item )
{
    if( !item ) {
        return;
    }

    QString id = generateFilename( item->id(), type );
    
    d->m_downloadManager.addJob( url, id, id, DownloadBrowse );
    d->m_downloadingItems.insert( id, item );
    
    connect( item, SIGNAL( destroyed( QObject* ) ), this, SLOT( removeItem( QObject* ) ) );

    addItemToList( item );
}

void AbstractDataPluginModel::downloadDescriptionFile( const QUrl& url )
{
    if( !url.isEmpty() ) {
        QString name( descriptionPrefix );
        name += QString::number( d->m_descriptionFileNumber );
        
        d->m_downloadManager.addJob( url, name, name, DownloadBrowse );
        d->m_descriptionFileNumber++;
    }
}

static bool lessThanByPointer( const AbstractDataPluginItem *item1,
                               const AbstractDataPluginItem *item2 )
{
    if( item1 && item2 ) {
        // Compare by favorite status (favorites first), then by operator<
        bool const favorite1 = item1->isFavorite();
        if ( favorite1 ^ item2->isFavorite() ) {
            return favorite1 ? item1 : item2;
        } else {
            return item1->operator<( item2 );
        }
    }
    else {
        return false;
    }
}

void AbstractDataPluginModel::addItemToList( AbstractDataPluginItem *item )
{
    addItemsToList( QList<AbstractDataPluginItem*>() << item );
}

void AbstractDataPluginModel::addItemsToList( const QList<AbstractDataPluginItem *> &items )
{
    bool needsUpdate = false;
    bool favoriteChanged = false;
    foreach( AbstractDataPluginItem *item, items ) {
        if( !item ) {
            continue;
        }

        // If the item is already in our list, don't add it.
        if( AbstractDataPluginItem *oldItem = findItem( item->id() ) ) {
            if ( oldItem == item ) {
                continue;
            }
            else {
                item->deleteLater();
                continue;
            }
        }

        mDebug() << "New item " << item->id();

        // This find the right position in the sorted to insert the new item
        QList<AbstractDataPluginItem*>::iterator i = qLowerBound( d->m_itemSet.begin(),
                                                                  d->m_itemSet.end(),
                                                                  item,
                                                                  lessThanByPointer );
        // Insert the item on the right position in the list
        d->m_itemSet.insert( i, item );

        connect( item, SIGNAL( destroyed( QObject* ) ), this, SLOT( removeItem( QObject* ) ) );
        connect( item, SIGNAL( updated() ), this, SIGNAL( itemsUpdated() ) );
        connect( item, SIGNAL( favoriteChanged( const QString&, bool ) ), this,
                 SLOT( favoriteItemChanged( const QString&, bool ) ) );

        if ( !needsUpdate && item->initialized() ) {
            needsUpdate = true;
        }

        if ( !favoriteChanged && item->initialized() && item->isFavorite() ) {
            favoriteChanged = true;
        }
    }

    if ( favoriteChanged && d->m_favoritesModel ) {
        d->m_favoritesModel->reset();
    }

    if ( needsUpdate ) {
        emit itemsUpdated();
    }
}

void AbstractDataPluginModel::getItem( const QString &, const MarbleModel * )
{
    qWarning() << "Retrieving items by identifier is not implemented by this plugin";
}

void AbstractDataPluginModel::setFavoriteItems( const QStringList& list )
{
    if ( d->m_favoriteItems != list) {
        d->m_favoriteItems = list;
        d->updateFavoriteItems();
        if ( d->m_favoritesModel ) {
            d->m_favoritesModel->reset();
        }
        emit favoriteItemsChanged( d->m_favoriteItems );
    }
}

QStringList AbstractDataPluginModel::favoriteItems() const
{
    return d->m_favoriteItems;
}

void AbstractDataPluginModel::setFavoriteItemsOnly( bool favoriteOnly )
{
    if ( isFavoriteItemsOnly() != favoriteOnly ) {
        d->m_favoriteItemsOnly = favoriteOnly;
        d->updateFavoriteItems();
        emit favoriteItemsOnlyChanged();
    }
}

bool AbstractDataPluginModel::isFavoriteItemsOnly() const
{
    return d->m_favoriteItemsOnly;
}

QObject *AbstractDataPluginModel::favoritesModel()
{
    if ( !d->m_favoritesModel ) {
        d->m_favoritesModel = new FavoritesModel( d, this );
        d->updateFavoriteItems();
    }

    return d->m_favoritesModel;
}

void AbstractDataPluginModel::favoriteItemChanged( const QString& id, bool isFavorite )
{
    QStringList favorites = d->m_favoriteItems;

    if ( isFavorite ) {
        if ( !favorites.contains(id) )
            favorites.append( id );
    } else {
        favorites.removeOne( id );
    }

    setFavoriteItems( favorites );
}

QString AbstractDataPluginModel::generateFilename( const QString& id, const QString& type ) const
{
    QString name;
    name += id;
    name += fileIdSeparator;
    name += type;
    
    return name;
}

QString AbstractDataPluginModel::generateFilepath( const QString& id, const QString& type ) const
{
    return MarbleDirs::localPath() + "/cache/" + d->m_name + '/' + generateFilename( id, type );
}

bool AbstractDataPluginModel::fileExists( const QString& fileName ) const
{
    return d->m_storagePolicy.fileExists( fileName );
}

bool AbstractDataPluginModel::fileExists( const QString& id, const QString& type ) const
{
    return fileExists( generateFilename( id, type ) );
}

AbstractDataPluginItem *AbstractDataPluginModel::findItem( const QString& id ) const
{
    QList<AbstractDataPluginItem*>::iterator listIt = d->m_itemSet.begin();
    QList<AbstractDataPluginItem*>::iterator const end = d->m_itemSet.end();
    
    for (; listIt != end; ++listIt ) {
        if( (*listIt)->id() == id ) {
            return (*listIt);
        }
    }
    
    return 0;
}

bool AbstractDataPluginModel::itemExists( const QString& id ) const
{
    return findItem( id );
}

void AbstractDataPluginModel::setItemSettings( QHash<QString,QVariant> itemSettings )
{
    d->m_itemSettings = itemSettings;
}

void AbstractDataPluginModel::handleChangedViewport()
{
    if( !d->m_lastMarbleModel || d->m_favoriteItemsOnly ) {
        return;
    }
    
    // All this is to prevent to often downloads
    if( d->m_lastNumber != 0
            // We don't need to download if nothing changed
            && ( !( d->m_downloadedBox == d->m_lastBox )
                 || d->m_downloadedNumber != d->m_lastNumber
                 || d->m_downloadedTarget != d->m_lastMarbleModel->planetId() )
            // We try to filter little changes of the bounding box
            && ( fabs( d->m_downloadedBox.east() - d->m_lastBox.east() ) * boxComparisonFactor
                 > d->m_lastBox.width()
                 || fabs( d->m_downloadedBox.south() - d->m_lastBox.south() ) * boxComparisonFactor
                 > d->m_lastBox.height()
                 || fabs( d->m_downloadedBox.north() - d->m_lastBox.north() ) * boxComparisonFactor
                 > d->m_lastBox.height()
                 || fabs( d->m_downloadedBox.west() - d->m_lastBox.west() ) * boxComparisonFactor
                 > d->m_lastBox.width() ) )
    {
        // We will wait a little bit longer to start the the
        // next download as we will really download something now.
        d->m_downloadTimer.setInterval( timeBetweenDownloads );
        
        // Save the download parameter
        d->m_downloadedBox = d->m_lastBox;
        d->m_downloadedNumber = d->m_lastNumber;
        d->m_downloadedTarget = d->m_lastMarbleModel->planetId();
        
        // Get items
        getAdditionalItems( d->m_lastBox, d->m_lastMarbleModel, d->m_lastNumber );
    }
    else {
        // Don't wait to long to start the next download as we decided not to download anything.
        // This will enhance response.
        d->m_downloadTimer.setInterval( timeBetweenTriedDownloads );
    }
}

void AbstractDataPluginModel::processFinishedJob( const QString& relativeUrlString,
                                                  const QString& id )
{
    Q_UNUSED( relativeUrlString );
    
    if( id.startsWith( descriptionPrefix ) ) {
        parseFile( d->m_storagePolicy.data( id ) );
    }
    else {
        // The downloaded file contains item data.
        
        // Splitting the id in itemId and fileType
        QStringList fileInformation = id.split( fileIdSeparator );
        
        if( fileInformation.size() < 2) {
            mDebug() << "Strange file information " << id;
            return;
        }
        QString itemId = fileInformation.at( 0 );
        fileInformation.removeAt( 0 );
        QString fileType = fileInformation.join( QString( fileIdSeparator ) );
        
        // Searching for the right item in m_downloadingItems
        QHash<QString, AbstractDataPluginItem *>::iterator i = d->m_downloadingItems.find( id );
        if( i != d->m_downloadingItems.end() ) {
            if( itemId != (*i)->id() ) {
                return;
            }
            
            (*i)->addDownloadedFile( generateFilepath( itemId, fileType ),
                                     fileType );

            d->m_downloadingItems.erase( i );
        }
    }
}

void AbstractDataPluginModel::removeItem( QObject *item )
{
    d->m_itemSet.removeAll( (AbstractDataPluginItem *) item );
    QHash<QString, AbstractDataPluginItem *>::iterator i;
    for( i = d->m_downloadingItems.begin(); i != d->m_downloadingItems.end(); ++i ) {
        if( (*i) == (AbstractDataPluginItem *) item ) {
            i = d->m_downloadingItems.erase( i );
        }
    }
}

void AbstractDataPluginModel::clear()
{
    d->m_displayedItems.clear();
    QList<AbstractDataPluginItem*>::iterator iter = d->m_itemSet.begin();
    QList<AbstractDataPluginItem*>::iterator const end = d->m_itemSet.end();
    for (; iter != end; ++iter ) {
        (*iter)->deleteLater();
    }
    d->m_itemSet.clear();
    emit itemsUpdated();
}

void AbstractDataPluginModel::registerItemProperties( const QMetaObject &item )
{
    d->m_metaObject = item;
}

} // namespace Marble

#include "AbstractDataPluginModel.moc"
