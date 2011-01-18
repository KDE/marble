//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_BOOKMARKMANAGERDIALOG_H
#define MARBLE_BOOKMARKMANAGERDIALOG_H

#include "ui_BookmarkManagerDialog.h"
#include "marble_export.h"

#include <QtGui/QDialog>

namespace Marble
{

class MarbleModel;
class BookmarkManagerDialogPrivate;

class MARBLE_EXPORT BookmarkManagerDialog : public QDialog, private Ui::UiBookmarkManagerDialog
{
    Q_OBJECT

 public:
    /** Constructor. The model is used to retrieve the bookmark manager */
    explicit BookmarkManagerDialog( MarbleModel* model, QWidget *parent = 0 );

    /** Destructor */
    ~BookmarkManagerDialog();

private Q_SLOTS:
    void saveBookmarks();

private:
    Q_PRIVATE_SLOT( d, void updateButtonState() );

    Q_PRIVATE_SLOT( d, void renameFolder() );

    Q_PRIVATE_SLOT( d, void editBookmark() );

    Q_PRIVATE_SLOT( d, void filterBookmarksByFolder( const QModelIndex &index ) );

    Q_PRIVATE_SLOT( d, void discardChanges() );

    Q_DISABLE_COPY( BookmarkManagerDialog )
    BookmarkManagerDialogPrivate* const d;
    friend class BookmarkManagerDialogPrivate;
};

}
#endif
