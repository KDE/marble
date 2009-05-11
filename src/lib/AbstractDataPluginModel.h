//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTDATAPLUGINMODEL_H
#define ABSTRACTDATAPLUGINMODEL_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QString>

#include "marble_export.h"

class QPoint;
class QTimer;
class QUrl;

namespace Marble {
    
class AbstractDataPluginModelPrivate;
class AbstractDataPluginItem;
class CacheStoragePolicy;
class HttpDownloadManager;
class GeoDataLatLonAltBox;
class MarbleDataFacade;
class ViewportParams;

class MARBLE_EXPORT AbstractDataPluginModel : public QObject
{
    Q_OBJECT
 
 public:
    explicit AbstractDataPluginModel( QString name, QObject *parent = 0 );
    ~AbstractDataPluginModel();
        
    /**
     * Returns the currently downloaded images in the @p viewport.
     * The maximum number of images can be specified with @p number,
     * 0 means no limit.
     * @return The list of item with most important item first.
     */
    QList<AbstractDataPluginItem*> items( ViewportParams *viewport,
                                          MarbleDataFacade *facade,
                                          qint32 number = 10 );
    
    /**
     * Returns all items which were returned by items() again.
     */
    QList<AbstractDataPluginItem*> displayedItems();
    
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos );
       
 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    virtual QUrl descriptionFileUrl( GeoDataLatLonAltBox *box,
                                     MarbleDataFacade *facade,
                                     qint32 number = 10 ) = 0;
       
    /**
     * The reimplementation has to parse the @p file and should generate items. This items
     * have to be scheduled to downloadItemData or could be directly added to the list,
     * depending on if they have to download information to be shown.
     **/
    virtual void parseFile( QByteArray file ) = 0;
        
    /**
     * Downloads the file from @p url and calls @p item -> addDownloadedFile() when the
     * download is finished.
     **/
    void downloadItemData( QUrl url, QString type, AbstractDataPluginItem *item );
    
    /**
     * Adds the @p item to the list of already initialized items.
     */
    void addItemToList( AbstractDataPluginItem *item );
    
    /**
     * Returns the name of the plugin
     */
    QString name() const;
    
    /**
     * Change the name of the plugin
     */
    void setName( QString name );
    
    /**
     * Generates the filename relatively to the download path from @p id and @p type
     */
    QString generateFilename( QString id, QString type ) const;
    
    /**
     * Generates the absolute filepath of the from @p id and @p type
     */
    QString generateFilepath( QString id, QString type ) const;
    
    /**
     * Testing the existence of the file @p fileName
     */
    bool fileExists( QString fileName );
    
    /**
     * Testing the existence of a file with @p id and @p type
     */
    bool fileExists( QString id, QString type );
    
    /**
     * Testing the existence of the item @p id in the list
     */
    bool itemExists( QString id );
    
 private Q_SLOTS:
    /**
     * Download a new description file
     */
    void downloadDescriptionFile();
    
    /**
     * Processing a finished download job
     */
    void processFinishedJob( QString relativeUrlString, QString id );
    
 private:
    AbstractDataPluginModelPrivate * const d;
};

}

#endif // ABSTRACTDATAPLUGINMODEL_H
