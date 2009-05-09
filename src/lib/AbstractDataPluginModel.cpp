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
#include "AbstractDataPluginWidget.h"
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

// Separator to separate the id of the widget from the file type
const char fileIdSeparator = '_';
    
class AbstractDataPluginModelPrivate {
 public:
    AbstractDataPluginModelPrivate( QString name, AbstractDataPluginModel * parent )
        : m_parent( parent ),
          m_name( name ),
          m_lastBox( new GeoDataLatLonAltBox() ),
          m_downloadedBox( new GeoDataLatLonAltBox() ),
          m_lastNumber( 0 ),
          m_downloadedNumber( 0 ),
          m_lastDataFacade( 0 ),
          m_downloadTimer( new QTimer( m_parent ) )
    {
    }
    
    ~AbstractDataPluginModelPrivate() {
        m_widgetSet.removeAll( 0 );
        QHash<QString, AbstractDataPluginWidget*>::iterator it = m_downloadingWidgets.begin();
        while( it != m_downloadingWidgets.end() ) {
            if( !(*it) ) {
                it = m_downloadingWidgets.erase( it );
            }
            else {
                ++it;
            }
        }
        qDeleteAll( m_widgetSet );
        qDeleteAll( m_downloadingWidgets );
        delete m_storagePolicy;
        delete m_lastBox;
        delete m_downloadedBox;
    }
    
    AbstractDataPluginModel *m_parent;
    QString m_name;
    GeoDataLatLonAltBox *m_lastBox;
    GeoDataLatLonAltBox *m_downloadedBox;
    qint32 m_lastNumber;
    qint32 m_downloadedNumber;
    MarbleDataFacade *m_lastDataFacade;
    QString m_downloadedTarget;
    QList<AbstractDataPluginWidget*> m_widgetSet;
    QHash<QString, AbstractDataPluginWidget*> m_downloadingWidgets;
    QList<AbstractDataPluginWidget*> m_displayedWidgets;
    QTimer *m_downloadTimer;
    
    CacheStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
};

AbstractDataPluginModel::AbstractDataPluginModel( QString name, QObject *parent )
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
             this,               SLOT( downloadDescriptionFile() ),
             Qt::QueuedConnection );
    d->m_downloadTimer->start( timeBetweenDownloads );
}

AbstractDataPluginModel::~AbstractDataPluginModel() {
    delete d;
}

QList<AbstractDataPluginWidget*> AbstractDataPluginModel::widgets( ViewportParams *viewport,
                                                                   MarbleDataFacade *facade,
                                                                   qint32 number )
{
    GeoDataLatLonAltBox *currentBox = new GeoDataLatLonAltBox( viewport->viewLatLonAltBox() );
    QString target = facade->target();
    QList<AbstractDataPluginWidget*> list;
    
    QList<AbstractDataPluginWidget*>::iterator i;
    
    d->m_displayedWidgets.removeAll( 0 );
    
    // Widgets that are already shown have the highest priority
    for ( i = d->m_displayedWidgets.begin();
          i != d->m_displayedWidgets.end() && list.size() < number;
          ++i )
    {
        // Don't try to access an object that doesn't exist
        if( !*i ) {
            continue;
        }
    
        // Only show widgets that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // Only show widgets that are on the current planet
        if( (*i)->target() != target ) {
            continue;
        }
        
        if( !currentBox->contains( (*i)->coordinates() ) ) {
            continue;
        }
        
        // If the widget was added initially at a nearer position, they don't have priority,
        // because we zoomed out since then.
        if( (*i)->addedAngularResolution() >= viewport->angularResolution() ) {
            list.append( *i );
        }
    }
        
    d->m_widgetSet.removeAll( 0 );
    
    for ( i = d->m_widgetSet.begin(); i != d->m_widgetSet.end() && list.size() < number; ++i ) {
        // Don't try to access an object that doesn't exist
        if( !*i ) {
            qDebug() << "Warning: Null pointer in m_widgetSet";
            continue;
        }
        
        // Only show widgets that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // Only show widgets that are on the current planet
        if( (*i)->target() != target ) {
            continue;
        }
        
        // If the widget is on the viewport, we want to return it
        if( currentBox->contains( (*i)->coordinates() )
            && !list.contains( *i ) )
        {
            list.append( *i );
            
            // We want to save the angular resolution of the first time the widget got added.
            // If it is in the list of displayedWidgets, it was added before
            if( !d->m_displayedWidgets.contains( *i ) ) {
                (*i)->setAddedAngularResolution( viewport->angularResolution() );
            }
        }
        // FIXME: We have to do something if the widget that is not on the viewport.
    }
    
    d->m_lastDataFacade = facade;
    
    if( (!((*currentBox) == (*d->m_lastBox))
          || number != d->m_lastNumber ) )
    {
        delete d->m_lastBox;
        d->m_lastBox = currentBox;
        d->m_lastNumber = number;
        d->m_lastDataFacade = facade;
    }
    
    d->m_displayedWidgets = list;
    return list;
}

QList<AbstractDataPluginWidget*> AbstractDataPluginModel::displayedWidgets() {
    return d->m_displayedWidgets;
}

QList<AbstractDataPluginWidget *> AbstractDataPluginModel::whichWidgetAt( const QPoint& curpos ) {
    QList<AbstractDataPluginWidget *> widgetsAt;
    
    foreach( AbstractDataPluginWidget* widget, d->m_displayedWidgets ) {
        if( widget && widget->isWidgetAt( curpos ) )
            widgetsAt.append( widget );
    }
    
    return widgetsAt;
}

