//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#include "MarbleInputHandler.h"

#include <QPoint>
#include <QPointer>
#include <QTimer>
#include <QCursor>
#include <QMouseEvent>
#include <QPixmap>
#include <QGestureEvent>
#include <QPinchGesture>

#include "kineticmodel.h"
#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "MarbleMap.h"
#include "GeoDataCoordinates.h"
#include "MarbleAbstractPresenter.h"
#include "ViewportParams.h"
#include "AbstractFloatItem.h"
#include "AbstractDataPluginItem.h"
#include "RenderPlugin.h"

namespace Marble {

const int TOOLTIP_START_INTERVAL = 1000;

class Q_DECL_HIDDEN MarbleInputHandler::Protected
{
public:
    Protected(MarbleAbstractPresenter *marblePresenter);

    MarbleAbstractPresenter *const m_marblePresenter;
    bool m_positionSignalConnected;
    QTimer *m_mouseWheelTimer;
    Qt::MouseButtons m_disabledMouseButtons;
    qreal m_wheelZoomTargetDistance;
    bool m_panViaArrowsEnabled;
    bool m_inertialEarthRotation;
    int m_steps;
    const int m_discreteZoomSteps = 120;
};

MarbleInputHandler::Protected::Protected(MarbleAbstractPresenter *marblePresenter)
    : m_marblePresenter( marblePresenter ),
      m_positionSignalConnected( false ),
      m_mouseWheelTimer( 0 ),
      m_disabledMouseButtons( Qt::NoButton ),
      m_wheelZoomTargetDistance( 0.0 ),
      m_panViaArrowsEnabled( true ),
      m_inertialEarthRotation( true ),
      m_steps(0)
{
}

MarbleInputHandler::MarbleInputHandler(MarbleAbstractPresenter *marblePresenter)
    : d(new Protected(marblePresenter))
{
    d->m_mouseWheelTimer = new QTimer( this );
    connect(d->m_mouseWheelTimer, SIGNAL(timeout()), this, SLOT(restoreViewContext()));

    connect(d->m_marblePresenter->map(), SIGNAL(renderPluginInitialized(RenderPlugin*)),
             this, SLOT(installPluginEventFilter(RenderPlugin*)));
}

MarbleInputHandler::~MarbleInputHandler()
{
    delete d->m_mouseWheelTimer;
    delete d;
}

void MarbleInputHandler::setPositionSignalConnected(bool connected)
{
    d->m_positionSignalConnected = connected;
}

bool MarbleInputHandler::isPositionSignalConnected() const
{
    return d->m_positionSignalConnected;
}

void MarbleInputHandler::setMouseButtonPopupEnabled(Qt::MouseButton mouseButton, bool enabled)
{
    if (enabled)
    {
        d->m_disabledMouseButtons &= ~Qt::MouseButtons(mouseButton);
    }
    else
    {
        d->m_disabledMouseButtons |= mouseButton;
    }
}

bool MarbleInputHandler::isMouseButtonPopupEnabled(Qt::MouseButton mouseButton) const
{
    return !(d->m_disabledMouseButtons & mouseButton);
}

void MarbleInputHandler::setPanViaArrowsEnabled(bool enabled)
{
    d->m_panViaArrowsEnabled = enabled;
}

bool MarbleInputHandler::panViaArrowsEnabled() const
{
    return d->m_panViaArrowsEnabled;
}

void MarbleInputHandler::setInertialEarthRotationEnabled(bool enabled)
{
    d->m_inertialEarthRotation = enabled;
}

bool MarbleInputHandler::inertialEarthRotationEnabled() const
{
    return d->m_inertialEarthRotation;
}

class Q_DECL_HIDDEN MarbleDefaultInputHandler::Private
{
 public:
    Private();
    ~Private();

    QPixmap m_curpmtl;
    QPixmap m_curpmtc;
    QPixmap m_curpmtr;
    QPixmap m_curpmcr;
    QPixmap m_curpmcl;
    QPixmap m_curpmbl;
    QPixmap m_curpmbc;
    QPixmap m_curpmbr;

