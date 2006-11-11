//
// C++ Interface: katlastilecreatordialog
//
// Description: KAtlasTileCreatorDialog 

// The KAtlasTileCreatorDialog displays the progress of the tile creation.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PLACEMARKINFODIALOG_H
#define PLACEMARKINFODIALOG_H

#include "ui_placemarkinfodialog.h"

/**
@author Torsten Rahn
*/

class PlaceMark;

class PlaceMarkInfoDialog : public QDialog, private Ui::PlaceMarkInfoDialog {

Q_OBJECT

public:
	PlaceMarkInfoDialog( PlaceMark*, QWidget *parent = 0 );

public slots:

protected:
	PlaceMark* m_mark;
	void showContent();
	const QPixmap flag( const QString& ); 
};

#endif // PLACEMARKINFODIALOG_H
