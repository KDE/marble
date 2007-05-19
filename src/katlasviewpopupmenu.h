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


#include <QtCore/QObject>
#include <QtGui/QAction>


/**
@author Torsten Rahn
*/

class QMenu;

class PlaceMark;
class KAtlasView;
class KAtlasGlobe;


class KAtlasViewPopupMenu  : public QObject 
{
    Q_OBJECT

 public:
    KAtlasViewPopupMenu(KAtlasView*, KAtlasGlobe*);
    virtual ~KAtlasViewPopupMenu(){};

 public slots:
    void  showLmbMenu( int, int );
    void  showRmbMenu( int, int );
    void  showFeatureInfo( QAction* );

 signals:
    void  addMeasurePoint( float, float );
    void  removeMeasurePoints();

 protected slots:
    void  slotAddMeasurePoint();
    void  slotAboutDialog();

 private:
    void createActions();

 private:
    KAtlasGlobe  *m_globe;
    KAtlasView   *m_view;

    QVector<PlaceMark*>  m_featurelist;	

    QMenu    *m_lmbMenu;
    QMenu    *m_rmbMenu;
    QAction  *m_earthaction;	
    QAction  *m_posaction;

    QAction  *m_pAddMeasurePointAction;	
    QAction  *m_pRemoveMeasurePointsAction;	
    QAction  *m_pAboutDialogAction;	

};

#endif // KATLASVIEWPOPUPMENU_H
