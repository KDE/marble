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

#include "MarbleWidgetInputHandler.h"

#include <cmath>

#include <QtCore/QPoint>
#include <QtCore/QPointer>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QRubberBand>
#include <QtGui/QToolTip>

#if QT_VERSION >= 0x40600
  #include <QTouchEvent>
#endif

#include "global.h"
#include "MarbleDebug.h"
#include "GeoDataCoordinates.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "AbstractFloatItem.h"
#include "AbstractDataPluginItem.h"
#include "MeasureTool.h"
#include "MarbleWidgetPopupMenu.h"

using namespace Marble;

const int TOOLTIP_START_INTERVAL = 1000;

class MarbleWidgetInputHandler::Protected
{
public:
    Protected();

    MarbleWidget *m_widget;
    MarbleModel *m_model;
    bool m_positionSignalConnected;
    QTimer *m_mouseWheelTimer;
    Qt::MouseButtons m_disabledMouseButtons;
};

MarbleWidgetInputHandler::Protected::Protected()
    : m_widget( 0 ),
      m_model( 0 ),
      m_positionSignalConnected( false ),
      m_mouseWheelTimer( 0 ),
      m_disabledMouseButtons( Qt::NoButton )
{
}


MarbleWidgetInputHandler::MarbleWidgetInputHandler()
    : d( new Protected )
{
    d->m_mouseWheelTimer = new QTimer( this );
    connect( d->m_mouseWheelTimer, SIGNAL( timeout() ),
	     this, SLOT( restoreViewContext() ) );
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

void MarbleWidgetInputHandler::init( MarbleWidget *w )
{
    d->m_widget = w;
    d->m_model = w->model();
    
    connect( d->m_widget, SIGNAL( renderPluginInitialized( RenderPlugin * ) ),
             this,        SLOT( installPluginEventFilter( RenderPlugin * ) ) );

    foreach( RenderPlugin *renderPlugin, d->m_widget->renderPlugins() ) {
        if( renderPlugin->isInitialized() )
            d->m_widget->installEventFilter( renderPlugin );
    }
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

    QPixmap m_curpmtl;
    QPixmap m_curpmtc;
    QPixmap m_curpmtr;
    QPixmap m_curpmcr;
    QPixmap m_curpmcl;
    QPixmap m_curpmbl;
    QPixmap m_curpmbc;
    QPixmap m_curpmbr;

    QCursor m_arrowcur[3][3];

    bool m_leftpressed;
    bool m_midpressed;
    int m_leftpressedx;
    int m_leftpressedy;
    int m_midpressedy;
    qreal m_leftpresseda;
    qreal m_leftpressedb;

    int m_dragThreshold;
    QTime m_dragtimer;

    QPoint m_selectionOrigin;
    QRubberBand *m_selectionRubber;

    QPointer<AbstractDataPluginItem> m_lastToolTipItem;
    QTimer m_toolTipTimer;
    QPoint m_toolTipPosition;

    MarbleWidgetPopupMenu *m_popupmenu;
};

MarbleWidgetDefaultInputHandler::Private::Private()
    : m_leftpressed( false ),
      m_midpressed( false ),
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

    m_arrowcur[0][0] = QCursor( m_curpmtl, 2, 2 );
    m_arrowcur[1][0] = QCursor( m_curpmtc, 10, 3 );
    m_arrowcur[2][0] = QCursor( m_curpmtr, 19, 2 );
    m_arrowcur[0][1] = QCursor( m_curpmcl, 3, 10 );
    m_arrowcur[1][1] = QCursor( Qt::OpenHandCursor );
    m_arrowcur[2][1] = QCursor( m_curpmcr, 18, 10 );
    m_arrowcur[0][2] = QCursor( m_curpmbl, 2, 19 );
    m_arrowcur[1][2] = QCursor( m_curpmbc, 11, 18 );
    m_arrowcur[2][2] = QCursor( m_curpmbr, 19, 19 );
}

MarbleWidgetDefaultInputHandler::Private::~Private()
{
}


