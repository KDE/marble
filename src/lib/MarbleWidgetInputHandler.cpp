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
  #include <QtGui/QTouchEvent>
#endif

#include "global.h"
#include "MarbleDebug.h"
#include "GeoDataCoordinates.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "AbstractFloatItem.h"
#include "AbstractDataPluginItem.h"
#include "MeasureTool.h"
#include "MarbleWidgetPopupMenu.h"
#include "AbstractProjection.h"

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
};

MarbleWidgetInputHandler::Protected::Protected( MarbleWidget *widget )
    : m_widget( widget ),
      m_model( widget->model() ),
      m_positionSignalConnected( false ),
      m_mouseWheelTimer( 0 ),
      m_disabledMouseButtons( Qt::NoButton ),
      m_wheelZoomTargetDistance( 0.0 )
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


class MarbleWidgetDefaultInputHandler::Private
{
 public:
    Private();
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
    // Indicates if the middle mouse button has been pressed already.
    bool m_midPressed;
    // The mouse pointer x position when the left mouse button has been pressed.
    int m_leftPressedX;
    // The mouse pointer y position when the left mouse button has been pressed.
    int m_leftPressedY;
    // The mouse pointer y position when the middle mouse button has been pressed.
    int m_midPressedY;
    int m_radiusWhenPressed;
    bool m_rightPressed;
    QPoint m_rightOrigin;
    qreal m_headingWhenPressed;
    qreal m_tiltWhenPressed;
    // The center longitude in radian when the left mouse button has been pressed.
    qreal m_leftPressedLon;
    // The center latitude in radian when the left mouse button has been pressed.
    qreal m_leftPressedLat;

    int m_dragThreshold;
    QTimer m_lmbTimer;

    QPoint m_selectionOrigin;
    QRubberBand *m_selectionRubber;

    QPointer<AbstractDataPluginItem> m_lastToolTipItem;
    QTimer m_toolTipTimer;
    QPoint m_toolTipPosition;

    MarbleWidgetPopupMenu *m_popupmenu;

    QAction  *m_addMeasurePointAction;
    QAction  *m_removeLastMeasurePointAction;
    QAction  *m_removeMeasurePointsAction;
};