    QCursor m_arrowCur[3][3];

    // Indicates if the left mouse button has been pressed already.
    bool m_leftPressed;
    // Indicates whether the drag was started by a click above or below the visible pole.
    int m_leftPressedDirection;
    // Indicates if the middle mouse button has been pressed already.
    bool m_midPressed;
    // The mouse pointer x position when the left mouse button has been pressed.
    int m_leftPressedX;
    // The mouse pointer y position when the left mouse button has been pressed.
    int m_leftPressedY;
    // The mouse pointer y position when the middle mouse button has been pressed.
    int m_midPressedY;
    int m_startingRadius;
    // The center longitude in radian when the left mouse button has been pressed.
    qreal m_leftPressedLon;
    // The center latitude in radian when the left mouse button has been pressed.
    qreal m_leftPressedLat;

    int m_dragThreshold;
    QTimer m_lmbTimer;
    QTimer m_pressAndHoldTimer;

    // Models to handle the kinetic spinning.
    KineticModel m_kineticSpinning;

    QPoint m_selectionOrigin;

    QPointer<AbstractDataPluginItem> m_lastToolTipItem;
    QTimer m_toolTipTimer;
    QPoint m_toolTipPosition;
};

MarbleDefaultInputHandler::Private::Private()
    : m_leftPressed(false),
      m_midPressed(false),
      m_dragThreshold(MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 15 : 3)
{
    m_curpmtl.load(QStringLiteral(":/marble/cursor/tl.png"));
    m_curpmtc.load(QStringLiteral(":/marble/cursor/tc.png"));
    m_curpmtr.load(QStringLiteral(":/marble/cursor/tr.png"));
    m_curpmcr.load(QStringLiteral(":/marble/cursor/cr.png"));
    m_curpmcl.load(QStringLiteral(":/marble/cursor/cl.png"));
    m_curpmbl.load(QStringLiteral(":/marble/cursor/bl.png"));
    m_curpmbc.load(QStringLiteral(":/marble/cursor/bc.png"));
    m_curpmbr.load(QStringLiteral(":/marble/cursor/br.png"));

    m_arrowCur[0][0] = QCursor( m_curpmtl, 2, 2 );
    m_arrowCur[1][0] = QCursor( m_curpmtc, 10, 3 );
    m_arrowCur[2][0] = QCursor( m_curpmtr, 19, 2 );
    m_arrowCur[0][1] = QCursor( m_curpmcl, 3, 10 );
    m_arrowCur[1][1] = QCursor( Qt::OpenHandCursor );
    m_arrowCur[2][1] = QCursor( m_curpmcr, 18, 10 );
    m_arrowCur[0][2] = QCursor( m_curpmbl, 2, 19 );
    m_arrowCur[1][2] = QCursor( m_curpmbc, 11, 18 );
    m_arrowCur[2][2] = QCursor( m_curpmbr, 19, 19 );
}

MarbleDefaultInputHandler::Private::~Private()
{
}

MarbleDefaultInputHandler::MarbleDefaultInputHandler(MarbleAbstractPresenter *marblePresenter)
    : MarbleInputHandler(marblePresenter),
      d(new Private())
{
    d->m_toolTipTimer.setSingleShot(true);
    d->m_toolTipTimer.setInterval(TOOLTIP_START_INTERVAL);
    connect(&d->m_toolTipTimer, SIGNAL(timeout()), this, SLOT(openItemToolTip()));
    d->m_lmbTimer.setSingleShot(true);
    connect(&d->m_lmbTimer, SIGNAL(timeout()), this, SLOT(lmbTimeout()));

    d->m_kineticSpinning.setUpdateInterval(35);
    connect(&d->m_kineticSpinning, SIGNAL(positionChanged(qreal,qreal)),
             MarbleInputHandler::d->m_marblePresenter, SLOT(centerOn(qreal,qreal)));
    connect(&d->m_kineticSpinning, SIGNAL(finished()), SLOT(restoreViewContext()));

    // Left and right mouse button signals.
    connect(this, SIGNAL(rmbRequest(int,int)), this, SLOT(showRmbMenu(int,int)));
    connect(this, SIGNAL(lmbRequest(int,int)), this, SLOT(showLmbMenu(int,int)));

    d->m_pressAndHoldTimer.setInterval(800);
    d->m_pressAndHoldTimer.setSingleShot(true);
    connect(&d->m_pressAndHoldTimer, SIGNAL(timeout()), this, SLOT(handlePressAndHold()));
}

MarbleDefaultInputHandler::~MarbleDefaultInputHandler()
{
    delete d;
}

void MarbleDefaultInputHandler::lmbTimeout()
{
    if (!selectionRubber()->isVisible())
    {
        qreal clickedLon = 0;
        qreal clickedLat = 0;

        bool isPointOnGlobe = MarbleInputHandler::d->m_marblePresenter->map()->geoCoordinates( d->m_leftPressedX, d->m_leftPressedY,
                                                                        clickedLon, clickedLat,
                                                                        GeoDataCoordinates::Degree );
        emit lmbRequest(d->m_leftPressedX, d->m_leftPressedY);

        /**
         * emit mouse click only when the clicked
         * position is within the globe.
         */
        if ( isPointOnGlobe ) {
            emit mouseClickGeoPosition( clickedLon, clickedLat,
                                        GeoDataCoordinates::Degree );
        }
    }
}

void MarbleInputHandler::restoreViewContext()
{
    // Needs to stop the timer since it repeats otherwise.
    d->m_mouseWheelTimer->stop();

    // Redraw the map with the quality set for Still (if necessary).
    d->m_marblePresenter->setViewContext(Still);
    d->m_marblePresenter->map()->viewport()->resetFocusPoint();
    d->m_wheelZoomTargetDistance = 0.0;
}

void MarbleDefaultInputHandler::hideSelectionIfCtrlReleased(QEvent *e)
{
    if (selectionRubber()->isVisible() && e->type() == QEvent::MouseMove)
    {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        if (!(event->modifiers() & Qt::ControlModifier))
        {
            selectionRubber()->hide();
        }
    }
}

bool MarbleDefaultInputHandler::handleDoubleClick(QMouseEvent *event)
{
    qreal mouseLon;
    qreal mouseLat;
    const bool isMouseAboveMap = MarbleInputHandler::d->m_marblePresenter->map()->geoCoordinates(event->x(), event->y(),
                                             mouseLon, mouseLat, GeoDataCoordinates::Radian);
    if(isMouseAboveMap)
    {
        d->m_pressAndHoldTimer.stop();
        d->m_lmbTimer.stop();
        MarbleInputHandler::d->m_marblePresenter->moveTo(event->pos(), 0.67);
    }
    return acceptMouse();
}

bool MarbleDefaultInputHandler::handleWheel(QWheelEvent *wheelevt)
{
    MarbleAbstractPresenter *marblePresenter = MarbleInputHandler::d->m_marblePresenter;
    marblePresenter->setViewContext(Animation);

    if( (MarbleInputHandler::d->m_steps > 0 && wheelevt->delta() < 0) ||
        (MarbleInputHandler::d->m_steps < 0 && wheelevt->delta() > 0) )
    {
        MarbleInputHandler::d->m_steps = wheelevt->delta();
    }
    else
    {
        MarbleInputHandler::d->m_steps += wheelevt->delta();
    }

    if (marblePresenter->map()->discreteZoom())
    {
        if(qAbs(MarbleInputHandler::d->m_steps) >= MarbleInputHandler::d->m_discreteZoomSteps)
        {
            marblePresenter->zoomAtBy(wheelevt->pos(), MarbleInputHandler::d->m_steps);
            MarbleInputHandler::d->m_steps = 0;
        }
    }
    else
    {
        qreal zoom = marblePresenter->zoom();
        qreal target = MarbleInputHandler::d->m_wheelZoomTargetDistance;
        if (marblePresenter->animationsEnabled() && target > 0.0)
        {
            // Do not use intermediate (interpolated) distance values caused by animations
            zoom = marblePresenter->zoomFromDistance(target);
        }
        qreal newDistance = marblePresenter->distanceFromZoom(zoom + MarbleInputHandler::d->m_steps);
        MarbleInputHandler::d->m_wheelZoomTargetDistance = newDistance;
        marblePresenter->zoomAt(wheelevt->pos(), newDistance);
        if (MarbleInputHandler::d->m_inertialEarthRotation)
        {
            d->m_kineticSpinning.jumpToPosition(MarbleInputHandler::d->m_marblePresenter->centerLongitude(),
                                                MarbleInputHandler::d->m_marblePresenter->centerLatitude());
        }
        MarbleInputHandler::d->m_steps = 0;
    }

    MarbleInputHandler::d->m_mouseWheelTimer->start(400);
    return true;
}

bool MarbleDefaultInputHandler::handlePinch(const QPointF &center, qreal scaleFactor, Qt::GestureState state)
{
    qreal  destLat;
    qreal  destLon;
    MarbleAbstractPresenter *marblePresenter = MarbleInputHandler::d->m_marblePresenter;

    bool isValid = marblePresenter->map()->geoCoordinates(center.x(), center.y(),
                 destLon, destLat, GeoDataCoordinates::Radian );

    if (isValid)
    {
        marblePresenter->map()->viewport()->setFocusPoint(GeoDataCoordinates(destLon, destLat));
    }

    switch (state)
    {
    case Qt::NoGesture:
        break;
    case Qt::GestureStarted:
        marblePresenter->setViewContext(Animation);
        d->m_pressAndHoldTimer.stop();
        d->m_lmbTimer.stop();
        d->m_midPressed = false;
        d->m_leftPressed = false;
        d->m_startingRadius = marblePresenter->radius();
        break;
    case Qt::GestureUpdated:
        marblePresenter->setRadius(marblePresenter->radius() * scaleFactor);
        break;
    case Qt::GestureFinished:
        marblePresenter->map()->viewport()->resetFocusPoint();
        marblePresenter->setViewContext(Still);
        break;
    case Qt::GestureCanceled:
        marblePresenter->setRadius(d->m_startingRadius);
        marblePresenter->map()->viewport()->resetFocusPoint();
        marblePresenter->setViewContext(Still);
        break;
    }
    return true;
}

bool MarbleDefaultInputHandler::handleGesture(QGestureEvent *ge)
{
    QPinchGesture *pinch = static_cast<QPinchGesture*>(ge->gesture(Qt::PinchGesture));
    if (!pinch)
    {
        return false;
    }

    qreal scaleFactor = pinch->scaleFactor();
    QPointF center = pinch->centerPoint();

    return handlePinch(center, scaleFactor, pinch->state());
}

void MarbleDefaultInputHandler::checkReleasedMove(QMouseEvent *event)
{
    // To prevent error from lost MouseButtonRelease events
    if (event->type() == QEvent::MouseMove && !(event->buttons() & Qt::LeftButton))
    {
        if (d->m_leftPressed)
        {
            d->m_leftPressed = false;

            if (MarbleInputHandler::d->m_inertialEarthRotation)
            {
                d->m_kineticSpinning.start();
            }
            else
            {
                MarbleInputHandler::d->m_marblePresenter->setViewContext(Still);
            }
        }
    }
    if (event->type() == QEvent::MouseMove && !(event->buttons() & Qt::MidButton))
    {
        d->m_midPressed = false;
    }
}

void MarbleDefaultInputHandler::handleMouseButtonPress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton )
    {
       handleLeftMouseButtonPress(event);
    }

    if ( event->button() == Qt::MidButton )
    {
       handleMiddleMouseButtonPress(event);
    }

    if ( event->button() == Qt::RightButton )
    {
       handleRightMouseButtonPress(event);
    }
}

