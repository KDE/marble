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

#include "katlasflag.h"

#include <QStatusBar>
#include <QUrl>
/**
@author Torsten Rahn
*/

class PlaceMark;

class PlaceMarkInfoDialog : public QDialog, private Ui::PlaceMarkInfoDialog {

Q_OBJECT

public:
	PlaceMarkInfoDialog( PlaceMark*, QWidget *parent = 0 );

signals:

	void source( QUrl );

public slots:
	void setFlagLabel();
	void showMessage( QString text ){
		QFont statusFont = QStatusBar().font();
		statusFont.setPointSize( qRound( 0.9 * statusFont.pointSize() ) );
		m_pStatusLabel->setFont( statusFont );
		m_pStatusLabel->setText( text );
	}

protected:
	KAtlasFlag* m_flagcreator;
	PlaceMark* m_mark;
	void showContent();
	void requestFlag( const QString& );
//	QStatusBar * m_pStatusBar;
};

#endif // PLACEMARKINFODIALOG_H