MarbleWidgetDefaultInputHandler::Private::Private()
    : m_leftPressed( false ),
      m_midPressed( false ),
      m_rightPressed( false ),
      m_dragThreshold( 3 ),
      m_popupmenu( 0 )
{
    m_curpmtl.load( MarbleDirs::path("bitmaps/cursor_tl.xpm") );
    m_curpmtc.load( MarbleDirs::path("bitmaps/cursor_tc.xpm") );
    m_curpmtr.load( MarbleDirs::path("bitmaps/cursor_tr.xpm") );
    m_curpmcr.load( MarbleDirs::path("bitmaps/cursor_cr.xpm") );
    m_curpmcl.load( MarbleDirs::path("bitmaps/cursor_cl.xpm") );
    m_curpmbl.load( MarbleDirs::path("bitmaps/cursor_bl.xpm") );
    m_curpmbc.load( MarbleDirs::path("bitmaps/cursor_bc.xpm") );
    m_curpmbr.load( MarbleDirs::path("bitmaps/cursor_br.xpm") );

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
          destLon, destLat, GeoDataCoordinates::Radian )) {
        return;
    }

    ViewportParams* now = marbleWidget->viewport();

    qreal x(0), y(0);
    if (!now->currentProjection()->screenCoordinates(destLon, destLat, now, x, y)) {
        return;
    }

    ViewportParams soon;
    soon.setProjection(now->projection());
    soon.setPlanetAxis(now->planetAxis());
    soon.setSize(now->size());

    qreal newRadius = marbleWidget->radiusFromDistance(newDistance);
    soon.setRadius( newRadius );

    qreal mouseLon, mouseLat;
    if (!soon.currentProjection()->geoCoordinates(int(x), int(y), &soon, mouseLon, mouseLat, GeoDataCoordinates::Radian )) {
        return;
    }

    qreal centerLat = DEG2RAD * marbleWidget->centerLatitude();
    qreal centerLon = DEG2RAD * marbleWidget->centerLongitude();

    qreal lon = destLon - ( mouseLon - centerLon );
    qreal lat = destLat - ( mouseLat - centerLat );

    GeoDataLookAt lookAt;
    lookAt.setLongitude( lon );
    lookAt.setLatitude( lat );
    lookAt.setAltitude( 0.0 );
    lookAt.setRange( newDistance * KM2METER );

    marbleWidget->viewport()->setFocusPoint( GeoDataCoordinates( destLon, destLat ) );
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
    : MarbleWidgetInputHandler( widget ), d( new Private )
{
    d->m_selectionRubber = new QRubberBand( QRubberBand::Rectangle,
                                            MarbleWidgetInputHandler::d->m_widget );
    d->m_selectionRubber->hide();

    d->m_toolTipTimer.setSingleShot( true );
    d->m_toolTipTimer.setInterval( TOOLTIP_START_INTERVAL );
    connect( &d->m_toolTipTimer, SIGNAL( timeout() ), this, SLOT( openItemToolTip() ) );
    d->m_lmbTimer.setSingleShot(true);
    connect( &d->m_lmbTimer, SIGNAL(timeout()), this, SLOT(lmbTimeout()));

    // The interface to the measure tool consists of a RMB popup menu
    // and some signals.
    MeasureTool *measureTool = MarbleWidgetInputHandler::d->m_widget->measureTool();

    // Connect the inputHandler and the measure tool to the popup menu
    if ( !d->m_popupmenu ) {
        d->m_popupmenu = new MarbleWidgetPopupMenu( MarbleWidgetInputHandler::d->m_widget,
                                                    MarbleWidgetInputHandler::d->m_model );
    }

    d->m_addMeasurePointAction = new QAction( tr( "Add &Measure Point" ), this);
    d->m_popupmenu->addAction( Qt::RightButton, d->m_addMeasurePointAction );
    d->m_removeLastMeasurePointAction = new QAction( tr( "Remove &Last Measure Point" ),
                                                  this);
    d->m_removeLastMeasurePointAction->setEnabled(false);
    d->m_popupmenu->addAction( Qt::RightButton, d->m_removeLastMeasurePointAction );
    d->m_removeMeasurePointsAction = new QAction( tr( "&Remove Measure Points" ),
                                                this);
    d->m_removeMeasurePointsAction->setEnabled(false);
    d->m_popupmenu->addAction( Qt::RightButton, d->m_removeMeasurePointsAction );

    connect( d->m_addMeasurePointAction, SIGNAL( triggered() ),
             SLOT( addMeasurePoint() ) );
    connect( d->m_removeLastMeasurePointAction, SIGNAL(triggered() ),
             measureTool, SLOT( removeLastMeasurePoint() ) );
    connect( d->m_removeMeasurePointsAction, SIGNAL( triggered() ),
             measureTool, SLOT( removeMeasurePoints() ) );

    connect( this, SIGNAL( rmbRequest( int, int ) ),
             this, SLOT( showRmbMenu( int, int ) ) );
    connect( measureTool, SIGNAL( numberOfMeasurePointsChanged( int ) ),
             this, SLOT( setNumberOfMeasurePoints( int ) ) );
    connect( this, SIGNAL( lmbRequest( int, int ) ),
             this, SLOT( showLmbMenu( int, int ) ) );
}

MarbleWidgetDefaultInputHandler::~MarbleWidgetDefaultInputHandler()
{
    // FIXME: move to Private
    delete d->m_selectionRubber;
    delete d;
}

void MarbleWidgetDefaultInputHandler::lmbTimeout()
{
    if (!d->m_selectionRubber->isVisible()) {
        emit lmbRequest( d->m_leftPressedX, d->m_leftPressedY );
    }
}

void MarbleWidgetInputHandler::restoreViewContext()
{
    // Needs to stop the timer since it repeats otherwise.
    d->m_mouseWheelTimer->stop();

    // Redraw the map with the quality set for Still (if necessary).
    d->m_widget->setViewContext( Still );
    d->m_widget->viewport()->resetFocusPoint();
    d->m_wheelZoomTargetDistance = 0.0;
}

void MarbleWidgetInputHandler::installPluginEventFilter( RenderPlugin *renderPlugin )
{
    d->m_widget->installEventFilter( renderPlugin );
}

void MarbleWidgetDefaultInputHandler::showLmbMenu( int x, int y )
{
    if ( isMouseButtonPopupEnabled( Qt::LeftButton ) ) {
        d->m_popupmenu->showLmbMenu( x, y );
    }
}

