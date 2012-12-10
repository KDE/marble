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

#include "MarbleWidgetInputHandler.h"

#include <cmath>

#include <QtCore/QPoint>
#include <QtCore/QPointer>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QRubberBand>
#include <QtGui/QToolTip>

#if QT_VERSION >= 0x40600
  #include <QtGui/QGestureEvent>
  #include <QtGui/QPinchGesture>
#endif

#include "kineticmodel.h"

#include "MarbleGlobal.h"
#include "MarbleDebug.h"
#include "GeoDataCoordinates.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "AbstractFloatItem.h"
#include "AbstractDataPluginItem.h"
#include "MarbleWidgetPopupMenu.h"
#include "Planet.h"
#include "RenderPlugin.h"

namespace Marble {

const int TOOLTIP_START_INTERVAL = 1000;

class MarbleWidgetInputHandler::Protected
{
public:
    Protected( MarbleWidget *widget );

    MarbleWidget *const m_widget;
    MarbleModel *const m_model;
    bool m_positionSignalConnected;
    QTimer *m_mouseWheelTimer;
    Qt::MouseButtons m_disabledMouseButtons;
    qreal m_wheelZoomTargetDistance;
    bool m_panViaArrowsEnabled;
    bool m_kineticScrollingEnabled;
};

MarbleWidgetInputHandler::Protected::Protected( MarbleWidget *widget )
    : m_widget( widget ),
      m_model( widget->model() ),
      m_positionSignalConnected( false ),
      m_mouseWheelTimer( 0 ),
      m_disabledMouseButtons( Qt::NoButton ),
      m_wheelZoomTargetDistance( 0.0 ),
      m_panViaArrowsEnabled( true ),
      m_kineticScrollingEnabled( true )
{
}


MarbleWidgetInputHandler::MarbleWidgetInputHandler( MarbleWidget *widget )
    : d( new Protected( widget ) )
{
    d->m_mouseWheelTimer = new QTimer( this );
    connect( d->m_mouseWheelTimer, SIGNAL( timeout() ),
	     this, SLOT( restoreViewContext() ) );
    
    connect( d->m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
             this,        SLOT( installPluginEventFilter( RenderPlugin * ) ) );

    foreach( RenderPlugin *renderPlugin, d->m_widget->renderPlugins() ) {
        if( renderPlugin->isInitialized() )
            d->m_widget->installEventFilter( renderPlugin );
    }
}

MarbleWidgetInputHandler::~MarbleWidgetInputHandler()
{
    delete d->m_mouseWheelTimer;
    delete d;
}

void MarbleWidgetInputHandler::setPositionSignalConnected( bool connected )
{
    d->m_positionSignalConnected = connected;
}

bool MarbleWidgetInputHandler::isPositionSignalConnected() const
{
    return d->m_positionSignalConnected;
}

void MarbleWidgetInputHandler::setMouseButtonPopupEnabled( Qt::MouseButton mouseButton, bool enabled )
{
    if ( enabled ) {
        d->m_disabledMouseButtons &= ~Qt::MouseButtons( mouseButton );
    }
    else {
        d->m_disabledMouseButtons |= mouseButton;
    }
}

bool MarbleWidgetInputHandler::isMouseButtonPopupEnabled( Qt::MouseButton mouseButton ) const
{
    return !( d->m_disabledMouseButtons & mouseButton );
}

void MarbleWidgetInputHandler::setPanViaArrowsEnabled( bool enabled )
{
    d->m_panViaArrowsEnabled = enabled;
}

bool MarbleWidgetInputHandler::panViaArrowsEnabled() const
{
    return d->m_panViaArrowsEnabled;
}

void MarbleWidgetInputHandler::setKineticScrollingEnabled( bool enabled )
{
    d->m_kineticScrollingEnabled = enabled;
}

bool MarbleWidgetInputHandler::kineticScrollingEnabled() const
{
    return d->m_kineticScrollingEnabled;
}

class MarbleWidgetDefaultInputHandler::Private
{
 public:
    Private( MarbleWidget *widget );
    ~Private();

