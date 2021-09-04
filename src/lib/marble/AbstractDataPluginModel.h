// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTDATAPLUGINMODEL_H
#define MARBLE_ABSTRACTDATAPLUGINMODEL_H

#include <QObject>
#include <QList>
#include <QHash>

#include "marble_export.h"

class QPoint;
class QUrl;
class QString;
class QStringList;

namespace Marble
{
    
class AbstractDataPluginModelPrivate;
class AbstractDataPluginItem;
class GeoDataLatLonAltBox;
class MarbleModel;
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

    /** @todo FIXME Qt Quick segfaults if using the real class here instead of QObject */
    Q_PROPERTY( QObject* favoritesModel READ favoritesModel CONSTANT )
 
 public:
    explicit AbstractDataPluginModel( const QString& name, const MarbleModel *marbleModel, QObject *parent = nullptr );
    ~AbstractDataPluginModel() override;
        
    const MarbleModel *marbleModel() const;

    /**
     * @brief Get the items on the viewport
     * Returns the currently downloaded images in the @p viewport.
     * The maximum number of images can be specified with @p number,
     * 0 means no limit.
     * @return The list of item with most important item first.
     */
    QList<AbstractDataPluginItem*> items( const ViewportParams *viewport,
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
    void setItemSettings(const QHash<QString, QVariant> &itemSettings);

    virtual void setFavoriteItems( const QStringList& list );
    QStringList favoriteItems() const;

    void setFavoriteItemsOnly( bool favoriteOnly );
    bool isFavoriteItemsOnly() const;

    QObject* favoritesModel();

    /**
     * Finds the item with @p id in the list.
     * @return The pointer to the item or (if no item has been found) 0
     */
    AbstractDataPluginItem *findItem( const QString& id ) const;

    /**
     * Testing the existence of the item @p id in the list
     */
    bool itemExists( const QString& id ) const;

public Q_SLOTS:
    /**
     * Adds the @p items to the list of initialized items. It checks if items with the same id are
     * already in the list and ignores and deletes them in this case.
     */
    void addItemsToList( const QList<AbstractDataPluginItem*> &items );

    /**
     * Convenience method to add one item to the list. See addItemsToList
     */
    void addItemToList( AbstractDataPluginItem *item );

    /**
     * Removes all items
     */
    void clear();

 protected:
    /**
     * Managing to get @p number additional items in @p box. This includes generating a url and
     * downloading the corresponding file.
     * This method has to be implemented in a subclass.
     **/
    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     qint32 number = 10 ) = 0;

    /**
     * @brief Retrieve data for a specific item
     * @param id Item id of the item to retrieve
     */
    virtual void getItem( const QString &id );
       
    /**
     * Parse the @p file and generate items. The items will be added to the list or the method
     * starts additionally needed downloads.
     * This method has to be implemented in a subclass.
     **/
    virtual void parseFile( const QByteArray& file );
        
    /**
     * Downloads the file from @p url. @p item -> addDownloadedFile() will be called when the
     * download is finished.
     * @param url the file URL
     * @param type The type of the download (to be specified by the subclasser)
     * @param item the data plugin item
     **/
    void downloadItem( const QUrl& url, const QString& type, AbstractDataPluginItem *item );

    /**
     * Download the description file from the @p url.
     */
    void downloadDescriptionFile( const QUrl& url );

    void registerItemProperties( const QMetaObject& item );
    
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

    void scheduleItemSort();

    void themeChanged();

 Q_SIGNALS:
    void itemsUpdated();
    void favoriteItemsChanged( const QStringList& favoriteItems );
    void favoriteItemsOnlyChanged();

 private:
    AbstractDataPluginModelPrivate * const d;
    friend class AbstractDataPluginModelPrivate;
};

}

#endif