void MarbleDefaultInputHandler::handleLeftMouseButtonPress(QMouseEvent *event)
{
    if (isMouseButtonPopupEnabled(Qt::LeftButton))
    {
        d->m_lmbTimer.start(400);
    }

    d->m_leftPressed = true;
    d->m_midPressed = false;
    selectionRubber()->hide();

    // On the single event of a mouse button press these
    // values get stored, to enable us to e.g. calculate the
    // distance of a mouse drag while the mouse button is
    // still down.
    d->m_leftPressedX = event->x();
    d->m_leftPressedY = event->y();

    // Calculate translation of center point
    d->m_leftPressedLon = MarbleInputHandler::d->m_marblePresenter->centerLongitude();
    d->m_leftPressedLat = MarbleInputHandler::d->m_marblePresenter->centerLatitude();

    d->m_leftPressedDirection = 1;

    if (MarbleInputHandler::d->m_inertialEarthRotation)
    {
        d->m_kineticSpinning.stop();
        d->m_kineticSpinning.setPosition(d->m_leftPressedLon, d->m_leftPressedLat);
    }

    // Choose spin direction by taking into account whether we
    // drag above or below the visible pole.
    if (MarbleInputHandler::d->m_marblePresenter->map()->projection() == Spherical)
    {
        if (d->m_leftPressedLat >= 0)
        {   // The visible pole is the north pole
            qreal northPoleX, northPoleY;
            MarbleInputHandler::d->m_marblePresenter->map()->screenCoordinates(0.0, 90.0, northPoleX, northPoleY);
            if (event->y() < northPoleY)
            {
                d->m_leftPressedDirection = -1;
            }
        }
        else
        {   // The visible pole is the south pole
            qreal southPoleX, southPoleY;
            MarbleInputHandler::d->m_marblePresenter->map()->screenCoordinates(0.0, -90.0, southPoleX, southPoleY);
            if (event->y() > southPoleY)
            {
                d->m_leftPressedDirection = -1;
            }
        }
    }

    if (event->modifiers() & Qt::ControlModifier)
    {
        mDebug() << Q_FUNC_INFO << "Starting selection";
        d->m_pressAndHoldTimer.stop();
        d->m_lmbTimer.stop();
        d->m_selectionOrigin = event->pos();
        selectionRubber()->setGeometry(QRect(d->m_selectionOrigin, QSize()));
        selectionRubber()->show();
    }
}

