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
// The MarbleWidgetPopupMenu handles context menus.
//


#ifndef MARBLEWIDGETPOPUPMENU_H
#define MARBLEWIDGETPOPUPMENU_H


#include <QtCore/QObject>
#include <QtCore/QPersistentModelIndex>
#include <QtGui/QAction>


class QMenu;

namespace Marble
{

class MarbleWidget;
class MarbleModel;


class MarbleWidgetPopupMenu  : public QObject 
{
    Q_OBJECT

 public:
    MarbleWidgetPopupMenu(MarbleWidget*, MarbleModel*);
    virtual ~MarbleWidgetPopupMenu(){}

 public Q_SLOTS:
    void  showLmbMenu( int, int );
    void  showRmbMenu( int, int );
    void  showFeatureInfo( QAction* );

 Q_SIGNALS:
    void  addMeasurePoint( qreal, qreal );
    void  removeMeasurePoints();

 protected Q_SLOTS:
    void  slotSetHomePoint();
    void  slotCopyCoordinates();
    void  slotAddMeasurePoint();
    void  slotRemoveMeasurePoints();
    void  slotAboutDialog();

 private:
    void createActions();

 private:
    Q_DISABLE_COPY( MarbleWidgetPopupMenu )
    MarbleModel    *m_model;
    MarbleWidget   *m_widget;

    QVector<QPersistentModelIndex>  m_featurelist;	

    QMenu    *m_lmbMenu;
    QMenu    *m_rmbMenu;
    QAction  *m_earthaction;	

    QAction  *m_copyCoordinateAction;
    QAction  *m_setHomePointAction; 
    QAction  *m_addMeasurePointAction;	
    QAction  *m_removeMeasurePointsAction;	
    QAction  *m_aboutDialogAction;	
};

}

#endif // MARBLEWIDGETPOPUPMENU_H
