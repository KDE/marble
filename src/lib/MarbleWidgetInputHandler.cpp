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

#include "MarbleWidgetInputHandler.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QRubberBand>

#include "global.h"
#include "GeoDataCoordinates.h"
#include "Quaternion.h"
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "MarbleAbstractFloatItem.h"
#include "MeasureTool.h"
#include "MarbleWidgetPopupMenu.h"

using namespace Marble;

MarbleWidgetInputHandler::MarbleWidgetInputHandler()
    : m_widget( 0 ),
      m_model( 0 ),
      m_positionSignalConnected( false ),
      m_mouseWheelTimer( new QTimer(this) )
{
    connect( m_mouseWheelTimer, SIGNAL( timeout() ),
	     this,              SLOT( restoreViewContext() ) );
}

MarbleWidgetInputHandler::~MarbleWidgetInputHandler()
{
    delete m_mouseWheelTimer;
}

void MarbleWidgetInputHandler::init(MarbleWidget *w)
{
    m_widget = w;
    m_model = w->model();

    foreach(MarbleAbstractFloatItem *floatItem, m_widget->floatItems())
    {
        m_widget->installEventFilter(floatItem);
    }
}


MarbleWidgetDefaultInputHandler::MarbleWidgetDefaultInputHandler()
    : MarbleWidgetInputHandler(), m_popupmenu( 0 ), m_leftpopup(true)
{
    curpmtl.load( MarbleDirs::path("bitmaps/cursor_tl.xpm") );
    curpmtc.load( MarbleDirs::path("bitmaps/cursor_tc.xpm") );
    curpmtr.load( MarbleDirs::path("bitmaps/cursor_tr.xpm") );
    curpmcr.load( MarbleDirs::path("bitmaps/cursor_cr.xpm") );
    curpmcl.load( MarbleDirs::path("bitmaps/cursor_cl.xpm") );
    curpmbl.load( MarbleDirs::path("bitmaps/cursor_bl.xpm") );
    curpmbc.load( MarbleDirs::path("bitmaps/cursor_bc.xpm") );
    curpmbr.load( MarbleDirs::path("bitmaps/cursor_br.xpm") );

    arrowcur [0][0] = QCursor(curpmtl,2,2);
    arrowcur [1][0] = QCursor(curpmtc,10,3);
    arrowcur [2][0] = QCursor(curpmtr,19,2);
    arrowcur [0][1] = QCursor(curpmcl,3,10);
    arrowcur [1][1] = QCursor(Qt::OpenHandCursor);
    arrowcur [2][1] = QCursor(curpmcr,18,10);
    arrowcur [0][2] = QCursor(curpmbl,2,19);
    arrowcur [1][2] = QCursor(curpmbc,11,18);
    arrowcur [2][2] = QCursor(curpmbr,19,19);

    m_leftpressed     = false;
    m_midpressed      = false;

    m_selectionRubber = new QRubberBand(QRubberBand::Rectangle, m_widget);
    m_selectionRubber->hide();

    m_dragThreshold   = 3;    
}

void MarbleWidgetInputHandler::restoreViewContext()
{
    // Needs to stop the timer since it repeats otherwise.
    m_mouseWheelTimer->stop();

    // Redraw the map with the quality set for Marble::Still (if necessary).
    m_widget->setViewContext( Marble::Still );
    if ( m_widget->mapQuality( Marble::Still )
        != m_widget->mapQuality( Marble::Animation ) )
    {
        m_widget->updateChangedMap();
    }
}

void MarbleWidgetDefaultInputHandler::init(MarbleWidget *w)
{
    MarbleWidgetInputHandler::init(w);
  
    // The interface to the measure tool consists of a RMB popup menu
    // and some signals.
    MeasureTool  *measureTool = m_widget->map()->measureTool();

    // Connect the inputHandler and the measure tool to the popup menu
    if ( !m_popupmenu ) {
        m_popupmenu    = new MarbleWidgetPopupMenu( m_widget, m_model );
    }
    connect( this, SIGNAL( rmbRequest( int, int ) ),
                       m_popupmenu,    SLOT( showRmbMenu( int, int ) ) );
    connect( m_popupmenu, SIGNAL( addMeasurePoint( qreal, qreal ) ),
                       measureTool, SLOT( addMeasurePoint( qreal, qreal ) ) );
    connect( m_popupmenu, SIGNAL( removeMeasurePoints() ),
                       measureTool, SLOT( removeMeasurePoints( ) ) );  
    connect( this, SIGNAL( lmbRequest( int, int ) ),
                       this,    SLOT( showLmbMenu( int, int ) ) );		       
}

