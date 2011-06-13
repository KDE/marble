//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The TileCreatorDialog displays the progress of the tile creation.
//

#ifndef MARBLE_TILECREATORDIALOG_H
#define MARBLE_TILECREATORDIALOG_H

#include <QtGui/QDialog>

#include "marble_export.h"

class QString;

namespace Marble
{

class TileCreator;
class TileCreatorDialogPrivate;

class MARBLE_EXPORT TileCreatorDialog : public QDialog
{

    Q_OBJECT

 public:
    explicit TileCreatorDialog( TileCreator *creator, QWidget *parent = 0 );
    ~TileCreatorDialog();

 public Q_SLOTS:
    void setProgress( int progress );
    void setSummary( const QString& name, const QString& description );

private Q_SLOTS:
    void cancelTileCreation();

 private:
    Q_DISABLE_COPY( TileCreatorDialog )
    TileCreatorDialogPrivate  * const d;
};

}

#endif