MarbleWidgetDefaultInputHandler::MarbleWidgetDefaultInputHandler()
    : MarbleWidgetInputHandler(), d( new Private )
{
    d->m_selectionRubber = new QRubberBand( QRubberBand::Rectangle,
                                            MarbleWidgetInputHandler::d->m_widget );
    d->m_selectionRubber->hide();

    d->m_toolTipTimer.setSingleShot( true );
    d->m_toolTipTimer.setInterval( TOOLTIP_START_INTERVAL );
    connect( &d->m_toolTipTimer, SIGNAL( timeout() ), this, SLOT( openItemToolTip() ) );
}

MarbleWidgetDefaultInputHandler::~MarbleWidgetDefaultInputHandler()
{
    // FIXME: move to Private
    delete d->m_selectionRubber;
    delete d;
}

void MarbleWidgetInputHandler::restoreViewContext()
{
    // Needs to stop the timer since it repeats otherwise.
    d->m_mouseWheelTimer->stop();

    // Redraw the map with the quality set for Marble::Still (if necessary).
    d->m_widget->setViewContext( Marble::Still );
    if ( d->m_widget->mapQuality( Marble::Still )
        != d->m_widget->mapQuality( Marble::Animation ) )
    {
        d->m_widget->updateChangedMap();
    }

    d->m_widget->map()->viewParams()->viewport()->resetFocusPoint();
}

void MarbleWidgetInputHandler::installPluginEventFilter( RenderPlugin *renderPlugin )
{
    d->m_widget->installEventFilter( renderPlugin );
}

