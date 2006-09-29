//
// C++ Interface: texcolorizer
//
// Description: GlobePediaControl 

// Testapplication with controls
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASCONTROL_H
#define KATLASCONTROL_H

#include "katlasview.h"

/**
@author Torsten Rahn
*/

class KAtlasToolBox;

class KAtlasControl : public QWidget
{
    Q_OBJECT

public:
	KAtlasControl(QWidget *);
	virtual ~KAtlasControl(){};

	void zoomIn();
	void zoomOut();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	void addPlaceMarkFile( QString filename ){ gpview->addPlaceMarkFile( filename ); }

private:
	KAtlasView* gpview;
	KAtlasToolBox* toolbox;
};

#endif // KATLASCONTROL_H
