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

#ifndef MARBLE_PLACEMARKINFODIALOG_H
#define MARBLE_PLACEMARKINFODIALOG_H

#include <QtCore/QPersistentModelIndex>

class QString;

// FIXME: remove "using namespace" from header
namespace Marble
{
}
using namespace Marble;

#include "ui_PlacemarkInfoDialog.h"

namespace Marble
{
class DeferredFlag;

class PlacemarkInfoDialog : public QDialog, private Ui::PlacemarkInfoDialog
{

    Q_OBJECT

 public:
    explicit PlacemarkInfoDialog( const QPersistentModelIndex &index, QWidget *parent = 0 );

 Q_SIGNALS:
    void source( const QString& );
    
 public Q_SLOTS:
    void setFlagLabel();
    void showMessage( const QString& text );

 protected:
    void showContent();
    void requestFlag( const QString& );

 private:
    Q_DISABLE_COPY( PlacemarkInfoDialog )
    DeferredFlag  *m_flagcreator;
    QPersistentModelIndex m_index;
};

}

#endif
