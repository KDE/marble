// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_NEWBOOKMARKFOLDERDIALOG_H
#define MARBLE_NEWBOOKMARKFOLDERDIALOG_H

#include "marble_export.h"
#include "ui_NewBookmarkFolderDialog.h"

namespace Marble
{

class MARBLE_EXPORT NewBookmarkFolderDialog : public QDialog, private Ui::UiNewBookmarkFolderDialog
{
    Q_OBJECT

public:
    explicit NewBookmarkFolderDialog(QWidget *parent = nullptr);

    ~NewBookmarkFolderDialog() override;

    void setFolderName(const QString &name);

    QString folderName() const;

private:
    Q_DISABLE_COPY(NewBookmarkFolderDialog)
};

}

#endif
