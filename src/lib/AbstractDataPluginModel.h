//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTDATAPLUGINMODEL_H
#define MARBLE_ABSTRACTDATAPLUGINMODEL_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QString>

#include "marble_export.h"

class QPoint;
class QUrl;

namespace Marble
{
    
class AbstractDataPluginModelPrivate;
class AbstractDataPluginItem;
class GeoDataLatLonAltBox;
class MarbleModel;
class PluginManager;
class ViewportParams;

/**
 * @short An abstract data model (not based on QAbstractModel) for a AbstractDataPlugin.
 *
 * This class is an abstract model for a AbstractDataPlugin. 
 * It provides the storage and selection of added <b>items</b> and it is also responsible for
 * downloading item data.
 *
 * The functions <b>getAdditionalItems()</b> and <b>parseFile()</b> have to be reimplemented in
 * a subclass.
 **/
class MARBLE_EXPORT AbstractDataPluginModel : public QObject
{
    Q_OBJECT
 
 public:
    explicit AbstractDataPluginModel( const QString& name,
                                      const PluginManager *pluginManager,
                                      QObject *parent = 0 );
    virtual ~AbstractDataPluginModel();
        
    /**
     * @brief Get the items on the viewport
     * Returns the currently downloaded images in the @p viewport.
     * The maximum number of images can be specified with @p number,
     * 0 means no limit.
     * @return The list of item with most important item first.
     */
    QList<AbstractDataPluginItem*> items( const ViewportParams *viewport,
                                          const MarbleModel *model,
                                          qint32 number = 10 );
    
    /**
     * @brief Get all items that contain the given point
     * Returns a list of all items that contain the point @p curpos
     */
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos );

    /**
     * @brief Sets the settings for all items.
     * Sets the settings for all items before painting. This ensures that all items react on
     * changed settings.
     */
    void setItemSettings( QHash<QString,QVariant> itemSettings );

    virtual void setFavoriteItems( const QStringList& list );
    QStringList favoriteItems() const;

    virtual void setFavoriteItemsOnly( bool favoriteOnly );
    bool isFavoriteItemsOnly() const;

 protected:
    /**
     * Managing to get @p number additional items in @p box. This includes generating a url and
     * downloading the corresponding file.
     * This method has to be implemented in a subclass.
     **/
    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     const MarbleModel *model,
                                     qint32 number = 10 ) = 0;
       
    /**
     * Parse the @p file and generate items. The items will be added to the list or the method
     * starts additionally needed downloads.
     * This method has to be implemented in a subclass.
     **/
    virtual void parseFile( const QByteArray& file );
        
    /**
     * Downloads the file from @p url. @p item -> addDownloadedFile() will be called when the
     * download is finished. Additionally initialized() items will be added to the item list
     * after the download. It checks if a item with the same id is already in the list and
     * ignores and deletes the item in this case.
     * @param: The type of the download (to be specified by the subclasser)
     **/
    void downloadItemData( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    
    /**
     * Download the description file from the @p url.
     */
    void downloadDescriptionFile( const QUrl& url );
    
    /**
     * Adds the @p item to the list of initialized items. It checks if a item with the same id is
     * already in the list and ignores and deletes the item in this case.
     */
    void addItemToList( AbstractDataPluginItem *item );
    
    /**
     * Generates the filename relative to the download path from @p id and @p type
     */
    QString generateFilename( const QString& id, const QString& type ) const;
    
    /**
     * Generates the absolute filepath of the from @p id and @p type
     */
    QString generateFilepath( const QString& id, const QString& type ) const;
    
    /**
     * Testing the existence of the file @p fileName
     */
    bool fileExists( const QString& fileName ) const;
    
    /**
     * Testing the existence of a file with @p id and @p type
     */
    bool fileExists( const QString& id, const QString& type ) const;
    
    /**
     * Finds the item with @p id in the list.
     * @return The pointer to the item or (if no item has been found) 0
     */
    AbstractDataPluginItem *findItem( const QString& id ) const;
    
    /**
     * Testing the existence of the item @p id in the list
     */
    bool itemExists( const QString& id ) const;

    /**
     * Removes all items
     */
    void clear();
    
 private Q_SLOTS:
    /**
     * @brief Get new items with getAdditionalItems if it is reasonable.
     */
    void handleChangedViewport();
    
    /**
     * @brief This method will assign downloaded files to the corresponding items
     * @param relativeUrlString The string containing the relative (to the downloader path)
     *                          url of the downloaded file.
     * @param id The id of the downloaded file
     */
    void processFinishedJob( const QString& relativeUrlString, const QString& id );
    
    /**
     * @brief Removes the item from the list.
     */
    void removeItem( QObject *item );

    void favoriteItemChanged( const QString& id, bool isFavorite );

 Q_SIGNALS:
    void itemsUpdated();
    void favoriteItemsChanged( const QStringList& favoriteItems );
    
 private:
    AbstractDataPluginModelPrivate * const d;
};

}

#endif
