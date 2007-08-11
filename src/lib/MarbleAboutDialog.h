//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The about dialog 
//


#ifndef MARBLEABOUTDIALOG_H
#define MARBLEABOUTDIALOG_H


#include "marble_export.h"

#include "ui_MarbleAboutDialog.h"


class MARBLE_EXPORT MarbleAboutDialog : public QDialog, private Ui::MarbleAboutDialog
{
    Q_OBJECT

 public:
    MarbleAboutDialog(QWidget *parent = 0);
};


#endif // KATLASABOUTDIALOG_H