void MarbleWidgetDefaultInputHandler::init( MarbleWidget *w )
{
    MarbleWidgetInputHandler::init( w );

    // The interface to the measure tool consists of a RMB popup menu
    // and some signals.
    MeasureTool *measureTool = MarbleWidgetInputHandler::d->m_widget->map()->measureTool();

    // Connect the inputHandler and the measure tool to the popup menu
    if ( !d->m_popupmenu ) {
        d->m_popupmenu = new MarbleWidgetPopupMenu( MarbleWidgetInputHandler::d->m_widget,
                                                    MarbleWidgetInputHandler::d->m_model );
    }
    connect( this, SIGNAL( rmbRequest( int, int ) ),
             this, SLOT( showRmbMenu( int, int ) ) );
    connect( d->m_popupmenu, SIGNAL( addMeasurePoint( qreal, qreal ) ),
             measureTool, SLOT( addMeasurePoint( qreal, qreal ) ) );
    connect( d->m_popupmenu, SIGNAL( removeLastMeasurePoint() ),
             measureTool, SLOT( removeLastMeasurePoint() ) );
    connect( d->m_popupmenu, SIGNAL( removeMeasurePoints() ),
             measureTool, SLOT( removeMeasurePoints( ) ) );
    connect( measureTool, SIGNAL( numberOfMeasurePointsChanged( int ) ),
             d->m_popupmenu, SLOT( slotNumberOfMeasurePointsChanged( int ) ) );
    connect( this, SIGNAL( lmbRequest( int, int ) ),
             this, SLOT( showLmbMenu( int, int ) ) );
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

    if ( keyEvent( MarbleWidgetInputHandler::d->m_widget->map(), e ) ) {
        MarbleWidgetInputHandler::d->m_widget->repaint();
        return true;
    }

    int polarity = MarbleWidgetInputHandler::d->m_widget->map()->viewParams()->viewport()->polarity();

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
            d->m_leftpressed = false;
        }
        if ( event->type() == QEvent::MouseMove
             && !( event->buttons() & Qt::MidButton ) )
        {
            d->m_midpressed = false;
        }

        // Do not handle (and therefore eat) mouse press and release events
        // that occur above visible float items. Mouse motion events are still
        // handled, however.
        foreach ( AbstractFloatItem *floatItem, MarbleWidgetInputHandler::d->m_widget->floatItems() ) {
            if ( floatItem->enabled() && floatItem->visible()
                 && floatItem->contains( event->pos() )
                 && e->type() != QEvent::MouseMove )
            {
                return false;
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

            if ( e->type() == QEvent::MouseButtonDblClick) {
                qDebug("check");
            }

            // Regarding mouse button presses:
            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton ) {

                d->m_dragtimer.restart();

                d->m_leftpressed = true;
                d->m_midpressed = false;
                d->m_selectionRubber->hide();

                // On the single event of a mouse button press these
                // values get stored, to enable us to e.g. calculate the
                // distance of a mouse drag while the mouse button is
                // still down.
                d->m_leftpressedx = event->x();
                d->m_leftpressedy = event->y();

                // Calculate translation of center point
                d->m_leftpresseda = MarbleWidgetInputHandler::d->m_widget->centerLongitude() * DEG2RAD;
                d->m_leftpressedb = MarbleWidgetInputHandler::d->m_widget->centerLatitude() * DEG2RAD;

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Marble::Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::MidButton ) {
                d->m_midpressed = true;
                d->m_leftpressed = false;
                d->m_midpressedy = event->y();

                d->m_selectionRubber->hide();
                MarbleWidgetInputHandler::d->m_widget->setViewContext( Marble::Animation );
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
                emit mouseClickScreenPosition( d->m_leftpressedx, d->m_leftpressedy );

                // Show menu if mouse cursor position remains unchanged
                // the click takes less than 250 ms
                if ( d->m_dragtimer.elapsed() <= 250
		     || ( d->m_leftpressedx == event->x()
			  && d->m_leftpressedy == event->y() ) ) {
                    emit lmbRequest( d->m_leftpressedx, d->m_leftpressedy );
                }

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Marble::Still );
                if ( MarbleWidgetInputHandler::d->m_widget->mapQuality( Marble::Still )
                    != MarbleWidgetInputHandler::d->m_widget->mapQuality( Marble::Animation ) )
                {
                    MarbleWidgetInputHandler::d->m_widget->updateChangedMap();
                }

                d->m_leftpressed = false;
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::MidButton ) {
                d->m_midpressed = false;

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Marble::Still );
                if ( MarbleWidgetInputHandler::d->m_widget->mapQuality( Marble::Still )
                    != MarbleWidgetInputHandler::d->m_widget->mapQuality( Marble::Animation ) )
                {
                    MarbleWidgetInputHandler::d->m_widget->updateChangedMap();
                }
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::RightButton) {
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
            if ( d->m_leftpressed && !d->m_selectionRubber->isVisible() ) {
                qreal radius = ( qreal )( MarbleWidgetInputHandler::d->m_widget->radius() );
                int deltax = event->x() - d->m_leftpressedx;
                int deltay = event->y() - d->m_leftpressedy;

                if ( abs( deltax ) > d->m_dragThreshold
                     || abs( deltay ) > d->m_dragThreshold )
		{
		  qreal direction = 1;
		  // Choose spin direction by taking into account whether we
		  // drag above or below the visible pole.
		  if ( MarbleWidgetInputHandler::d->m_widget->projection() == Spherical ) {
		      if ( polarity > 0 ) {

			  if ( event->y() < ( - MarbleWidgetInputHandler::d->m_widget->northPoleY()
					      + MarbleWidgetInputHandler::d->m_widget->height() / 2 ) )
			      direction = -1;
		      }
		      else {
			  if ( event->y() > ( + MarbleWidgetInputHandler::d->m_widget->northPoleY()
					      + MarbleWidgetInputHandler::d->m_widget->height() / 2 ) )
			      direction = -1;
		      }
		  }
		  MarbleWidgetInputHandler::d->m_widget->centerOn( RAD2DEG * ( qreal )( d->m_leftpresseda )
								  - 90.0 * direction * deltax / radius,
								  RAD2DEG * ( qreal )( d->m_leftpressedb )
								  + 90.0 * deltay / radius );
	      }
	    }


            if ( d->m_midpressed ) {
                int eventy = event->y();
                int dy = d->m_midpressedy - eventy;
                d->m_midpressed = eventy;
                MarbleWidgetInputHandler::d->m_widget->zoomViewBy( (int)( 2 * dy / 3 ) );
                MarbleWidgetInputHandler::d->m_widget->repaint();
            }

            if ( d->m_selectionRubber->isVisible() ) 
            {
                // We change selection.
                d->m_selectionRubber->setGeometry( QRect( d->m_selectionOrigin,
                                                          event->globalPos() ).normalized() );
            }
        }
        else {
            d->m_leftpressed = false;

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
            = MarbleWidgetInputHandler::d->m_widget->model()->whichItemAt( mousePosition );
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

        if ( ( MarbleWidgetInputHandler::d->m_widget->model()->whichFeatureAt( mousePosition ).size() == 0 )
             && ( !dataAction ) )
        {
            if ( !d->m_leftpressed )
                d->m_arrowcur [1][1] = QCursor( Qt::OpenHandCursor );
            else
                d->m_arrowcur [1][1] = QCursor( Qt::ClosedHandCursor );
        }
        else {
            if ( !d->m_leftpressed )
                d->m_arrowcur [1][1] = QCursor( Qt::PointingHandCursor );
        }

        MarbleWidgetInputHandler::d->m_widget->setCursor( d->m_arrowcur[dirX+1][dirY+1] );

        return false; // let others, especially float items, still process the event
    }
    else {
        if ( e->type() == QEvent::Wheel ) {
            MarbleWidget *marbleWidget = MarbleWidgetInputHandler::d->m_widget;
            // FIXME: disable animation quality after some time
            marbleWidget->setViewContext( Marble::Animation );

            QWheelEvent *wheelevt = static_cast<QWheelEvent*>( e );

            qreal  destLat;
            qreal  destLon;
            bool isValid = marbleWidget->geoCoordinates(wheelevt->x(), wheelevt->y(),
                             destLon, destLat, GeoDataCoordinates::Radian );

            marbleWidget->setUpdatesEnabled( false );
            marbleWidget->zoomViewBy( (int)(wheelevt->delta() / 3) );

            qreal  mouseLat;
            qreal  mouseLon;
            isValid = isValid && marbleWidget->geoCoordinates(wheelevt->x(), wheelevt->y(),
                        mouseLon, mouseLat, GeoDataCoordinates::Radian );

            qreal centerLat = DEG2RAD * marbleWidget->centerLatitude();
            qreal centerLon = DEG2RAD * marbleWidget->centerLongitude();

            if ( isValid ) {
                qreal lon = destLon - (mouseLon - centerLon);
                qreal lat = destLat - (mouseLat - centerLat);
                marbleWidget->centerOn( RAD2DEG * lon, RAD2DEG * lat );
            }
            marbleWidget->map()->viewParams()->viewport()->setFocusPoint(GeoDataCoordinates(destLon, destLat));
            marbleWidget->setUpdatesEnabled( true );

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

                MarbleWidgetInputHandler::d->m_widget->setViewContext( Marble::Animation );

                //convert the scaleFactor to be 0: the same: < 0: smaller, > 0: bigger and make it bigger by multiplying for an arbitrary big value
                MarbleWidgetInputHandler::d->m_widget->zoomViewBy( (scaleFactor-1)*200);
                MarbleWidgetInputHandler::d->m_mouseWheelTimer->start( 400 );
            }
        }