    /**
      * @brief Change to a different distance, keeping the given point still
      * @param widget The marble widget to work on
      * @param pos A screen position. The underlying geopoint should stay at the
      * same position while zooming
      * @param distance New camera distance to the ground
      */
    void ZoomAt(MarbleWidget* widget, const QPoint &pos, qreal distance);

    /**
      * @brief Change zoom value by the given factor, making the given point the new center
      * @param widget The marble widget to work on
      * @param pos A screen position. The underlying geopoint will become the new center point
      * @param factor Zoom factor, 1.0 means no change
      */
    void MoveTo(MarbleWidget* marbleWidget, const QPoint &pos, qreal zoomFactor);

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

    // Models to handle the kinetic spinning.
    KineticModel m_kineticSpinning;

    QPoint m_selectionOrigin;
    QRubberBand m_selectionRubber;

    QPointer<AbstractDataPluginItem> m_lastToolTipItem;
    QTimer m_toolTipTimer;
    QPoint m_toolTipPosition;
};

MarbleWidgetDefaultInputHandler::Private::Private( MarbleWidget *widget )
    : m_leftPressed( false ),
      m_midPressed( false ),
      m_dragThreshold( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 15 : 3 ),
      m_selectionRubber( QRubberBand::Rectangle, widget )
{
    m_curpmtl.load( ":/marble/cursor/tl.png");
    m_curpmtc.load( ":/marble/cursor/tc.png");
    m_curpmtr.load( ":/marble/cursor/tr.png");
    m_curpmcr.load( ":/marble/cursor/cr.png");
    m_curpmcl.load( ":/marble/cursor/cl.png");
    m_curpmbl.load( ":/marble/cursor/bl.png");
    m_curpmbc.load( ":/marble/cursor/bc.png");
    m_curpmbr.load( ":/marble/cursor/br.png");

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

MarbleWidgetDefaultInputHandler::Private::~Private()
{
}

void MarbleWidgetDefaultInputHandler::Private::ZoomAt(MarbleWidget* marbleWidget, const QPoint &pos, qreal newDistance)
{
    Q_ASSERT(newDistance > 0.0);

    qreal  destLat;
    qreal  destLon;
    if (!marbleWidget->geoCoordinates(pos.x(), pos.y(), 
          destLon, destLat, GeoDataCoordinates::Degree )) {
        return;
    }

    ViewportParams* now = marbleWidget->viewport();

    qreal x(0), y(0);
    if (!now->screenCoordinates(destLon * DEG2RAD, destLat * DEG2RAD, x, y)) {
        return;
    }

    ViewportParams soon;
    soon.setProjection(now->projection());
    soon.centerOn(now->centerLongitude(), now->centerLatitude());
    soon.setSize(now->size());

    qreal newRadius = marbleWidget->radiusFromDistance(newDistance);
    soon.setRadius( newRadius );

    qreal mouseLon, mouseLat;
    if (!soon.geoCoordinates(int(x), int(y), mouseLon, mouseLat, GeoDataCoordinates::Degree )) {
        return;
    }

    const qreal lon = destLon - ( mouseLon - marbleWidget->centerLongitude() );
    const qreal lat = destLat - ( mouseLat - marbleWidget->centerLatitude() );

    GeoDataLookAt lookAt;
    lookAt.setLongitude( lon, GeoDataCoordinates::Degree );
    lookAt.setLatitude( lat, GeoDataCoordinates::Degree );
    lookAt.setAltitude( 0.0 );
    lookAt.setRange( newDistance * KM2METER );

    marbleWidget->setFocusPoint( GeoDataCoordinates( destLon, destLat, 0, GeoDataCoordinates::Degree ) );
    marbleWidget->flyTo( lookAt, Linear );
}

void MarbleWidgetDefaultInputHandler::Private::MoveTo( MarbleWidget* marbleWidget,
                                                       const QPoint &pos,
                                                       qreal factor )
{
    Q_ASSERT( factor > 0.0 );

    qreal  destLat;
    qreal  destLon;
    qreal distance = marbleWidget->distance();
    marbleWidget->geoCoordinates( pos.x(), pos.y(), destLon, destLat, GeoDataCoordinates::Radian );
    
    GeoDataLookAt lookAt;
    lookAt.setLongitude( destLon );
    lookAt.setLatitude( destLat );
    lookAt.setAltitude( 0.0 );
    lookAt.setRange( distance * factor * KM2METER );
    
    marbleWidget->flyTo( lookAt );
}

MarbleWidgetDefaultInputHandler::MarbleWidgetDefaultInputHandler( MarbleWidget *widget )
    : MarbleWidgetInputHandler( widget ),
      d( new Private( widget ) )
{
#if QT_VERSION >= 0x40600
    widget->grabGesture( Qt::PinchGesture );
#endif

    d->m_selectionRubber.hide();

    d->m_toolTipTimer.setSingleShot( true );
    d->m_toolTipTimer.setInterval( TOOLTIP_START_INTERVAL );
    connect( &d->m_toolTipTimer, SIGNAL( timeout() ), this, SLOT( openItemToolTip() ) );
    d->m_lmbTimer.setSingleShot(true);
    connect( &d->m_lmbTimer, SIGNAL(timeout()), this, SLOT(lmbTimeout()));

    d->m_kineticSpinning.setUpdateInterval( 35 );
    connect( &d->m_kineticSpinning, SIGNAL( positionChanged( qreal, qreal ) ),
             MarbleWidgetInputHandler::d->m_widget, SLOT( centerOn( qreal, qreal ) ) );
    connect( &d->m_kineticSpinning, SIGNAL( finished() ), SLOT( restoreViewContext() ) );


    // Left and right mouse button signals.
    connect( this, SIGNAL( rmbRequest( int, int ) ),
             this, SLOT( showRmbMenu( int, int ) ) );
    connect( this, SIGNAL( lmbRequest( int, int ) ),
             this, SLOT( showLmbMenu( int, int ) ) );
}

MarbleWidgetDefaultInputHandler::~MarbleWidgetDefaultInputHandler()
{
    delete d;
}

void MarbleWidgetDefaultInputHandler::lmbTimeout()
{
    if (!d->m_selectionRubber.isVisible()) {
        emit lmbRequest( d->m_leftPressedX, d->m_leftPressedY );
    }
}

void MarbleWidgetInputHandler::restoreViewContext()
{
    // Needs to stop the timer since it repeats otherwise.
    d->m_mouseWheelTimer->stop();

    // Redraw the map with the quality set for Still (if necessary).
    d->m_widget->setViewContext( Still );
    d->m_widget->resetFocusPoint();
    d->m_wheelZoomTargetDistance = 0.0;
}

void MarbleWidgetInputHandler::installPluginEventFilter( RenderPlugin *renderPlugin )
{
    d->m_widget->installEventFilter( renderPlugin );
}

void MarbleWidgetDefaultInputHandler::showLmbMenu( int x, int y )
{
    if ( isMouseButtonPopupEnabled( Qt::LeftButton ) ) {
        MarbleWidgetInputHandler::d->m_widget->popupMenu()->showLmbMenu( x, y );
        d->m_toolTipTimer.stop();
    }
}

void MarbleWidgetDefaultInputHandler::showRmbMenu( int x, int y )
{
    if ( isMouseButtonPopupEnabled( Qt::RightButton ) ) {
        MarbleWidgetInputHandler::d->m_widget->popupMenu()->showRmbMenu( x, y );
    }
}

void MarbleWidgetDefaultInputHandler::openItemToolTip()
{
    if ( !d->m_lastToolTipItem.isNull() ) {
        QToolTip::showText( MarbleWidgetInputHandler::d->m_widget->mapToGlobal( d->m_toolTipPosition ),
                            d->m_lastToolTipItem->toolTip(),
                            MarbleWidgetInputHandler::d->m_widget,
                            d->m_lastToolTipItem->containsRect( d->m_toolTipPosition ).toRect() );
    }
}

bool MarbleWidgetDefaultInputHandler::eventFilter( QObject* o, QEvent* e )
{
    Q_UNUSED( o );

    if (d->m_selectionRubber.isVisible() && e->type() == QEvent::MouseMove)
    {
        QMouseEvent *event = static_cast<QMouseEvent*>( e );
        if (!(event->modifiers() & Qt::ControlModifier))
        {
            d->m_selectionRubber.hide();
        }
    }

    if (e->type() == QEvent::MouseButtonDblClick)
    {
        d->m_lmbTimer.stop();
        QMouseEvent *event = static_cast<QMouseEvent*>( e );
        d->MoveTo(MarbleWidgetInputHandler::d->m_widget, event->pos(), 0.67);
        MarbleWidgetInputHandler::d->m_mouseWheelTimer->start( 400 );
    }

    if ( keyEvent( MarbleWidgetInputHandler::d->m_widget, e ) ) {
        return true;
    }

    int polarity = MarbleWidgetInputHandler::d->m_widget->viewport()->polarity();

    if ( e->type() == QEvent::MouseMove
         || e->type() == QEvent::MouseButtonPress
         || e->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *event = static_cast<QMouseEvent*>( e );

        int dirX = 0;
        int dirY = 0;

        // To prevent error from lost MouseButtonRelease events
        if ( event->type() == QEvent::MouseMove
             && !( event->buttons() & Qt::LeftButton ) )
        {
            if ( d->m_leftPressed ) {
                d->m_leftPressed = false;

                if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                    d->m_kineticSpinning.start();
                } else {
                    MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );
                }
            }
        }
        if ( event->type() == QEvent::MouseMove
             && !( event->buttons() & Qt::MidButton ) )
        {
            d->m_midPressed = false;
        }

        // Do not handle (and therefore eat) mouse press and release events
        // that occur above visible float items. Mouse motion events are still
        // handled, however.
        if ( e->type() != QEvent::MouseMove
             && !d->m_selectionRubber.isVisible() )
        {
            foreach ( AbstractFloatItem *floatItem, MarbleWidgetInputHandler::d->m_widget->floatItems() ) {
                if ( floatItem->enabled() && floatItem->visible()
                     && floatItem->contains( event->pos() ) )
                {
                    d->m_lmbTimer.stop();
                    return false;
                }
            }
        }

        qreal mouseLon;
        qreal mouseLat;
        const bool isMouseAboveMap = MarbleWidgetInputHandler::d->m_widget->geoCoordinates( event->x(), event->y(),
                                                 mouseLon, mouseLat,
                                                 GeoDataCoordinates::Radian );

        // emit the position string only if the signal got attached
        if ( MarbleWidgetInputHandler::d->m_positionSignalConnected ) {
            if ( !isMouseAboveMap ) {
                emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
            }
            else {
                QString position = GeoDataCoordinates( mouseLon, mouseLat ).toString();
                emit mouseMoveGeoPosition( position );
            }
        }

        QPoint mousePosition( event->x(), event->y() );

        if ( isMouseAboveMap || d->m_selectionRubber.isVisible()
             || MarbleWidgetInputHandler::d->m_widget->whichFeatureAt( mousePosition ).size() != 0) {
            
            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton ) {

                if (isMouseButtonPopupEnabled(Qt::LeftButton))
                    d->m_lmbTimer.start(400);

                d->m_leftPressed = true;
                d->m_midPressed = false;
                d->m_selectionRubber.hide();

                // On the single event of a mouse button press these
                // values get stored, to enable us to e.g. calculate the
                // distance of a mouse drag while the mouse button is
                // still down.
                d->m_leftPressedX = event->x();
                d->m_leftPressedY = event->y();

                // Calculate translation of center point
                d->m_leftPressedLon = MarbleWidgetInputHandler::d->m_widget->centerLongitude();
                d->m_leftPressedLat = MarbleWidgetInputHandler::d->m_widget->centerLatitude();

                d->m_leftPressedDirection = 1;

                if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                    d->m_kineticSpinning.stop();
                }

                // Choose spin direction by taking into account whether we
                // drag above or below the visible pole.
                if ( MarbleWidgetInputHandler::d->m_widget->projection() == Spherical ) {
                    if ( d->m_leftPressedLat >= 0 ) {
                        // The visible pole is the north pole
                        qreal northPoleX, northPoleY;
                        MarbleWidgetInputHandler::d->m_widget->screenCoordinates( 0.0, 90.0, northPoleX, northPoleY );
                        if ( event->y() < northPoleY )
                            d->m_leftPressedDirection = -1;
                    }
                    else {
                        // The visible pole is the south pole
                        qreal southPoleX, southPoleY;
                        MarbleWidgetInputHandler::d->m_widget->screenCoordinates( 0.0, -90.0, southPoleX, southPoleY );
                        if ( event->y() > southPoleY )
                            d->m_leftPressedDirection = -1;
                    }
                }

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::MidButton ) {
                d->m_midPressed = true;
                d->m_leftPressed = false;
                d->m_startingRadius = MarbleWidgetInputHandler::d->m_widget->radius();
                d->m_midPressedY = event->y();

                if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                    d->m_kineticSpinning.start();
                }

