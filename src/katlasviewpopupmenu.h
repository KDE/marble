//
// C++ Interface: katlasviewcontextmenu.h
//
// Description: KAtlasViewPopupMenu 

// The KAtlasViewPopupMenu handles context menus.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASVIEWPOPUPMENU_H
#define KATLASVIEWPOPUPMENU_H

#include <QAction>
#include <QObject>

/**
@author Torsten Rahn
*/
class KAtlasView;
class KAtlasGlobe;
class QMenu;
class PlaceMark;

class KAtlasViewPopupMenu  : public QObject {

    Q_OBJECT

public:
	KAtlasViewPopupMenu(KAtlasView*, KAtlasGlobe*);
	virtual ~KAtlasViewPopupMenu(){};

public slots:
	void showLmbMenu( int, int );
	void showFeatureInfo( QAction* );

protected:
	KAtlasView* m_gpview;
	KAtlasGlobe* m_globe;

	QVector<PlaceMark*> m_featurelist;	

	QMenu* m_lmbMenu;
	QAction* m_earthaction;	
	QAction* m_posaction;	

	void createActions();
};

#endif // KATLASVIEWPOPUPMENU_H
