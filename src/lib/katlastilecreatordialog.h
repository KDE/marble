//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The KAtlasTileCreatorDialog displays the progress of the tile creation.
//


#ifndef KATLASTILECREATORDIALOG_H
#define KATLASTILECREATORDIALOG_H


#include "ui_katlastilecreatordialog.h"


class KAtlasTileCreatorDialog : public QDialog, private Ui::KAtlasTileCreatorDialog
{

    Q_OBJECT

 public:
    KAtlasTileCreatorDialog( QWidget *parent = 0 );

 public slots:
    void setProgress( int progress );
    void setSummary( const QString& name, const QString& description );
};


#endif // KATLASTILECREATORDIALOG_H