void AbstractDataPluginModel::downloadWidgetData( QUrl url,
                                                  QString type,
                                                  AbstractDataPluginWidget *widget )
{
    if( !widget ) {
        return;
    }
    QString id = generateFilename( widget->id(), type );
    
    d->m_downloadManager->addJob( url, id, id );
    d->m_downloadingWidgets.insert( id, widget );
}

static bool lessThanByPointer( const AbstractDataPluginWidget *widget1,
                               const AbstractDataPluginWidget *widget2 )
{
    if( widget1 != 0 && widget2 != 0 ) {
        return widget1->operator<( widget2 );
    }
    else {
        return false;
    }
}

void AbstractDataPluginModel::addWidgetToList( AbstractDataPluginWidget *widget ) {
    qDebug() << "New widget " << widget->id();
    
    if( !widget ) {
        return;
    }
    
    // This find the right position in the sorted to insert the new widget 
    QList<AbstractDataPluginWidget*>::iterator i = qLowerBound( d->m_widgetSet.begin(),
                                                                d->m_widgetSet.end(),
                                                                widget,
                                                                lessThanByPointer );
    // Insert the widget on the right position in the list
    d->m_widgetSet.insert( i, widget );
}

QString AbstractDataPluginModel::name() const {
    return d->m_name;
}

void AbstractDataPluginModel::setName( QString name ) {
    d->m_name = name;
}

QString AbstractDataPluginModel::generateFilename( QString id, QString type ) const {
    QString name;
    name += id;
    name += fileIdSeparator;
    name += type;
    
    return name;
}

QString AbstractDataPluginModel::generateFilepath( QString id, QString type ) const {
    return MarbleDirs::localPath() + "/cache/" + d->m_name + '/' + generateFilename( id, type );
}
    
bool AbstractDataPluginModel::fileExists( QString fileName ) {
    return d->m_storagePolicy->fileExists( fileName );
}

bool AbstractDataPluginModel::fileExists( QString id, QString type ) {
    return fileExists( generateFilename( id, type ) );
}

bool AbstractDataPluginModel::widgetExists( QString id ) {
    QList<AbstractDataPluginWidget*>::iterator listIt;
    
    for( listIt = d->m_widgetSet.begin();
         listIt != d->m_widgetSet.end();
         ++listIt )
    {
        if( (*listIt)->id() == id ) {
            return true;
        }
    }
    
    QHash<QString,AbstractDataPluginWidget*>::iterator hashIt;
    
    for( hashIt = d->m_downloadingWidgets.begin();
         hashIt != d->m_downloadingWidgets.end();
         ++hashIt )
    {
        if( (*hashIt)->id() == id ) {
            return true;
        }
    }
    
    return false;
}

void AbstractDataPluginModel::downloadDescriptionFile() {
    if( !d->m_lastDataFacade ) {
        return;
    }
    
    if( ( !( *d->m_downloadedBox == *d->m_lastBox )
          || d->m_downloadedNumber != d->m_lastNumber
          || d->m_downloadedTarget != d->m_lastDataFacade->target() )
        && d->m_lastNumber != 0 )
    {
        // Save the box we want to download.
        // We don't want to download too often.
        delete d->m_downloadedBox;
        d->m_downloadedBox = new GeoDataLatLonAltBox( *d->m_lastBox );
        d->m_downloadedNumber = d->m_lastNumber;
        d->m_downloadedTarget = d->m_lastDataFacade->target();
        
        QString name( descriptionPrefix );
        name += QString::number( d->m_lastBox->east() );
        name += QString::number( d->m_lastBox->west() );
        name += QString::number( d->m_lastBox->north() );
        name += QString::number( d->m_lastBox->south() );
        
        QUrl url = descriptionFileUrl( d->m_lastBox, d->m_lastDataFacade, d->m_lastNumber );
        
        if( !url.isEmpty() ) {
            d->m_downloadManager->addJob( url, name, name );
        }
    }
}

void AbstractDataPluginModel::processFinishedJob( QString relativeUrlString, QString id ) {
    Q_UNUSED( relativeUrlString );
    
    if( id.startsWith( descriptionPrefix ) ) {
        parseFile( d->m_storagePolicy->data( id ) );
    }
    else {
        // The downloaded file contains widget data.
        
        // Splitting the id in widgetId and fileType
        QStringList fileInformation = id.split( fileIdSeparator );
        
        if( fileInformation.size() < 2) {
            qDebug() << "Strange file information " << id;
            return;
        }
        QString widgetId = fileInformation.at( 0 );
        fileInformation.removeAt( 0 );
        QString fileType = fileInformation.join( QString( fileIdSeparator ) );
        
        // Searching for the right widget in m_downloadingWidgets
        QHash<QString, AbstractDataPluginWidget *>::iterator i = d->m_downloadingWidgets.find( id );
        if( i != d->m_downloadingWidgets.end() ) {
            if( widgetId != (*i)->id() ) {
                qDebug() << "Different id";
                return;
            }
            
            (*i)->addDownloadedFile( generateFilepath( widgetId, fileType ), 
                                     fileType );
            
            // If the file is ready for displaying, it can be added to the list of
            // initialized widgets
            if( (*i)->initialized() ) {
                addWidgetToList( *i );
            }

            d->m_downloadingWidgets.erase( i );
        }
    }
}

} // namespace Marble

#include "AbstractDataPluginModel.moc"
