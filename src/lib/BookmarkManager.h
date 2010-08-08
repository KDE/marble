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

    bool loadFile( const QString &relativeFilePath );
    
    void addBookmark( const GeoDataPlacemark &bookmark, const QString &folderName ) ;

    QVector<GeoDataFolder*> folders() const;

    void addNewBookmarkFolder( const QString &folder );

    void removeAllBookmarks();
    
 private:
    
    bool updateBookmarkFile() const;    

    BookmarkManagerPrivate *d;

};

}


#endif
