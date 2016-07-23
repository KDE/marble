//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_BOOKMARKMANAGERPRIVATE_H
#define MARBLE_BOOKMARKMANAGERPRIVATE_H

#include <QString>

namespace Marble
{

class GeoDataContainer;
class GeoDataDocument;
class GeoDataTreeModel;
class StyleBuilder;

class BookmarkManagerPrivate
{
public:
    explicit BookmarkManagerPrivate( GeoDataTreeModel *treeModel );

    ~BookmarkManagerPrivate();

    void resetBookmarkDocument();

    static void setVisualCategory( GeoDataContainer *container );

    GeoDataTreeModel *const m_treeModel;

    GeoDataDocument *m_bookmarkDocument;

    QString m_bookmarkFileRelativePath;
    const StyleBuilder* m_styleBuilder;
};

}

#endif