void MarbleDefaultInputHandler::handleMiddleMouseButtonPress(QMouseEvent *event)
{
    d->m_midPressed = true;
    d->m_leftPressed = false;
    d->m_startingRadius = MarbleInputHandler::d->m_marblePresenter->radius();
    d->m_midPressedY = event->y();

    if (MarbleInputHandler::d->m_inertialEarthRotation)
    {
        d->m_kineticSpinning.start();
    }

    selectionRubber()->hide();
    MarbleInputHandler::d->m_marblePresenter->setViewContext(Animation);
}

void MarbleDefaultInputHandler::handleRightMouseButtonPress(QMouseEvent *event)
{
    emit rmbRequest(event->x(), event->y());
}

void MarbleDefaultInputHandler::handleMouseButtonRelease(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        //emit current coordinates to be be interpreted
        //as requested
        emit mouseClickScreenPosition(d->m_leftPressedX, d->m_leftPressedY);

        d->m_leftPressed = false;
        if (MarbleInputHandler::d->m_inertialEarthRotation)
        {
            d->m_kineticSpinning.start();
        }
        else
        {
            MarbleInputHandler::d->m_marblePresenter->setViewContext(Still);
        }
    }

    if (event->button() == Qt::MidButton)
    {
        d->m_midPressed = false;

        MarbleInputHandler::d->m_marblePresenter->setViewContext(Still);
    }

    if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton)
    {
    }

    if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::LeftButton
         && selectionRubber()->isVisible())
    {
        mDebug() << Q_FUNC_INFO << "Leaving selection";
        MarbleInputHandler::d->m_marblePresenter->setSelection(selectionRubber()->geometry());
        selectionRubber()->hide();
    }
}

