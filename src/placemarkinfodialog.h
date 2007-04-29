//
// C++ Interface: placemarkinfodialog
//
// Description: PlaceMarkInfoDialog 

// The PlaceMarkInfoDialog... (FIXME)
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution


#ifndef PLACEMARKINFODIALOG_H
#define PLACEMARKINFODIALOG_H


#include "ui_placemarkinfodialog.h"

#include <QtGui/QStatusBar>
#include <QtCore/QUrl>

#include "katlasflag.h"


/**
@author Torsten Rahn
*/


class PlaceMark;

class PlaceMarkInfoDialog : public QDialog, private Ui::PlaceMarkInfoDialog
{

    Q_OBJECT

 public:
    PlaceMarkInfoDialog( PlaceMark*, QWidget *parent = 0 );

 signals:
    void source( QUrl );

 public slots:
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

 protected:
    KAtlasFlag  *m_flagcreator;
    PlaceMark   *m_mark;
};


#endif // PLACEMARKINFODIALOG_H
