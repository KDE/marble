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


#ifndef PLACEMARKINFODIALOG_H
#define PLACEMARKINFODIALOG_H


#include "ui_PlaceMarkInfoDialog.h"

#include <QtCore/QPersistentModelIndex>
#include <QtCore/QUrl>
#include <QtGui/QStatusBar>

#include "DeferredFlag.h"


class PlaceMark;

class PlaceMarkInfoDialog : public QDialog, private Ui::PlaceMarkInfoDialog
{

    Q_OBJECT

 public:
    explicit PlaceMarkInfoDialog( const QPersistentModelIndex &index, QWidget *parent = 0 );

 Q_SIGNALS:
    void source( QString );

 public Q_SLOTS:
    void setFlagLabel();
    void showMessage( QString text )
    {
        QFont  statusFont = QStatusBar().font();
        statusFont.setPointSize( qRound( 0.9 * statusFont.pointSize() ) );
        m_pStatusLabel->setFont( statusFont );
        m_pStatusLabel->setText( text );
    }

 protected:
    void showContent();
    void requestFlag( const QString& );

 private:
    DeferredFlag  *m_flagcreator;
    QPersistentModelIndex m_index;
};


#endif // PLACEMARKINFODIALOG_H
