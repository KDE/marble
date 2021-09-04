// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_BOOKMARKMANAGER_H
#define MARBLE_BOOKMARKMANAGER_H

#include <QObject>
#include <QVector>
#include "MarbleGlobal.h"

class QString;

namespace Marble
{

    class BookmarkManagerPrivate;
    class GeoDataContainer;
    class GeoDataDocument;
    class GeoDataPlacemark;
    class GeoDataCoordinates;
    class GeoDataFolder;
    class GeoDataTreeModel;
    class StyleBuilder;
/**
 * This class is responsible for loading the
 * book mark objects from the files and various
 * book mark operations
 */

class MARBLE_EXPORT BookmarkManager : public QObject
{
    Q_OBJECT

 public:

    explicit BookmarkManager( GeoDataTreeModel *treeModel, QObject *parent = nullptr );

    ~BookmarkManager() override;

    /**
      * @brief load bookmark file as GeoDataDocument and return true
      * if loaded successfully else false
      * @param relativeFilePath relative path of bookmark file
      */
    bool loadFile( const QString &relativeFilePath );

    /**
      * @brief return bookmark file path
      */
    QString bookmarkFile() const;

    /**
      * @brief add bookmark in a folder
      * @param bookmark bookmark to be added
      * @param folder folder to add bookmark to
      */
    void addBookmark( GeoDataContainer *folder, const GeoDataPlacemark &bookmark ) ;

    void updateBookmark( GeoDataPlacemark *bookmark );

    void removeBookmark( GeoDataPlacemark *bookmark );

    /**
      * @brief checks all the bookmarks in container recursively and returns
      * pointer to the one having the same coordinate as the provided
      */
    GeoDataPlacemark* bookmarkAt(GeoDataContainer *container, const GeoDataCoordinates &coordinate);

    GeoDataDocument * document();
    const GeoDataDocument * document() const;

    bool showBookmarks() const;

    /**
      * @brief return Vector of folders
      */
    QVector<GeoDataFolder*> folders() const;

    /**
      * @brief add a folder
      * @param container geodata container
      * @param name name of folder to be created
      * @return pointer to the newly added folder
      */
    GeoDataFolder* addNewBookmarkFolder( GeoDataContainer *container, const QString &name );

    void renameBookmarkFolder( GeoDataFolder *folder, const QString &name );

    void removeBookmarkFolder( GeoDataFolder *folder );

    /**
     * @brief checks that there is at least one folder
     */

    void ensureDefaultFolder();

    /**
      * @brief remove all folders and bookmarks except default folder
      */
    void removeAllBookmarks();

    /**
     * @since 0.26.0
     */
    void setStyleBuilder(const StyleBuilder* styleBuilder);

public Q_SLOTS:
    void setShowBookmarks( bool visible );

Q_SIGNALS:
    /** One or more bookmarks were added or removed */
    void bookmarksChanged();

 private:
   friend class BookmarkManagerDialog;

   /**
    * @brief updates bookmark file and return true if updated successfully
    */
    bool updateBookmarkFile();

    static GeoDataDocument* openFile( const QString& fileName );

    BookmarkManagerPrivate* const d;

};

}


#endif