void MarbleDefaultInputHandler::notifyPosition(bool isMouseAboveMap, qreal mouseLon, qreal mouseLat)
{
    // emit the position string only if the signal got attached
    if (MarbleInputHandler::d->m_positionSignalConnected) {
        if (!isMouseAboveMap)
        {
            emit mouseMoveGeoPosition(QCoreApplication::translate( "Marble", NOT_AVAILABLE));
        }
        else
        {
            QString position = GeoDataCoordinates(mouseLon, mouseLat).toString();
            emit mouseMoveGeoPosition(position);
        }
    }
}

void MarbleDefaultInputHandler::adjustCursorShape(const QPoint &mousePosition, const QPoint &mouseDirection)
{
    // Find out if there are data items and if one has defined an action
    QList<AbstractDataPluginItem *> dataItems
        = MarbleInputHandler::d->m_marblePresenter->map()->whichItemAt(mousePosition);
    bool dataAction = false;
    QPointer<AbstractDataPluginItem> toolTipItem;
    QList<AbstractDataPluginItem *>::iterator it = dataItems.begin();
    QList<AbstractDataPluginItem *>::iterator const end = dataItems.end();
    for (; it != end && dataAction == false && toolTipItem.isNull(); ++it)
    {
        if ((*it)->action())
        {
            dataAction = true;
        }

        if (!(*it)->toolTip().isNull() && toolTipItem.isNull())
        {
            toolTipItem = (*it);
        }
    }

    if (toolTipItem.isNull()) {
        d->m_toolTipTimer.stop();
    }
    else if (!( d->m_lastToolTipItem.data() == toolTipItem.data()))
    {
        d->m_toolTipTimer.start();
        d->m_lastToolTipItem = toolTipItem;
        d->m_toolTipPosition = mousePosition;
    }
    else
    {
        if (!d->m_toolTipTimer.isActive())
        {
            d->m_toolTipTimer.start();
        }
        d->m_toolTipPosition = mousePosition;
    }

    if ((MarbleInputHandler::d->m_marblePresenter->map()->whichFeatureAt(mousePosition).size() == 0)
         && (!dataAction ))
    {
        if (!d->m_leftPressed)
        {
            d->m_arrowCur [1][1] = QCursor(Qt::OpenHandCursor);
        }
        else
        {
            d->m_arrowCur [1][1] = QCursor(Qt::ClosedHandCursor);
        }
    }
    else
    {
        if (!d->m_leftPressed)
        {
            d->m_arrowCur [1][1] = QCursor(Qt::PointingHandCursor);
        }
    }

    if (panViaArrowsEnabled())
    {
        setCursor(d->m_arrowCur[mouseDirection.x()+1][mouseDirection.y()+1]);
    }
    else
    {
        setCursor(d->m_arrowCur[1][1]);
    }
}

