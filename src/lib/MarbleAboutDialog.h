//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The about dialog 
//

#ifndef MARBLE_MARBLEABOUTDIALOG_H
#define MARBLE_MARBLEABOUTDIALOG_H


#include <QtGui/QDialog>

#include "marble_export.h"

namespace Marble
{

class MarbleAboutDialogPrivate;


class MARBLE_EXPORT MarbleAboutDialog : public QDialog
{
    Q_OBJECT

public:
    enum Tab {
        About,
        Authors,
        Data,
        License
    };

    explicit MarbleAboutDialog(QWidget *parent = 0);
    ~MarbleAboutDialog();

    /**
      * Set the application title (usually name and version) to show
      * The default application title is 'Marble Virtual Globe'
      */
    void setApplicationTitle( const QString &title );

    /**
     * @brief setInitialTab Change the tab shown when executing the dialog
     * @param tab The tab to show
     */
    void setInitialTab( Tab tab );

 private:
    Q_PRIVATE_SLOT( d, void loadPageContents( int idx ) )
    Q_DISABLE_COPY( MarbleAboutDialog )
    MarbleAboutDialogPrivate  * const d;
};

}

#endif