void MarbleWidgetDefaultInputHandler::showRmbMenu( int x, int y )
{
    if ( isMouseButtonPopupEnabled( Qt::RightButton ) ) {
        d->m_addMeasurePointAction->setData( QPoint( x, y ) );
        d->m_popupmenu->showRmbMenu( x, y );
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

    if (d->m_selectionRubber->isVisible() && e->type() == QEvent::MouseMove)
    {
        QMouseEvent *event = static_cast<QMouseEvent*>( e );
        if (!(event->modifiers() & Qt::ControlModifier))
        {
            d->m_selectionRubber->hide();
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
        QRegion activeRegion = MarbleWidgetInputHandler::d->m_widget->activeRegion();

        int dirX = 0;
        int dirY = 0;

        // To prevent error from lost MouseButtonRelease events
        if ( event->type() == QEvent::MouseMove
             && !( event->buttons() & Qt::LeftButton ) )
        {
            d->m_leftPressed = false;
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
             && !d->m_selectionRubber->isVisible() )
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

        // emit the position string only if the signal got attached
        if ( MarbleWidgetInputHandler::d->m_positionSignalConnected ) {
            qreal lat;
            qreal lon;
            bool isValid = MarbleWidgetInputHandler::d->m_widget->geoCoordinates( event->x(), event->y(),
                                                     lon, lat,
                                                     GeoDataCoordinates::Radian );

            if ( !isValid ) {
                emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
            }
            else {
                QString position = GeoDataCoordinates( lon, lat ).toString();
                emit mouseMoveGeoPosition( position );
            }
        }


        if ( activeRegion.contains( event->pos() ) || d->m_selectionRubber->isVisible() ) {

            // Regarding mouse button presses:
            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton ) {

                if (isMouseButtonPopupEnabled(Qt::LeftButton))
                    d->m_lmbTimer.start(400);

                d->m_leftPressed = true;
                d->m_midPressed = false;
                d->m_selectionRubber->hide();

                // On the single event of a mouse button press these
                // values get stored, to enable us to e.g. calculate the
                // distance of a mouse drag while the mouse button is
                // still down.
                d->m_leftPressedX = event->x();
                d->m_leftPressedY = event->y();

                // Calculate translation of center point
                d->m_leftPressedLon = MarbleWidgetInputHandler::d->m_widget->centerLongitude() * DEG2RAD;
                d->m_leftPressedLat = MarbleWidgetInputHandler::d->m_widget->centerLatitude() * DEG2RAD;

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::MidButton ) {
                d->m_midPressed = true;
                d->m_leftPressed = false;
                d->m_radiusWhenPressed = MarbleWidgetInputHandler::d->m_widget->radius();
                d->m_midPressedY = event->y();

                d->m_selectionRubber->hide();
                MarbleWidgetInputHandler::d->m_widget->setViewContext( Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::RightButton ) {
                d->m_rightPressed = true;
                d->m_rightOrigin = event->pos();
                d->m_headingWhenPressed = MarbleWidgetInputHandler::d->m_widget->heading();
                d->m_tiltWhenPressed = MarbleWidgetInputHandler::d->m_widget->tilt();
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton
                 && ( event->modifiers() & Qt::ControlModifier ) )
            {
                qDebug("Marble: Starting selection");
                d->m_lmbTimer.stop();                
                d->m_selectionOrigin = event->globalPos();
                d->m_selectionRubber->setGeometry( QRect( d->m_selectionOrigin, QSize() ));
                d->m_selectionRubber->show();
            }

            // Regarding mouse button releases:
            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::LeftButton )
            {

                //emit current coordinates to be be interpreted
                //as requested
                emit mouseClickScreenPosition( d->m_leftPressedX, d->m_leftPressedY );

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );

                d->m_leftPressed = false;
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::MidButton ) {
                d->m_midPressed = false;

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Still );
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::RightButton) {
                if ( d->m_rightOrigin == event->pos() ) {
                    emit rmbRequest( event->x(), event->y() );
                }
                d->m_rightPressed = false;
            }

            if ( e->type() == QEvent::MouseButtonRelease
                && event->button() == Qt::LeftButton 
                 && d->m_selectionRubber->isVisible() ) 
            {
                qDebug("Marble: Leaving selection");
                QRect r( MarbleWidgetInputHandler::d->m_widget->mapFromGlobal( d->m_selectionRubber->geometry().topLeft() ),
                         MarbleWidgetInputHandler::d->m_widget->mapFromGlobal( d->m_selectionRubber->geometry().bottomRight() ));
                MarbleWidgetInputHandler::d->m_widget->setSelection( r );
                d->m_selectionRubber->hide();
            }

            // Regarding all kinds of mouse moves:
            if ( d->m_leftPressed && !d->m_selectionRubber->isVisible() ) {
                qreal radius = ( qreal )( MarbleWidgetInputHandler::d->m_widget->radius() );
                qreal deltax = event->x() - d->m_leftPressedX;
                qreal deltay = event->y() - d->m_leftPressedY;

                if ( abs( deltax ) > d->m_dragThreshold
                     || abs( deltay ) > d->m_dragThreshold ) {

                    d->m_lmbTimer.stop();
                    Quaternion rotation;
                    rotation.createFromEuler( 0, 0, MarbleWidgetInputHandler::d->m_widget->heading() * DEG2RAD );
                    Quaternion quat( - M_PI/2 * deltax / radius, + M_PI/2 * deltay / radius );
                    quat.rotateAroundAxis( rotation );
                    quat.getSpherical( deltax, deltay );
                    MarbleWidgetInputHandler::d->m_widget->centerOn( RAD2DEG * ( qreal )( d->m_leftPressedLon + deltax ),
                                                                     RAD2DEG * ( qreal )( d->m_leftPressedLat + deltay ) );
                }
            }


            if ( d->m_midPressed ) {
                int eventy = event->y();
                int dy = d->m_midPressedY - eventy;
                MarbleWidgetInputHandler::d->m_widget->setRadius( d->m_radiusWhenPressed * pow( 1.005, dy ) );
            }

            if ( d->m_rightPressed ) {
                const int dx = event->x() - d->m_rightOrigin.x();
                MarbleWidgetInputHandler::d->m_widget->setHeading( d->m_headingWhenPressed + dx );

                int tilt = d->m_tiltWhenPressed + event->y() - d->m_rightOrigin.y();
                if ( tilt < 0 )
                    tilt = 0;
                if ( tilt > 90 )
                    tilt = 90;
                MarbleWidgetInputHandler::d->m_widget->setTilt( tilt );
            }

            if ( d->m_selectionRubber->isVisible() ) 
            {
                // We change selection.
                d->m_selectionRubber->setGeometry( QRect( d->m_selectionOrigin,
                                                          event->globalPos() ).normalized() );
            }
        }
        else {
            d->m_leftPressed = false;

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
                 && e->type() == QEvent::MouseButtonPress ) {

                d->m_lmbTimer.stop();                
                if ( polarity < 0 )
                    MarbleWidgetInputHandler::d->m_widget->rotateBy( -MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirX),
                                                                     MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirY) );
                else
                    MarbleWidgetInputHandler::d->m_widget->rotateBy( -MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(-dirX),
                                                                     MarbleWidgetInputHandler::d->m_widget->moveStep() * (qreal)(+dirY) );
            }
        }

        // Adjusting Cursor shape

        QPoint mousePosition( event->x(), event->y() );
        
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

        MarbleWidgetInputHandler::d->m_widget->setCursor( d->m_arrowCur[dirX+1][dirY+1] );

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

            MarbleWidgetInputHandler::d->m_mouseWheelTimer->start( 400 );
            return true;
        }