QPoint MarbleDefaultInputHandler::mouseMovedOutside(QMouseEvent *event)
{   //Returns a 2d vector representing the direction in which the mouse left
    int dirX = 0;
    int dirY = 0;
    int polarity = MarbleInputHandler::d->m_marblePresenter->viewport()->polarity();

    if (d->m_leftPressed) {
        d->m_leftPressed = false;

        if (MarbleInputHandler::d->m_inertialEarthRotation)
        {
            d->m_kineticSpinning.start();
        }
    }

    QRect boundingRect = MarbleInputHandler::d->m_marblePresenter->viewport()->mapRegion().boundingRect();

    if (boundingRect.width() != 0)
    {
        dirX = (int)( 3 * (event->x() - boundingRect.left()) / boundingRect.width()) - 1;
    }
    if (dirX > 1)
    {
        dirX = 1;
    }
    if (dirX < -1)
    {
        dirX = -1;
    }

    if (boundingRect.height() != 0)
    {
        dirY = (int)(3 * (event->y() - boundingRect.top()) / boundingRect.height()) - 1;
    }
    if (dirY > 1)
    {
        dirY = 1;
    }
    if (dirY < -1)
    {
        dirY = -1;
    }

    if (event->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress
            && panViaArrowsEnabled() && !d->m_kineticSpinning.hasVelocity())
    {
        d->m_pressAndHoldTimer.stop();
        d->m_lmbTimer.stop();
        qreal moveStep = MarbleInputHandler::d->m_marblePresenter->moveStep();
        if (polarity < 0)
        {
            MarbleInputHandler::d->m_marblePresenter->rotateBy(-moveStep * (qreal)(+dirX), moveStep * (qreal)(+dirY));
        }
        else
        {
            MarbleInputHandler::d->m_marblePresenter->rotateBy(-moveStep * (qreal)(-dirX), moveStep * (qreal)(+dirY));
        }
    }

    if (!MarbleInputHandler::d->m_inertialEarthRotation)
    {
        MarbleInputHandler::d->m_marblePresenter->setViewContext(Still);
    }

    return QPoint(dirX, dirY);
}

