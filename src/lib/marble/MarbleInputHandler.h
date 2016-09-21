//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#ifndef MARBLE_MARBLEINPUTHANDLER_H
#define MARBLE_MARBLEINPUTHANDLER_H

#include <QObject>

#include "marble_export.h"
#include "GeoDataCoordinates.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QTouchEvent;
class QWheelEvent;
class QGestureEvent;
class QCursor;
class QTimer;
class QString;
class QRect;

namespace Marble
{

class MarbleAbstractPresenter;
class AbstractDataPluginItem;
class RenderPlugin;

class MARBLE_EXPORT MarbleInputHandler  : public QObject
{
    Q_OBJECT

 public:
    explicit MarbleInputHandler( MarbleAbstractPresenter* );
    virtual ~MarbleInputHandler();

    void setPositionSignalConnected( bool connected );
    bool isPositionSignalConnected() const;

    /**
//
// The MarbleInputHandler handles mouse and keyboard input.
//

     * @brief  Set whether a popup menu appears on a click (not drag) with the left mouse button
     * @param  enabled True to enable the popup menu (default), false to disable it
     */
    void setMouseButtonPopupEnabled( Qt::MouseButton mouseButton, bool enabled );

    /**
     * @brief  Return whether the left mouse button popup menu is active
     * @return True iff a popup menu is shown on left mouse button clicks
     */
    bool isMouseButtonPopupEnabled( Qt::MouseButton mouseButton ) const;

    void setPanViaArrowsEnabled( bool enabled );

    bool panViaArrowsEnabled() const;

    void setInertialEarthRotationEnabled( bool enabled );

    /**
     * @brief Returns true iff dragging the map with the mouse keeps spinning
     * in the chosen direction for a slightly longer time than the mouse is
     * actually performing the drag operation
     */
    bool inertialEarthRotationEnabled() const;

 Q_SIGNALS:
    // Mouse button menus
    void lmbRequest( int, int );
    void rmbRequest( int, int );

    //Gps coordinates
    void mouseClickScreenPosition( int, int );
    void mouseMoveGeoPosition( const QString& );

    /*
     * To detect mouse click followed by mouse move
     * with no mouse move in between
     */
    void mouseClickGeoPosition( qreal, qreal, GeoDataCoordinates::Unit );

 protected Q_SLOTS:
    void restoreViewContext();

 protected:
    class Protected;
    Protected * const d;

 private Q_SLOTS:
    virtual void installPluginEventFilter( RenderPlugin *renderPlugin ) = 0;

 private:
    Q_DISABLE_COPY( MarbleInputHandler )
};

class AbstractSelectionRubber
{
public:
    virtual ~AbstractSelectionRubber() {}
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isVisible() const = 0;
    virtual const QRect &geometry() const = 0;
    virtual void setGeometry(const QRect &geometry) = 0;
};

class MARBLE_EXPORT MarbleDefaultInputHandler  : public MarbleInputHandler
{
    Q_OBJECT

 public:
    explicit MarbleDefaultInputHandler( MarbleAbstractPresenter* marblePresenter);
    virtual ~MarbleDefaultInputHandler();

 protected:
    bool eventFilter( QObject *, QEvent * );
    bool handleMouseEvent(QMouseEvent *e);
    bool handlePinch(const QPointF &center, qreal scaleFactor, Qt::GestureState state);

    //FIXME - refactor (abstraction & composition)
    QPointer<AbstractDataPluginItem> lastToolTipItem();
    QTimer* toolTipTimer();
    QPoint toolTipPosition();

    virtual bool handleKeyPress(QKeyEvent *e);
    virtual void handleMouseButtonPressAndHold(const QPoint &position);

 private Q_SLOTS:
    virtual void installPluginEventFilter( RenderPlugin *renderPlugin ) = 0;
    virtual void showLmbMenu( int, int ) = 0;
    virtual void showRmbMenu( int, int ) = 0;
    void handlePressAndHold();

    virtual void openItemToolTip() = 0;
    virtual void setCursor(const QCursor &) = 0;

    void lmbTimeout();

 private:
    virtual AbstractSelectionRubber *selectionRubber() = 0;
    virtual bool layersEventFilter(QObject *, QEvent *) = 0;

    virtual bool handleTouch(QTouchEvent *e);
    virtual bool handleDoubleClick(QMouseEvent *e);
    virtual bool handleWheel(QWheelEvent *e);
    virtual bool handleGesture(QGestureEvent *e);

    virtual void handleMouseButtonPress(QMouseEvent *e);
    virtual void handleLeftMouseButtonPress(QMouseEvent *e);
    virtual void handleRightMouseButtonPress(QMouseEvent *e);
    virtual void handleMiddleMouseButtonPress(QMouseEvent *e);
    virtual void handleMouseButtonRelease(QMouseEvent *e);

    virtual void hideSelectionIfCtrlReleased(QEvent *e);
    virtual void checkReleasedMove(QMouseEvent *e);

    //Returns whatever should be returned from mouse event handling loop
    virtual bool acceptMouse();

    void notifyPosition(bool isAboveMap, qreal mouseLon, qreal mouseLat);
    QPoint mouseMovedOutside(QMouseEvent *event);
    void adjustCursorShape(const QPoint& mousePosition, const QPoint& mouseDirection);

    Q_DISABLE_COPY(MarbleDefaultInputHandler)
    class Private;
    Private * const d;
};

}

#endif  //MARBLE_MARBLEINPUTHANDLER_H