#if QT_VERSION >= 0x40600
        else if ( e->type() == QEvent::TouchBegin ||
                  e->type() == QEvent::TouchUpdate ||
                  e->type() == QEvent::TouchEnd) {

            QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>( e )->touchPoints();
            if (touchPoints.count() == 2) {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                const QLineF line0(touchPoint0.lastPos(), touchPoint1.lastPos());
                const QLineF line1(touchPoint0.pos(), touchPoint1.pos());
                //scaleFactor is the ratio the view will be scaled compared to now,
                // 1:the same, 2: the double, 0.5: half
                qreal scaleFactor = 1;

                if (line0.length() > 0) {
                    scaleFactor = line1.length() / line0.length();
                }

                MarbleWidget *marbleWidget = MarbleWidgetInputHandler::d->m_widget;

                marbleWidget->setViewContext( Animation );

                qreal  destLat;
                qreal  destLon;
                QPointF center = line1.pointAt(0.5);
                bool isValid = marbleWidget->geoCoordinates(center.x(), center.y(),
                             destLon, destLat, GeoDataCoordinates::Radian );

                if (isValid) {
                    marbleWidget->viewport()->setFocusPoint(GeoDataCoordinates(destLon, destLat));
                }

                //convert the scaleFactor to be 0: the same: < 0: smaller, > 0: bigger and make it bigger by multiplying for an arbitrary big value
                marbleWidget->zoomViewBy( (scaleFactor-1)*200);
                MarbleWidgetInputHandler::d->m_mouseWheelTimer->start( 400 );
            }
        }
#endif
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

void MarbleWidgetDefaultInputHandler::addMeasurePoint()
{
    QPoint  p = d->m_addMeasurePointAction->data().toPoint();

    qreal  lat;
    qreal  lon;

    MarbleWidgetInputHandler::d->m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
    MeasureTool *measureTool = MarbleWidgetInputHandler::d->m_widget->measureTool();
    measureTool->addMeasurePoint( lon, lat );
}

void MarbleWidgetDefaultInputHandler::setNumberOfMeasurePoints( int newNumber )
{
    const bool enableMeasureActions = ( newNumber > 0 );
    d->m_removeMeasurePointsAction->setEnabled(enableMeasureActions);
    d->m_removeLastMeasurePointAction->setEnabled(enableMeasureActions);
}

}

#include "MarbleWidgetInputHandler.moc"