bool MarbleDefaultInputHandler::handleMouseEvent(QMouseEvent *event)
{
    QPoint direction;

    checkReleasedMove(event);

    // Do not handle (and therefore eat) mouse press and release events
    // that occur above visible float items. Mouse motion events are still
    // handled, however.
    if (event->type() != QEvent::MouseMove && !selectionRubber()->isVisible())
    {
		auto const floatItems = MarbleInputHandler::d->m_marblePresenter->map()->floatItems();
        foreach (AbstractFloatItem *floatItem, floatItems)
        {
            if ( floatItem->enabled() && floatItem->visible()
                 && floatItem->contains( event->pos() ) )
            {
                d->m_pressAndHoldTimer.stop();
                d->m_lmbTimer.stop();
                return false;
            }
        }
    }

    qreal mouseLon;
    qreal mouseLat;
    const bool isMouseAboveMap = MarbleInputHandler::d->m_marblePresenter->map()->geoCoordinates(event->x(), event->y(),
                                             mouseLon, mouseLat, GeoDataCoordinates::Radian);
    notifyPosition(isMouseAboveMap, mouseLon, mouseLat);

    QPoint mousePosition(event->x(), event->y());

    if (isMouseAboveMap || selectionRubber()->isVisible()
         || MarbleInputHandler::d->m_marblePresenter->map()->whichFeatureAt( mousePosition ).size() != 0)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            d->m_pressAndHoldTimer.start();
            handleMouseButtonPress(event);
        }

        if (event->type() == QEvent::MouseButtonRelease)
        {
            d->m_pressAndHoldTimer.stop();
            handleMouseButtonRelease(event);
        }

        // Regarding all kinds of mouse moves:
        if (d->m_leftPressed && !selectionRubber()->isVisible())
        {
            qreal radius = (qreal)(MarbleInputHandler::d->m_marblePresenter->radius());
            int deltax = event->x() - d->m_leftPressedX;
            int deltay = event->y() - d->m_leftPressedY;

            if (abs(deltax) > d->m_dragThreshold
                 || abs(deltay) > d->m_dragThreshold
                 || !d->m_lmbTimer.isActive())
            {
                MarbleInputHandler::d->m_marblePresenter->setViewContext(Animation);

                d->m_pressAndHoldTimer.stop();
                d->m_lmbTimer.stop();

                const qreal posLon = d->m_leftPressedLon - 90.0 * d->m_leftPressedDirection * deltax / radius;
                const qreal posLat = d->m_leftPressedLat + 90.0 * deltay / radius;
                MarbleInputHandler::d->m_marblePresenter->centerOn(posLon, posLat);
                if (MarbleInputHandler::d->m_inertialEarthRotation)
                {
                    d->m_kineticSpinning.setPosition(posLon, posLat);
                }
            }
        }

        if (d->m_midPressed)
        {
            int eventy = event->y();
            int dy = d->m_midPressedY - eventy;
            MarbleInputHandler::d->m_marblePresenter->setRadius(d->m_startingRadius * pow(1.005, dy));
        }

        if (selectionRubber()->isVisible())
        {
            // We change selection.
            selectionRubber()->setGeometry(QRect(d->m_selectionOrigin, event->pos()).normalized());
        }
    }
    else
    {
        direction = mouseMovedOutside(event);
    }

    if (MarbleInputHandler::d->m_marblePresenter->viewContext() != Animation) {
        adjustCursorShape(mousePosition, direction);
    }
    return acceptMouse();
}