                d->m_selectionRubber.hide();
                MarbleWidgetInputHandler::d->m_widget->setViewContext( Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::RightButton ) {
                emit rmbRequest( event->x(), event->y() );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton
                 && ( event->modifiers() & Qt::ControlModifier ) )
            {
                qDebug("Marble: Starting selection");
                d->m_lmbTimer.stop();                
                d->m_selectionOrigin = event->pos();
                d->m_selectionRubber.setGeometry( QRect( d->m_selectionOrigin, QSize() ));
                d->m_selectionRubber.show();
            }

            // Regarding mouse button releases:
            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::LeftButton )
            {

                //emit current coordinates to be be interpreted
                //as requested
                emit mouseClickScreenPosition( d->m_leftPressedX, d->m_leftPressedY );

                d->m_leftPressed = false;
                if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                    d->m_kineticSpinning.start();
                } else {
                    MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );
                }
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::MidButton ) {
                d->m_midPressed = false;

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::RightButton) {
            }

            if ( e->type() == QEvent::MouseButtonRelease
                && event->button() == Qt::LeftButton 
                 && d->m_selectionRubber.isVisible() )
            {
                qDebug("Marble: Leaving selection");
                MarbleWidgetInputHandler::d->m_widget->setSelection( d->m_selectionRubber.geometry() );
                d->m_selectionRubber.hide();
            }

            // Regarding all kinds of mouse moves:
            if ( d->m_leftPressed && !d->m_selectionRubber.isVisible() ) {
                qreal radius = ( qreal )( MarbleWidgetInputHandler::d->m_widget->radius() );
                int deltax = event->x() - d->m_leftPressedX;
                int deltay = event->y() - d->m_leftPressedY;

                if ( abs( deltax ) > d->m_dragThreshold
                     || abs( deltay ) > d->m_dragThreshold
                     || !d->m_lmbTimer.isActive() ) {

                    d->m_lmbTimer.stop();

                    const qreal posLon = d->m_leftPressedLon - 90.0 * d->m_leftPressedDirection * deltax / radius;
                    const qreal posLat = d->m_leftPressedLat + 90.0 * deltay / radius;
                    MarbleWidgetInputHandler::d->m_widget->centerOn( posLon, posLat );
                    if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                        d->m_kineticSpinning.setPosition( posLon, posLat );
                    }
                }
            }


            if ( d->m_midPressed ) {
                int eventy = event->y();
                int dy = d->m_midPressedY - eventy;
                MarbleWidgetInputHandler::d->m_widget->setRadius( d->m_startingRadius * pow( 1.005, dy ) );
            }

            if ( d->m_selectionRubber.isVisible() )
            {
                // We change selection.
                d->m_selectionRubber.setGeometry( QRect( d->m_selectionOrigin, event->pos() ).normalized() );
            }
        }
        else {
            d->m_leftPressed = false;

            if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                d->m_kineticSpinning.start();
            }

            QRect boundingRect = MarbleWidgetInputHandler::d->m_widget->mapRegion().boundingRect();

            if ( boundingRect.width() != 0 ) {
                dirX = (int)( 3 * ( event->x() - boundingRect.left() ) / boundingRect.width() ) - 1;
            }

            if ( dirX > 1 )
                dirX = 1;
            if ( dirX < -1 )
                dirX = -1;

            if ( boundingRect.height() != 0 ) {
                dirY = (int)( 3 * ( event->y() - boundingRect.top() ) / boundingRect.height() ) - 1;
            }

            if ( dirY > 1 )
                dirY = 1;
            if ( dirY < -1 )
                dirY = -1;

            if ( event->button() == Qt::LeftButton
                 && e->type() == QEvent::MouseButtonPress && panViaArrowsEnabled() ) {

                d->m_lmbTimer.stop();                
                if ( polarity < 0 )
                    MarbleWidgetInputHandler::d->m_widget->rotateBy( -MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirX),
                                                                     MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirY) );
                else
                    MarbleWidgetInputHandler::d->m_widget->rotateBy( -MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(-dirX),
                                                                     MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirY) );
            }

            if ( !MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );
            }
        }

        // Adjusting Cursor shape

        // Find out if there are data items and if one has defined an action
        QList<AbstractDataPluginItem *> dataItems
            = MarbleWidgetInputHandler::d->m_widget->whichItemAt( mousePosition );
        bool dataAction = false;
        QPointer<AbstractDataPluginItem> toolTipItem;
        QList<AbstractDataPluginItem *>::iterator it = dataItems.begin();
        QList<AbstractDataPluginItem *>::iterator const end = dataItems.end();
        for (; it != end && dataAction == false && toolTipItem.isNull(); ++it ) {
            if ( (*it)->action() ) {
                dataAction = true;
            }

            if ( !(*it)->toolTip().isNull() && toolTipItem.isNull() ) {
                toolTipItem = (*it);
            }
        }

        if ( toolTipItem.isNull() ) {
            d->m_toolTipTimer.stop();
        }
        else if ( !( d->m_lastToolTipItem.data() == toolTipItem.data() ) ) {
            d->m_toolTipTimer.start();
            d->m_lastToolTipItem = toolTipItem;
            d->m_toolTipPosition = mousePosition;
        }
        else {
            if ( !d->m_toolTipTimer.isActive() ) {
                d->m_toolTipTimer.start();
            }

            d->m_toolTipPosition = mousePosition;
        }

        if ( ( MarbleWidgetInputHandler::d->m_widget->whichFeatureAt( mousePosition ).size() == 0 )
             && ( !dataAction ) )
        {
            if ( !d->m_leftPressed )
                d->m_arrowCur [1][1] = QCursor( Qt::OpenHandCursor );
            else
                d->m_arrowCur [1][1] = QCursor( Qt::ClosedHandCursor );
        }
        else {
            if ( !d->m_leftPressed )
                d->m_arrowCur [1][1] = QCursor( Qt::PointingHandCursor );
        }