void MarbleWidgetDefaultInputHandler::showLmbMenu( int x, int y)
{
  if (m_leftpopup)
  {
    m_popupmenu->showLmbMenu(x,y);
  }
}

void MarbleWidgetDefaultInputHandler::setLeftMouseButtonPopup(bool enabled)
{
  m_leftpopup = enabled;
}
    
bool MarbleWidgetDefaultInputHandler::leftMouseButtonPopup()
{
  return m_leftpopup;
}

bool MarbleWidgetDefaultInputHandler::eventFilter( QObject* o, QEvent* e )
{
    Q_UNUSED( o );

    if ( keyEvent( m_widget->map(), e ) ) {
        m_widget->repaint();
        return true;
    }

    int polarity = m_widget->map()->viewParams()->viewport()->polarity();

    if ( e->type() == QEvent::MouseMove
              || e->type() == QEvent::MouseButtonPress
              || e->type() == QEvent::MouseButtonRelease ) {

        QMouseEvent  *event        = static_cast<QMouseEvent*>(e);
        QRegion       activeRegion = m_widget->activeRegion();

        m_dirX = 0;
        m_dirY = 0;

        // Do not handle (and therefore eat) mouse press and release events 
        // that occur above visible float items. Mouse motion events are still 
        // handled, however.
        foreach( MarbleAbstractFloatItem *floatItem, m_widget->floatItems() ) {
            QRectF widgetRect(0,0,m_widget->width(),m_widget->height());
            QRectF floatItemRect = QRectF(floatItem->positivePosition(widgetRect), 
                    floatItem->size());
            if ( floatItem->enabled() && floatItem->visible() 
                    && floatItemRect.contains(event->pos())
                    && e->type() != QEvent::MouseMove )
            {
                return false;
            }
        }

        // emit the position string only if the signal got attached
        if ( m_positionSignalConnected ) {
            qreal  lat;
            qreal  lon;
            bool    isValid = m_widget->geoCoordinates( event->x(), event->y(),
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


        if ( activeRegion.contains( event->pos() ) || m_selectionRubber->isVisible() ) {

            if ( e->type() == QEvent::MouseButtonDblClick) {
                qDebug("check");
            }

            // Regarding mouse button presses:
            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton ) {

                m_dragtimer.restart();

                m_leftpressed    = true;
                m_midpressed     = false;
                m_selectionRubber->hide();

                // On the single event of a mouse button press these
                // values get stored, to enable us to e.g. calculate the
                // distance of a mouse drag while the mouse button is
                // still down.
                m_leftpressedx = event->x();
                m_leftpressedy = event->y();

                // Calculate translation of center point
                m_leftpresseda =  m_widget->centerLongitude() * DEG2RAD;
                m_leftpressedb =  m_widget->centerLatitude() * DEG2RAD;

                m_widget->setViewContext( Marble::Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::MidButton) {
                m_midpressed     = true;
                m_leftpressed    = false;
                m_midpressedy    = event->y();

                m_selectionRubber->hide();
                m_widget->setViewContext( Marble::Animation );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::RightButton ) {
                emit rmbRequest( event->x(), event->y() );
            }

            if ( e->type() == QEvent::MouseButtonPress
                 && event->button() == Qt::LeftButton 
                 && (event->modifiers() & Qt::ControlModifier) )
            {
                qDebug("Marble: Starting selection");
                m_selectionOrigin = event->globalPos();
                m_selectionRubber->setGeometry(QRect(m_selectionOrigin, QSize()));
                m_selectionRubber->show();
            }

            // Regarding mouse button releases:
            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::LeftButton)
            {

                //emit current coordinates to be be interpreted
                //as requested
                emit mouseClickScreenPosition( m_leftpressedx, m_leftpressedy );

                // Show menu if mouse cursor position remains unchanged
                // the click takes less than 250 ms
                if ( m_dragtimer.elapsed() <= 250
		     || ( m_leftpressedx == event->x()
			  && m_leftpressedy == event->y() ) ) {
                    emit lmbRequest( m_leftpressedx, m_leftpressedy );
                }

                m_widget->setViewContext( Marble::Still );
                if ( m_widget->mapQuality( Marble::Still )
                    != m_widget->mapQuality( Marble::Animation ) )
                {
                    m_widget->updateChangedMap();
                }

                m_leftpressed = false;
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::MidButton) {
                m_midpressed = false;

                m_widget->setViewContext( Marble::Still );
                if ( m_widget->mapQuality( Marble::Still )
                    != m_widget->mapQuality( Marble::Animation ) )
                {
                    m_widget->updateChangedMap();
                }
            }

            if ( e->type() == QEvent::MouseButtonRelease
                 && event->button() == Qt::RightButton) {
            }

            if ( e->type() == QEvent::MouseButtonRelease
                && event->button() == Qt::LeftButton 
                 && m_selectionRubber->isVisible() ) 
            {
               qDebug("Marble: Leaving selection");
               QRect r = QRect(m_widget->mapFromGlobal(m_selectionRubber->geometry().topLeft()),                                 m_widget->mapFromGlobal(m_selectionRubber->geometry().bottomRight()));
               m_widget->setSelection(r);
               m_selectionRubber->hide();
            }

            // Regarding all kinds of mouse moves:
            if ( m_leftpressed && !m_selectionRubber->isVisible() ) {
                qreal  radius = (qreal)(m_widget->radius());
                int     deltax = event->x() - m_leftpressedx;
                int     deltay = event->y() - m_leftpressedy;

                if ( abs(deltax) <= m_dragThreshold
                     && abs(deltay) <= m_dragThreshold )
                    return true;

                qreal direction = 1;
                // Choose spin direction by taking into account whether we
                // drag above or below the visible pole.
                if ( m_widget->projection() == Spherical ) {
                    if ( polarity > 0 ) {

                        if ( event->y() < ( - m_widget->northPoleY()
                                        + m_widget->height() / 2 ) )
                            direction = -1;
                    }
                    else {
                        if (event->y() > ( + m_widget->northPoleY()
                                           + m_widget->height() / 2 ) )
                            direction = -1;
                    }
                }
                m_widget->centerOn( RAD2DEG * (qreal)(m_leftpresseda)
                                    - 90.0 * direction * deltax / radius,
                                    RAD2DEG * (qreal)(m_leftpressedb)
                                    + 90.0 * deltay / radius );
            }


            if ( m_midpressed ) {
                int  eventy = event->y();
                int  dy     = m_midpressedy - eventy;
                m_midpressed = eventy;
                m_widget->zoomViewBy( (int)( 2 * dy / 3 ) );
                m_widget->repaint();
            }

            if ( m_selectionRubber->isVisible() ) 
            {
                // We change selection.
                m_selectionRubber->setGeometry(QRect(m_selectionOrigin,
                                                     event->globalPos()).normalized());
            }
        }
        else {
            m_leftpressed = false;

            QRect boundingRect = m_widget->projectedRegion().boundingRect();

            if ( boundingRect.width() != 0 )
            {
                m_dirX = (int)( 3 * ( event->x() - boundingRect.left() ) / boundingRect.width() ) - 1;
            }

            if ( m_dirX > 1 )
                m_dirX = 1;
            if ( m_dirX < -1 )
                m_dirX = -1;

            if ( boundingRect.height() != 0 )
            {
                m_dirY = (int)( 3 * ( event->y() - boundingRect.top() ) / boundingRect.height() ) - 1;
            }

            if ( m_dirY > 1 )
                m_dirY = 1;
            if ( m_dirY < -1 )
                m_dirY = -1;

            if ( event->button() == Qt::LeftButton
                 && e->type() == QEvent::MouseButtonPress ) {

                if ( polarity < 0 )
                    m_widget->rotateBy( -m_widget->moveStep() * (qreal)(+m_dirX),
                                        m_widget->moveStep() * (qreal)(+m_dirY) );
                else
                    m_widget->rotateBy( -m_widget->moveStep() * (qreal)(-m_dirX),
                                        m_widget->moveStep() * (qreal)(+m_dirY) );
            }
        }

        // Adjusting Cursor shape

        if ( ( m_widget->model()->whichFeatureAt( QPoint( event->x(),
                                                          event->y() ) ) ).size() == 0 )
        {
            if ( !m_leftpressed )
                arrowcur [1][1] = QCursor(Qt::OpenHandCursor);
            else
                arrowcur [1][1] = QCursor(Qt::ClosedHandCursor);
        }
        else {
            if ( !m_leftpressed )
                arrowcur [1][1] = QCursor(Qt::PointingHandCursor);
        }

        m_widget->setCursor(arrowcur[m_dirX+1][m_dirY+1]);

        return false; // let others, especially float items, still process the event
    }
    else {
        if ( e->type() == QEvent::Wheel ) {
            // FIXME: disable animation quality after some time
            m_widget->setViewContext( Marble::Animation );

            QWheelEvent  *wheelevt = static_cast<QWheelEvent*>(e);
            m_widget->zoomViewBy( (int)(wheelevt->delta() / 3) );
            m_mouseWheelTimer->start(400);
            return true;
        }
        else
            return false;
    }
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
