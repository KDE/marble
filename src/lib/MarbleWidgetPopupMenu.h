//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The MarbleWidgetPopupMenu handles context menus.
//

#ifndef MARBLE_MARBLEWIDGETPOPUPMENU_H
#define MARBLE_MARBLEWIDGETPOPUPMENU_H


#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include <QtCore/QVector>


class QAction;
class QMenu;

namespace Marble
{

class AbstractDataPluginItem;
class MarbleWidget;
class MarbleModel;
class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleRunnerManager;


class MarbleWidgetPopupMenu  : public QObject 
{
    Q_OBJECT

 public:
    MarbleWidgetPopupMenu(MarbleWidget*, MarbleModel*);
    virtual ~MarbleWidgetPopupMenu(){}

    /**
      * Adds the action to the menu associated with the specified
      * mouse button
      */
    void addAction( Qt::MouseButton button, QAction* action );

 public Q_SLOTS:
    void  showLmbMenu( int, int );
    void  showRmbMenu( int, int );
    void  showFeatureInfo( QAction* );

 protected Q_SLOTS:
    void  slotSetHomePoint();
    void  slotCopyCoordinates();
    void  slotAboutDialog();

private Q_SLOTS:
    void directionsFromHere();
    void directionsToHere();
    void startReverseGeocoding();
    void showAddressInformation( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );
    void addBookmark();
    void openGoToDialog();

 private:
    /**
      * Returns the geo coordinates of the mouse pointer at the last right button menu.
      * You must not pass 0 as coordinates parameter. The result indicates whether the
      * coordinates are valid, which will be true if the right button menu was opened at least once.
      */
    bool mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer );
    void createActions();

 private:
    Q_DISABLE_COPY( MarbleWidgetPopupMenu )
    MarbleModel    *m_model;
    MarbleWidget   *m_widget;

    QVector<QModelIndex>  m_featurelist;
    QList<AbstractDataPluginItem *> m_itemList;

    QMenu    *m_lmbMenu;
    QMenu    *m_rmbMenu;
    QMenu    *m_smallScreenMenu;
    QAction  *m_planetAction;	

    QAction  *m_copyCoordinateAction;
    QAction  *m_setHomePointAction; 
    QAction  *m_aboutDialogAction;

    QAction  *m_rmbExtensionPoint;

    MarbleRunnerManager* m_runnerManager;
};

}

#endif