bool MarbleDefaultInputHandler::acceptMouse()
{
    // let others, especially float items, still process the event
    // Note: This caused a bug in combination with oxygen, see https://bugs.kde.org/show_bug.cgi?id=242414
    // and changing it a related regression, see https://bugs.kde.org/show_bug.cgi?id=324862
    return false;
}

bool MarbleDefaultInputHandler::eventFilter(QObject* o, QEvent* e)
{
    Q_UNUSED(o);

    if (layersEventFilter(o, e))
    {
        return true;
    }

    hideSelectionIfCtrlReleased(e);

    switch (e->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        return handleTouch(static_cast<QTouchEvent *>(e));
    case QEvent::KeyPress:
        return handleKeyPress(static_cast<QKeyEvent *>(e));
    case QEvent::Gesture:
        return handleGesture(static_cast<QGestureEvent *>(e));
    case QEvent::Wheel:
        return handleWheel(static_cast<QWheelEvent*>(e));
    case QEvent::MouseButtonDblClick:
        return handleDoubleClick(static_cast<QMouseEvent*>(e));
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return handleMouseEvent(static_cast<QMouseEvent*>(e));
    default:
        return false;
    }
}

bool MarbleDefaultInputHandler::handleTouch(QTouchEvent*)
{
    return false; //reimplement to handle in cases of QML and PinchArea element
}

bool MarbleDefaultInputHandler::handleKeyPress(QKeyEvent* event)
{
    if ( event->type() == QEvent::KeyPress ) {
        MarbleAbstractPresenter *marblePresenter = MarbleInputHandler::d->m_marblePresenter;
        bool handled = true;
        switch ( event->key() ) {
        case Qt::Key_Left:
            marblePresenter->moveByStep(-1, 0);
            break;
        case Qt::Key_Right:
            marblePresenter->moveByStep(1, 0);
            break;
        case Qt::Key_Up:
            marblePresenter->moveByStep(0, -1);
            break;
        case Qt::Key_Down:
            marblePresenter->moveByStep(0, 1);
            break;
        case Qt::Key_Plus:
            marblePresenter->zoomIn();
            break;
        case Qt::Key_Minus:
            marblePresenter->zoomOut();
            break;
        case Qt::Key_Home:
            marblePresenter->goHome();
            break;
        default:
            handled = false;
            break;
        }

        return handled;
    }
    return false;
}

void MarbleDefaultInputHandler::handleMouseButtonPressAndHold(const QPoint &)
{
    // Default implementation does nothing
}

void MarbleDefaultInputHandler::handlePressAndHold()
{
    handleMouseButtonPressAndHold(QPoint(d->m_leftPressedX, d->m_leftPressedY));
}

QPointer<AbstractDataPluginItem> MarbleDefaultInputHandler::lastToolTipItem()
{
    return d->m_lastToolTipItem;
}

QTimer* MarbleDefaultInputHandler::toolTipTimer()
{
    return &d->m_toolTipTimer;
}

QPoint MarbleDefaultInputHandler::toolTipPosition()
{
    return d->m_toolTipPosition;
}

}

#include "moc_MarbleInputHandler.cpp"

