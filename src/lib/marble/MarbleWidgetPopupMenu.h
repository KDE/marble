// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#ifndef MARBLE_MARBLEWIDGETPOPUPMENU_H
#define MARBLE_MARBLEWIDGETPOPUPMENU_H

#include "marble_export.h"

#include <QObject>

class QAction;
class QPoint;

namespace Marble
{

class MarbleWidget;
class MarbleModel;
class GeoDataCoordinates;
class GeoDataPlacemark;

/**
 * The MarbleWidgetPopupMenu handles context menus.
 */
class MARBLE_EXPORT MarbleWidgetPopupMenu  : public QObject
{
    Q_OBJECT

 public:
    MarbleWidgetPopupMenu( MarbleWidget*, const MarbleModel* );

    ~MarbleWidgetPopupMenu() override;

    /**
      * Adds the action to the menu associated with the specified
      * mouse button
      */
    void addAction( Qt::MouseButton button, QAction* action );

    /**
     * @brief mousePosition Position of the last mouse button click
     * @return
     */
    QPoint mousePosition() const;

 public Q_SLOTS:
    void  showLmbMenu( int, int );
    void  showRmbMenu( int, int );

private Q_SLOTS:
    void resetMenu();
    void slotInfoDialog();
    void slotCopyCoordinates();
    void slotCopyGeo();
    void slotAboutDialog();

private Q_SLOTS:
    void directionsFromHere();
    void directionsToHere();
    void startReverseGeocoding();
    void showAddressInformation( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );
    void addBookmark();
    void toggleFullscreen( bool enabled );

 private:
    Q_DISABLE_COPY( MarbleWidgetPopupMenu )
    class Private;
    Private* const d;
};

}

#endif
