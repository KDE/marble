// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

//
// The TileCreatorDialog displays the progress of the tile creation.
//

#ifndef MARBLE_TILECREATORDIALOG_H
#define MARBLE_TILECREATORDIALOG_H

#include <QDialog>

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
    explicit TileCreatorDialog( TileCreator *creator, QWidget *parent = nullptr );
    ~TileCreatorDialog() override;

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
