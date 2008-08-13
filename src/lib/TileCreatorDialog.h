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
// The TileCreatorDialog displays the progress of the tile creation.
//


#ifndef TILECREATORDIALOG_H
#define TILECREATORDIALOG_H


#include "ui_TileCreatorDialog.h"

namespace Marble
{

class TileCreator;

class TileCreatorDialog : public QDialog, private Ui::TileCreatorDialog
{

    Q_OBJECT

 public:
    explicit TileCreatorDialog( TileCreator *creator, QWidget *parent = 0 );
    ~TileCreatorDialog();

 public Q_SLOTS:
    void setProgress( int progress );
    void setSummary( const QString& name, const QString& description );

 private:
    TileCreator *m_creator;
};

}

#endif // TILECREATORDIALOG_H
