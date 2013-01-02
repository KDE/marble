//
// This file is part of the Marble Virtual Globe.
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

#include "marble_export.h"

#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include <QtCore/QVector>
#include <QtCore/QPoint>

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
class MapInfoDialog;


class MARBLE_EXPORT MarbleWidgetPopupMenu  : public QObject
{
    Q_OBJECT

 public:
    MarbleWidgetPopupMenu(MarbleWidget*, const MarbleModel*);
    virtual ~MarbleWidgetPopupMenu(){}

    /**
      * Adds the action to the menu associated with the specified
      * mouse button
      */
    void addAction( Qt::MouseButton button, QAction* action );

    QPoint mousePosition() const;

Q_SIGNALS:
    void trackPlacemark( const GeoDataPlacemark *placemark );

 public Q_SLOTS:
    void  showLmbMenu( int, int );
    void  showRmbMenu( int, int );

 protected Q_SLOTS:
    void slotInfoDialog();
    void slotShowOrbit( bool show );
    void slotTrackPlacemark();
    void slotCopyCoordinates();
    void slotAboutDialog();

private Q_SLOTS:
    void directionsFromHere();
    void directionsToHere();
    void startReverseGeocoding();
    void showAddressInformation( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );
    void addBookmark();
    void toggleFullscreen( bool enabled );

 private:
    /**
      * Returns the geo coordinates of the mouse pointer at the last right button menu.
      * You must not pass 0 as coordinates parameter. The result indicates whether the
      * coordinates are valid, which will be true if the right button menu was opened at least once.
      */
    bool mouseCoordinates( GeoDataCoordinates* coordinates, QAction* dataContainer );
    QMenu* createInfoBoxMenu();

    void setupDialogSatellite(MapInfoDialog *popup, const GeoDataPlacemark *index);
    void setupDialogCity(MapInfoDialog *popup, const GeoDataPlacemark *index);

 private:
    Q_DISABLE_COPY( MarbleWidgetPopupMenu )
    const MarbleModel    *const m_model;
    MarbleWidget   *m_widget;

    QVector<const GeoDataPlacemark*>  m_featurelist;
    QList<AbstractDataPluginItem *> m_itemList;

    QMenu    *const m_lmbMenu;
    QMenu    *const m_rmbMenu;

    QAction *m_infoDialogAction;
    QAction *m_showOrbitAction;
    QAction *m_trackPlacemarkAction;

    QAction  *const m_copyCoordinateAction;

    QAction  *m_rmbExtensionPoint;

    MarbleRunnerManager *const m_runnerManager;

    QPoint m_mousePosition;
};

}

#endif
