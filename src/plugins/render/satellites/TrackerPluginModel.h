//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_TRACKERPLUGINMODEL_H
#define MARBLE_TRACKERPLUGINMODEL_H

#include <QtCore/QObject>

class QUrl;

namespace Marble
{

class GeoDataTreeModel;
class PluginManager;
class TrackerPluginItem;
class TrackerPluginModelPrivate;

/**
 * A model used to download, store and update items
 */
class TrackerPluginModel : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructs a model with the given @p treeModel and @p pluginManager.
     * 
     * These parameters can be obtained by calling treeModel() and
     * pluginManager() on an instance of MarbleModel.
     */
    TrackerPluginModel( GeoDataTreeModel *treeModel, const PluginManager *pluginManager  );

    virtual ~TrackerPluginModel();

    void enable( bool enabled );

    /**
     * Add the item @p mark to the model.
     *
     * @see beginUpdateItems, endUpdateItems
     */
    void addItem( TrackerPluginItem *mark );

    /**
     * Remove all items from the model.
     */
    void clear();

    /**
     * Begin a series of add or remove items operations on the model.
     *
     * Always call this method before adding or removing items to the model
     * and call endUpdateItems() once you're done updating the model.
     * @see endUpdateItems(), addItem()
     */
    void beginUpdateItems();

    /**
     * End a series of add or remove items operations on the model.
     *
     * Always call this method once you're finished adding and removing items
     * to the model.
     * @see beginUpdateItems(), addItem(), removeItem()
     */
    void endUpdateItems();

    /**
     * Adds @p url to the download queue.
     * Once the file is downloaded, parseFile() will be called with its first
     * parameter equals to @p id.
     */
    void downloadFile( const QUrl &url, const QString &id );

    /**
     * This method is called whenever a file queued up for download by
     * downloadFile() has finished downloading, reimplement it to use it.
     *
     * @param id The @p id parameter passed to downloadFile()
     * @param file The content of the file
     */
    virtual void parseFile( const QString &id, const QByteArray &file );

private:
    TrackerPluginModelPrivate *d;
    Q_PRIVATE_SLOT( d, void downloaded( const QString &, const QString & ) );
    Q_PRIVATE_SLOT( d, void update() );
};

} // namespace Marble

#endif // MARBLE_TRACKERPLUGINMODEL_H
