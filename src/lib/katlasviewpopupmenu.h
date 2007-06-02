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

//
// The KAtlasViewPopupMenu handles context menus.
//


#ifndef KATLASVIEWPOPUPMENU_H
#define KATLASVIEWPOPUPMENU_H


#include <QtCore/QObject>
#include <QtGui/QAction>


class QMenu;

class PlaceMark;
class MarbleWidget;
class MarbleModel;


class KAtlasViewPopupMenu  : public QObject 
{
    Q_OBJECT

 public:
    KAtlasViewPopupMenu(MarbleWidget*, MarbleModel*);
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
    MarbleModel    *m_model;
    MarbleWidget   *m_view;

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
