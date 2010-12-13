//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>     
//

#ifndef MARBLE_BOOKMARKINFODIALOG_H
#define MARBLE_BOOKMARKINFODIALOG_H

#include "ui_BookmarkInfoDialog.h"
#include "MarbleWidget.h"
#include "marble_export.h"
#include "MarbleRunnerManager.h"

/** @todo FIXME after freeze: Rename to AddBookmarkDialog*/

namespace Marble
{

class BookmarkInfoDialogPrivate;

class MARBLE_EXPORT BookmarkInfoDialog : public QDialog, private Ui::BookmarkInfoDialog
{
    Q_OBJECT

 public:
    /** Constructor that uses the widget's center as the bookmark location */
    explicit BookmarkInfoDialog( MarbleWidget *marbleWidget );

    /** Constructor that uses the given coordinates as the bookmark location */
    BookmarkInfoDialog( const GeoDataCoordinates &coordinates, MarbleWidget *marbleWidget );

    /** Destructor */
    ~BookmarkInfoDialog();

 private Q_SLOTS:
    void addBookmark();
    
    void openNewFolderDialog();

    void retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark);

    QString append( const QString &bookmark, const QString &text);

 private:
    Q_DISABLE_COPY( BookmarkInfoDialog )
    BookmarkInfoDialogPrivate* const d;
    friend class BookmarkInfoDialogPrivate;
};

}
#endif
