// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_BOOKMARKMANAGERDIALOG_H
#define MARBLE_BOOKMARKMANAGERDIALOG_H

#include "marble_export.h"
#include "ui_BookmarkManagerDialog.h"

#include <QDialog>

namespace Marble
{

class GeoDataDocument;
class GeoDataContainer;
class MarbleModel;
class BookmarkManagerDialogPrivate;

class MARBLE_EXPORT BookmarkManagerDialog : public QDialog, private Ui::UiBookmarkManagerDialog
{
    Q_OBJECT

public:
    /** Constructor. The model is used to retrieve the bookmark manager */
    explicit BookmarkManagerDialog(MarbleModel *model, QWidget *parent = nullptr);

    /** Destructor */
    ~BookmarkManagerDialog() override;

    void setButtonBoxVisible(bool visible);

private Q_SLOTS:
    void saveBookmarks();

    void exportBookmarks();

    void importBookmarks();

private:
    Q_PRIVATE_SLOT(d, void updateButtonState())

    Q_PRIVATE_SLOT(d, void addNewFolder())

    Q_PRIVATE_SLOT(d, void renameFolder())

    Q_PRIVATE_SLOT(d, void deleteFolder())

    Q_PRIVATE_SLOT(d, void editBookmark())

    Q_PRIVATE_SLOT(d, void deleteBookmark())

    Q_PRIVATE_SLOT(d, void handleFolderSelection(const QModelIndex &index))

    Q_PRIVATE_SLOT(d, void discardChanges())

    Q_DISABLE_COPY(BookmarkManagerDialog)
    BookmarkManagerDialogPrivate *const d;
    friend class BookmarkManagerDialogPrivate;
};

}
#endif
