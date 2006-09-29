//
// C++ Interface: katlasviewinputhandler.h
//
// Description: KAtlasViewInputHandler 

// The KAtlasViewInputHandler handles mouse and keyboard input.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASVIEWINPUTHANDLER_H
#define KATLASVIEWINPUTHANDLER_H

#include <QObject>
#include <QKeyEvent>
#include <QEvent>
#include <QPixmap>
#include <QCursor>
#include "katlasglobe.h"

/**
@author Torsten Rahn
*/
class KAtlasView;

class KAtlasViewInputHandler  : public QObject {
public:
	KAtlasViewInputHandler(KAtlasView*, KAtlasGlobe*);
	virtual ~KAtlasViewInputHandler(){};

private:
	QPixmap curpmtl, curpmtc, curpmtr, curpmcr, curpmcl, curpmbl, curpmbc, curpmbr;
	QCursor arrowcur[3][3];
	KAtlasView* m_gpview;
	KAtlasGlobe* m_globe;
	int dirx, diry;

	bool m_pressed;
	int m_pressedx, m_pressedy;
	float m_presseda, m_pressedb;
protected:
	bool eventFilter( QObject *, QEvent * );
};

#endif // KATLASVIEWINPUTHANDLER
