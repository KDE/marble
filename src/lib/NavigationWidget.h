//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_NAVIGATIONWIDGET_H
#define MARBLE_NAVIGATIONWIDGET_H

// Marble
#include "marble_export.h"

// Qt
#include <QtGui/QWidget>

class QModelIndex;
class QAbstractItemModel;

namespace Marble
{

class NavigationWidgetPrivate;

class MarbleWidget;

class MARBLE_EXPORT NavigationWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit NavigationWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~NavigationWidget();

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget );

 Q_SIGNALS:
    /**
     * @brief Signal emitted when the Home button has been pressed.
     */
    void goHome();
    /**
     * @brief Signal emitted when the Zoom In button has been pressed.
     */
    void zoomIn();
    /**
     * @brief Signal emitted when the Zoom Out button has been pressed.
     */
    void zoomOut();
    /**
     * @brief Signal emitted when the zoom slider has been moved.
     * @param zoom  The new zoom value.
     */
    void zoomChanged( int zoom );

    /**
     * @brief Signal emitted when the Move Left button has been pressed.
     */
    void moveLeft();
    /**
     * @brief Signal emitted when the Move Right button has been pressed.
     */
    void moveRight();
    /**
     * @brief Signal emitted when the Move Up button has been pressed.
     */
    void moveUp();
    /**
     * @brief Signal emitted when the Move Down button has been pressed.
     */
    void moveDown();


 public Q_SLOTS:
    /**
     * @brief Set a list/set of placemark names for the search widget.
     * @param locations  the QAbstractitemModel containing the placemarks
     *
     * This function is called to display a potentially large number
     * of placemark names in a widget and let the user search between
     * them.
     * @see centerOn
     */
    void setLocations( QAbstractItemModel* locations );

    void selectTheme( const QString & );

    /**
     * @brief Sets the value of the slider.
     * @param zoom The new zoom value.
     *
     * This slot should be called when the zoom value is changed from
     * the widget itself, e.g. by using the scroll wheel.  It sets the
     * value of the slider, but nothing more.  In particular it
     * doesn't emit the zoomChanged signal.
     */
    void changeZoom( int zoom );

 private Q_SLOTS:
    /// called whenever the user types something new in the search box
    void searchLineChanged( const QString &search );

    /// called the Return or Enter key is pressed in the search box.
    void searchReturnPressed();

    /// called by the singleShot to initiate a search based on the searchLine
    void search();

    void updateButtons( int );

    void mapCenterOnSignal( const QModelIndex & );

    void adjustForAnimation();
    void adjustForStill();

 protected:
    /**
     * @brief Reimplementation of the resizeEvent() of the widget.
     *
     * If the MarbleControlBox gets shrunk enough, the slider in the
     * Navigation tab will be hidden, leaving only the Zoom Up and
     * Zoom Down buttons.
     */
    void resizeEvent( QResizeEvent * );

 private:
    Q_DISABLE_COPY( NavigationWidget )

    NavigationWidgetPrivate * const d;
};

}

#endif