#ifndef Q_WS_MAEMO_5
        if ( panViaArrowsEnabled() ) {
            MarbleWidgetInputHandler::d->m_widget->setCursor( d->m_arrowCur[dirX+1][dirY+1] );
        } else {
            MarbleWidgetInputHandler::d->m_widget->setCursor( d->m_arrowCur [1][1] );
        }
#endif

        // let others, especially float items, still process the event
        // Mouse move events need to be eaten to avoid the default oxygen behavior of
        // moving the window around when we don't handle the event. See bug 242414.
        return event->type() != QEvent::MouseMove;
    }
    else {
        if ( e->type() == QEvent::Wheel ) {

            QWheelEvent *wheelevt = static_cast<QWheelEvent*>( e );

            MarbleWidget *marbleWidget = MarbleWidgetInputHandler::d->m_widget;
            marbleWidget->setViewContext( Animation );

            int steps = wheelevt->delta() / 3;
            qreal zoom = marbleWidget->zoom();
            qreal target = MarbleWidgetInputHandler::d->m_wheelZoomTargetDistance;
            if ( marbleWidget->animationsEnabled() && target > 0.0 ) {
                // Do not use intermediate (interpolated) distance values caused by animations
                zoom = marbleWidget->zoomFromDistance( target );
            }
            qreal newDistance = marbleWidget->distanceFromZoom( zoom + steps );
            MarbleWidgetInputHandler::d->m_wheelZoomTargetDistance = newDistance;
            d->ZoomAt(MarbleWidgetInputHandler::d->m_widget, wheelevt->pos(), newDistance);
            if ( MarbleWidgetInputHandler::d->m_kineticScrollingEnabled ) {
                d->m_kineticSpinning.jumpToPosition( MarbleWidgetInputHandler::d->m_widget->centerLongitude(),
                                                     MarbleWidgetInputHandler::d->m_widget->centerLatitude() );
            }

            MarbleWidgetInputHandler::d->m_mouseWheelTimer->start( 400 );
            return true;
        }
        else if ( e->type() == QEvent::Gesture ) {
            QGestureEvent *ge = static_cast<QGestureEvent *>(e);
            QPinchGesture *pinch = static_cast<QPinchGesture*>(ge->gesture(Qt::PinchGesture));
            if (pinch) {
                qreal scaleFactor = pinch->scaleFactor();
                qreal  destLat;
                qreal  destLon;
                QPointF center = pinch->centerPoint();

                MarbleWidget *marbleWidget = MarbleWidgetInputHandler::d->m_widget;

                bool isValid = marbleWidget->geoCoordinates(center.x(), center.y(),
                             destLon, destLat, GeoDataCoordinates::Radian );

                if (isValid) {
                    marbleWidget->setFocusPoint(GeoDataCoordinates(destLon, destLat));
                }

                switch ( pinch->state() ) {
                case Qt::NoGesture:
                    break;
                case Qt::GestureStarted:
                    marbleWidget->setViewContext( Animation );
                    d->m_midPressed = false;
                    d->m_leftPressed = false;
                    d->m_startingRadius = marbleWidget->radius();
                    break;
                case Qt::GestureUpdated:
                    marbleWidget->setRadius( marbleWidget->radius() * scaleFactor );
                    break;
                case Qt::GestureFinished:
                    marbleWidget->resetFocusPoint();
                    marbleWidget->setViewContext( Still );
                    break;
                case Qt::GestureCanceled:
                    marbleWidget->setRadius( d->m_startingRadius );
                    marbleWidget->resetFocusPoint();
                    marbleWidget->setViewContext( Still );
                    break;
                }

                return true;
            }
        }
        else
            return false;
    }
    return QObject::eventFilter( o, e );
}

bool MarbleWidgetDefaultInputHandler::keyEvent( MarbleWidget * widget, QEvent* e )
{
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent const * const k = dynamic_cast<QKeyEvent const * const>( e );
        Q_ASSERT( k );

        switch ( k->key() ) {
        case Qt::Key_Left:
            widget->moveLeft();
            break;
        case Qt::Key_Up:
            widget->moveUp();
            break;
        case Qt::Key_Right:
            widget->moveRight();
            break;
        case Qt::Key_Down:
            widget->moveDown();
            break;
        case Qt::Key_Plus:
            widget->zoomIn();
            break;
        case Qt::Key_Minus:
            widget->zoomOut();
            break;
        case Qt::Key_Home:
            widget->goHome();
            break;
        default:
            break;
        }

        return true;
    }

    return false;
}

}

#include "MarbleWidgetInputHandler.moc"

