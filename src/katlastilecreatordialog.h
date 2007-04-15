//
// C++ Interface: katlastilecreatordialog
//
// Description: KAtlasTileCreatorDialog 

// The KAtlasTileCreatorDialog displays the progress of the tile creation.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution


#ifndef KATLASTILECREATORDIALOG_H
#define KATLASTILECREATORDIALOG_H


#include "ui_katlastilecreatordialog.h"


/**
@author Torsten Rahn
*/

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