#endif
        else
            return false;
    }
    return QObject::eventFilter( o, e );
}

bool MarbleWidgetDefaultInputHandler::keyEvent( MarbleMap * map, QEvent* e )
{
    int polarity = map->viewParams()->viewport()->polarity();

    //  if ( o == marbleWidget ){
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent const * const k = dynamic_cast<QKeyEvent const * const>( e );
        Q_ASSERT( k );

        int dirx = 0;
        int diry = 0;
        switch ( k->key() ) {
        case Qt::Key_Left:

            // Depending on whether the planet is "upright" or
            // "upside down" we need to choose the direction
            //  of the rotation

            if ( polarity < 0 )
                dirx = -1;
            else
                dirx = 1;
            break;
        case Qt::Key_Up:
            diry = 1;
            break;
        case Qt::Key_Right:

            // Depending on whether the planet is "upright" or
            // "upside down" we need to choose the direction
            //  of the rotation

            if ( polarity < 0 )
                dirx = 1;
            else
                dirx = -1;
            break;
        case Qt::Key_Down:
            diry = -1;
            break;
        case Qt::Key_Plus:
            map->zoomIn();
            break;
        case Qt::Key_Minus:
            map->zoomOut();
            break;
        case Qt::Key_Home:
            map->goHome();
            break;
        default:
            break;
        }

        if ( dirx != 0 || diry != 0 ) {
            map->rotateBy( -map->moveStep() * (qreal)(dirx),
                           -map->moveStep() * (qreal)(diry) );
        }

        return true;
    }

    return false;
}


#include "MarbleWidgetInputHandler.moc"
