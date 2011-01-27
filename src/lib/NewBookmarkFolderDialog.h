//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>     
//

#ifndef MARBLE_NEWBOOKMARKFOLDERINFODIALOG_H
#define MARBLE_NEWBOOKMARKFOLDERINFODIALOG_H

#include "ui_NewBookmarkFolderDialog.h"
#include "marble_export.h"

namespace Marble
{

class MarbleWidget;

class MARBLE_EXPORT NewBookmarkFolderDialog : public QDialog, private Ui::UiNewBookmarkFolderDialog
{

    Q_OBJECT

 public:
    explicit NewBookmarkFolderDialog( QWidget *parent = 0 );

    ~NewBookmarkFolderDialog();

    void setMarbleWidget( MarbleWidget* widget );

    void setFolderName( const QString &name );

    QString folderName() const;

 public Q_SLOTS:
   void addNewBookmarkFolder();

 private:
    Q_DISABLE_COPY( NewBookmarkFolderDialog )

    MarbleWidget *m_widget;
};

}

#endif
