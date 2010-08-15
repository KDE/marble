//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010    Gaurav Gupta <1989.gaurav@googlemail.com> 
//

#ifndef MARBLE_BOOKMARKMANAGER_H
#define MARBLE_BOOKMARKMANAGER_H

#include <QtCore/QString>
#include "BookmarkManager_p.h"

namespace Marble
{

    class GeoDataPlacemark;
    class GeoDataFolder;
/**
 * This class is responsible for loading the
 * book mark objects from the files and various 
 * book mark operations
 */

class BookmarkManager
{

 public:

    BookmarkManager();

    ~BookmarkManager();
    
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
      * @param folderName folder name in which bookmark to be added
      */
    void addBookmark( const GeoDataPlacemark &bookmark, const QString &folderName ) ;

    /**
      * @brief return Vector of folders
      */
    QVector<GeoDataFolder*> folders() const;

    /**
      * @brief add a folder
      * @param folder name of folder to be created
      */
    void addNewBookmarkFolder( const QString &folder );
    
    /**
      * @brief remove all folders and bookmarks except default folder
      */
    void removeAllBookmarks();
    
 private:
   
   /**
    * @brief updates bookmark file and return true if updated successfully
    */ 
    bool updateBookmarkFile() const;    

    BookmarkManagerPrivate *d;

};

}


#endif
