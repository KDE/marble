//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Florian Eßer <f.esser@rwth-aachen.de>
//

#include "ElevationProfileFloatItem.h"

#include <QThread>
#include <QtCore/QRect>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

#include "ui_ElevationProfileConfigWidget.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "routing/RoutingManager.h"
#include "MarbleDirs.h"
#include "ElevationModel.h"
#include "MarbleGraphicsGridLayout.h"
#include "LabelGraphicsItem.h"

namespace Marble
{

ElevationProfileFloatItem::ElevationProfileFloatItem( const QPointF &point, const QSizeF &size )
        : AbstractFloatItem( point, size ),
        m_aboutDialog( 0 ),
        m_configDialog( 0 ),
        m_target( QString() ),
        m_leftGraphMargin( 0 ),
        m_eleGraphWidth( 0 ),
        m_viewportWidth( 0 ),
        m_eleGraphHeight( 50 ),
        m_bestDivisorX( 0 ),
        m_pixelIntervalX( 0 ),
        m_valueIntervalX( 0 ),
        m_bestDivisorY( 0 ),
        m_pixelIntervalY( 0 ),
        m_valueIntervalY( 0 ),
        m_unitX( tr( "km" ) ),
        m_unitY( tr( "m") ),
        m_isInitialized( false ),
        m_contextMenu( 0 ),
        m_marbleWidget( 0 ),
        m_routingModel( 0 ),
        m_routingLayer( 0 ),
        m_routeAvailable( false ),
        m_firstVisiblePoint( 0 ),
        m_lastVisiblePoint( 0 ),
        m_zoomToViewport( false ),
        m_lastMarkerRegion( QRegion() )

{
    setVisible( false );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        setPosition( QPointF( 220.0, 10.5 ) );
    }
    setPadding(1);
    m_markerIcon = new LabelGraphicsItem( &m_markerIconContainer );
    m_markerIcon->setImage( QImage( ":/flag-red-mirrored.png" ) );
    m_markerText = new LabelGraphicsItem( &m_markerTextContainer );

    MarbleGraphicsGridLayout *topLayout1 = new MarbleGraphicsGridLayout( 1, 1 );
    m_markerIconContainer.setLayout( topLayout1 );
    topLayout1->addItem( m_markerIcon, 0, 0 );

    MarbleGraphicsGridLayout *topLayout2 = new MarbleGraphicsGridLayout( 1, 1 );
    m_markerTextContainer.setLayout( topLayout2 );
    m_markerText->setFrame( RoundedRectFrame );
    topLayout2->setAlignment( Qt::AlignCenter );
    topLayout2->addItem( m_markerText, 0, 1 );
}

ElevationProfileFloatItem::~ElevationProfileFloatItem()
{
}

QStringList ElevationProfileFloatItem::backendTypes() const
{
    return QStringList( "elevationprofile" );
}

QStringList ElevationProfileFloatItem::renderPosition() const
{
    return QStringList() << "FLOAT_ITEM" << "HOVERS_ABOVE_SURFACE";
}

qreal ElevationProfileFloatItem::zValue() const
{
    return 1.0;
}

QString ElevationProfileFloatItem::name() const
{
    return tr("Elevation Profile");
}

QString ElevationProfileFloatItem::guiString() const
{
    return tr("&Elevation Profile");
}

QString ElevationProfileFloatItem::nameId() const
{
    return QString( "elevationprofile" );
}

QString ElevationProfileFloatItem::description() const
{
    return tr("This is a float item that provides an route/track elevation profile.");
}

QIcon ElevationProfileFloatItem::icon () const
{
    return QIcon(":/icons/elevationprofile.png");
}

void ElevationProfileFloatItem::initialize ()
{
    connect( marbleModel()->elevationModel(), SIGNAL( updateAvailable() ), SLOT( updateData() ) );
}

bool ElevationProfileFloatItem::isInitialized () const
{
    return m_isInitialized;
}

QDialog *ElevationProfileFloatItem::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Elevation Profile Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010, 2011 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author esser;
        esser.name = QString::fromUtf8 ( "Florian Eßer" );
        esser.task = tr( "Developer" );
        esser.email = "f.esser@rwth-aachen.de";
        authors.append( esser );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}



void ElevationProfileFloatItem::changeViewport( ViewportParams *viewport )
{
    if ( !( viewport->width() == m_viewportWidth && m_isInitialized ) ) {
        m_fontHeight     = QFontMetrics( font() ).ascent();
        setContentSize( QSizeF( viewport->width() / 3, m_eleGraphHeight + m_fontHeight * 2.5 + 3 ) );
        m_leftGraphMargin = QFontMetrics( font() ).width("0000");
        m_eleGraphWidth = contentSize().width() - m_leftGraphMargin;
        m_viewportWidth = viewport->width();
        if ( !m_isInitialized ) {
            setPosition( QPointF( (viewport->width() - contentSize().width()) / 2 , 10.5 ) );
            connect( this, SIGNAL( dataUpdated() ), SLOT( forceRepaint() ) );
        }
        m_isInitialized = true;
        updateData();
    }
    update();
}



void ElevationProfileFloatItem::paintContent( GeoPainter *painter,
        ViewportParams *viewport,
        const QString& renderPos,
        GeoSceneLayer * layer )
{
    // TODO: Cleanup, reduce redundant variables etc.
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setFont( font() );

    m_fontHeight     = QFontMetrics( font() ).ascent();

    if ( ! ( m_routeAvailable && m_isInitialized && m_eleData.size() > 0 ) ) {
        painter->setPen( QColor( Qt::black ) );
        QString text = tr( "Create a route to view its elevation profile." );

        QPoint pos(
            ( contentSize().width() - QFontMetrics( font() ).boundingRect( text ).width() ) / 2,
            ( contentSize().height() + m_fontHeight ) / 2
        );
        painter->drawText( pos, text );
        painter->restore();
        return;
    }
    qreal graphDistance = m_eleData.last().x();
    qreal graphElevation = m_maxElevation;
    int   valueOffsetX = 0;
    int   valueOffsetY = 0;
    int start = 0;
    int end   = m_eleData.count() - 1;
    if ( m_zoomToViewport ) {
        start = m_firstVisiblePoint;
        end   = m_lastVisiblePoint;
        valueOffsetX = (int) m_eleData.value(m_firstVisiblePoint).x();
        graphDistance = m_eleData.value(m_lastVisiblePoint).x()
                        - m_eleData.value(m_firstVisiblePoint).x();

        qreal localMax = 0.0;
        valueOffsetY = m_maxElevation;
        for ( int i = start; i <= end; i++ ) {
            if ( m_eleData.value(i).y() > localMax ) {
                localMax = m_eleData.value(i).y();
            }
            if ( m_eleData.value(i).y() < valueOffsetY ) {
                valueOffsetY = m_eleData.value(i).y();
            }
        }
        graphElevation = localMax - valueOffsetY;
    }


    DistanceUnit distanceUnit;
    distanceUnit = MarbleGlobal::getInstance()->locale()->distanceUnit();

    if ( distanceUnit == MilesFeet ) {
        graphDistance *= KM2MI;
    }

    calcScaleX( graphDistance );
    calcScaleY( graphElevation );

    QString  intervalStr;
    int      lastStringEnds     = 0;
    int      currentStringBegin = 0;

    // draw viewport bounds
    if ( ! m_zoomToViewport
            && ( m_firstVisiblePoint > 0 || m_lastVisiblePoint < m_eleData.size() - 1 ) ) {
        QColor color( Qt::black );
        color.setAlpha( 64 );
        painter->fillRect(
            m_leftGraphMargin
            + m_eleData.value(m_firstVisiblePoint).x() * m_eleGraphWidth / graphDistance,
            0,
            ( m_eleData.value(m_lastVisiblePoint).x()
              - m_eleData.value(m_firstVisiblePoint).x() ) * m_eleGraphWidth / graphDistance,
            m_eleGraphHeight,
            color
        );
    }

    // draw Y grid and labels
    for ( int j = 0; j <= m_bestDivisorY; j ++ ) {
        if ( distanceUnit == Meter ) {
            if ( valueOffsetY + m_bestDivisorY * m_valueIntervalY > 10000 ) {
                m_unitY = tr("km");
                intervalStr.setNum( ( valueOffsetY + j * m_valueIntervalY ) / 1000 );
            } else {
                m_unitY = tr("m");
                intervalStr.setNum( valueOffsetY + j * m_valueIntervalY );
            }
        } else {
            // TODO: ft instead of mi for the elevation?
            m_unitY = tr("mi");
            if ( m_bestDivisorY * m_valueIntervalY > 3800 ) {
                intervalStr.setNum( ( valueOffsetY + j * m_valueIntervalY ) / 1000 );
            } else {
                intervalStr.setNum( ( valueOffsetY + j * m_valueIntervalY ) / 1000.0, 'f', 2 );
            }
        }

        currentStringBegin = m_eleGraphHeight - j * m_pixelIntervalY + m_fontHeight / 2;
        if ( j == m_bestDivisorY ) {
            intervalStr += " " + m_unitY;
            if ( currentStringBegin < m_fontHeight + 1 ) {
                currentStringBegin  = m_fontHeight + 1;
            }
        }
        painter->setPen(   QColor( Qt::darkGray ) );
        painter->drawLine( m_leftGraphMargin, m_eleGraphHeight - j * m_pixelIntervalY,
                           contentSize().width(), m_eleGraphHeight - j * m_pixelIntervalY );
        painter->setPen(   QColor( Qt::black ) );
        painter->drawText( 0, currentStringBegin, intervalStr );
    }

    // draw X grid and labels
    // TODO: Nicer interval ticks when zoomed in, e.g. 42.5 instead of 42.444
    painter->setPen( QColor( Qt::black ) );
    for ( int j = 0; j <= m_bestDivisorX; j ++ ) {
        if ( distanceUnit == Meter ) {
            if ( valueOffsetX + m_bestDivisorX * m_valueIntervalX > 10000 ) {
                m_unitX = tr("km");
                intervalStr.setNum( ( valueOffsetX + j * m_valueIntervalX ) / 1000 );
            } else {
                m_unitX = tr("m");
                intervalStr.setNum( valueOffsetX + j * m_valueIntervalX );
            }
        } else {
            m_unitX = tr("mi");
            if ( m_bestDivisorX * m_valueIntervalX > 3800 ) {
                intervalStr.setNum( ( valueOffsetX +  j * m_valueIntervalX ) / 1000 );
            } else {
                intervalStr.setNum( ( valueOffsetX + j * m_valueIntervalX ) / 1000.0, 'f', 2 );
            }
        }

        if ( j == m_bestDivisorX ) {
            if ( valueOffsetX == 0) {
                intervalStr += " " + m_unitX;
            }
            currentStringBegin = (m_leftGraphMargin + m_eleGraphWidth
                                  - QFontMetrics( font() ).width( intervalStr ) * 1.5);
        } else {
            currentStringBegin = (m_leftGraphMargin + j * m_pixelIntervalX
                                  - QFontMetrics( font() ).width( intervalStr ) / 2 );
        }

        if ( lastStringEnds < currentStringBegin ) {
            painter->setPen( QColor( Qt::darkGray ) );
            painter->drawLine(m_leftGraphMargin + j * m_pixelIntervalX, 0,
                              m_leftGraphMargin + j * m_pixelIntervalX, m_eleGraphHeight );
            painter->setPen( QColor( Qt::black ) );
            painter->drawText( currentStringBegin, contentSize().height() - 1.5 * m_fontHeight, intervalStr );
            lastStringEnds = currentStringBegin + QFontMetrics( font() ).width( intervalStr );
        }
    }

    // display elevation gain/loss data
    // TODO: miles/feet...
    painter->setPen( QColor( Qt::black ) );
    intervalStr = tr( "Elevation difference: ca. %1 m (Gain: %2 m, Loss: %3 m)" )
                    .arg( QString::number( m_gain - m_loss, 'f', 0 ) )
                    .arg( QString::number( m_gain, 'f', 0 ) )
                    .arg( QString::number( m_loss, 'f', 0 ) );
    painter->drawText( padding(), contentSize().height() - padding(), intervalStr );

    // draw elevation profile
    painter->setPen( QColor( Qt::black ) );
    QPoint oldPos (
        m_leftGraphMargin,
        m_eleGraphHeight - ( m_eleData.value(start).y() - valueOffsetY )
        * m_eleGraphHeight / graphElevation
    );
    for ( int i = start; i <= end; ++i ) {
        QPoint newPos (
            m_leftGraphMargin + ( m_eleData.value(i).x() - valueOffsetX )
            * m_eleGraphWidth / graphDistance,
            m_eleGraphHeight - ( m_eleData.value(i).y() - valueOffsetY )
            * m_eleGraphHeight / graphElevation
        );
        if ( newPos.x() != oldPos.x() ) {
            painter->drawLine(oldPos.x(), oldPos.y(), newPos.x(), newPos.y());
            oldPos = newPos;
        }
    }

    // draw interactive cursor
    if ( m_mouseInWidget ) {
        painter->setPen( QColor( Qt::white ) );
        painter->drawLine( m_leftGraphMargin + m_cursorPositionX, 0,
                           m_leftGraphMargin + m_cursorPositionX, m_eleGraphHeight );
        qreal xpos = valueOffsetX + ( m_cursorPositionX / m_eleGraphWidth ) * graphDistance;
        qreal ypos = 0;
        GeoDataCoordinates currentPoint;
        for ( int i = start; i < end; ++i) {
            ypos = m_eleData.value(i).y();
            if ( m_eleData.value(i).x() >= xpos ) {
                currentPoint = m_points[i];
                currentPoint.setAltitude( m_eleData.value(i).y() );
                break;
            }
        }
        ypos = ( ( ypos - valueOffsetY ) / graphElevation ) * m_eleGraphHeight;
        ypos = m_eleGraphHeight - ypos;

        painter->drawLine( m_leftGraphMargin + m_cursorPositionX - 5, ypos,
                           m_leftGraphMargin + m_cursorPositionX + 5, ypos );
        if ( distanceUnit == Meter ) {
            m_unitX = tr("m");
            if ( xpos > 10000 ) {
                m_unitX = tr("km");
                xpos /= 1000;
            }
        } else { // miles
            m_unitX = tr("mi");
            xpos /= 1000;
        }
        intervalStr.setNum( xpos, 'f', 2 );
        intervalStr += " " + m_unitX;
        currentStringBegin = m_leftGraphMargin + m_cursorPositionX
                             - QFontMetrics( font() ).width( intervalStr ) / 2;
        painter->drawText( currentStringBegin, contentSize().height() - 1.5 * m_fontHeight, intervalStr );

        intervalStr.setNum( currentPoint.altitude(), 'f', 1 );
        if ( m_cursorPositionX + QFontMetrics( font() ).width( intervalStr ) + m_leftGraphMargin
                < m_eleGraphWidth ) {
            currentStringBegin = ( m_leftGraphMargin + m_cursorPositionX + 5 + 2 );
        } else {
            currentStringBegin = m_leftGraphMargin + m_cursorPositionX - 5
                                 - QFontMetrics( font() ).width( intervalStr ) * 1.5;
        }
        // Make sure the text still fits into the window
        while ( ypos < m_fontHeight ) {
            ypos++;
        }
        painter->drawText( currentStringBegin, ypos + m_fontHeight / 2, intervalStr );


        // mark position on the map
        m_markerIconContainer.show();
        m_markerTextContainer.show();
        QRegion newMarkerRegion;
        qreal x;
        qreal y;
        qreal lon;
        qreal lat;
        // move the icon by some pixels, so that the pole of the flag sits at the exact point
        int dx = -4;
        int dy = -6;
        m_marbleWidget->screenCoordinates( currentPoint.longitude( Marble::GeoDataCoordinates::Degree ),
                                           currentPoint.latitude ( Marble::GeoDataCoordinates::Degree ),
                                           x, y );
        m_marbleWidget->geoCoordinates( x + dx, y + dy, lon, lat, Marble::GeoDataCoordinates::Degree );
        m_markerIconContainer.setCoordinate( GeoDataCoordinates( lon, lat, currentPoint.altitude(),
                                                            Marble::GeoDataCoordinates::Degree ) );
        // move the text label, so that it sits right next to the flag
        dx = 6;
        dy = -16;
        m_marbleWidget->geoCoordinates( x + dx, y + dy, lon, lat, Marble::GeoDataCoordinates::Degree );
        m_markerTextContainer.setCoordinate( GeoDataCoordinates( lon, lat, currentPoint.altitude(),
                                                            Marble::GeoDataCoordinates::Degree ) );
        m_markerText->setText( " " + intervalStr + " " + m_unitY );

        // drawing area of flag
        newMarkerRegion += QRect( QPoint( x - m_markerIconContainer.size().width() - 1,
                                          y - m_markerIconContainer.size().height() - 1 ),
                                  m_markerIconContainer.size().toSize() + QSize( 2, 2 ) );
        // drawing area of text
        newMarkerRegion += QRect( QPoint( x - 3,
                                          y - m_markerText->contentSize().height() - 3 ),
                                  m_markerText->contentSize().toSize() + QSize( 10, 6 ) );
        // redraw
        if ( newMarkerRegion != m_lastMarkerRegion ) {
            repaintRegion( m_lastMarkerRegion + newMarkerRegion );
        }
        m_lastMarkerRegion = newMarkerRegion;
    } else {
        if( m_markerIconContainer.visible() || m_markerIconContainer.visible() ) {
            m_markerIconContainer.hide();
            m_markerTextContainer.hide();
            repaintRegion( m_lastMarkerRegion );
        }
    }
    painter->restore();
}



bool ElevationProfileFloatItem::renderOnMap(GeoPainter* painter, ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer)
{
    if ( renderPos == "HOVERS_ABOVE_SURFACE" ) {
        m_markerIconContainer.paintEvent( painter, viewport, renderPos, layer );
        m_markerTextContainer.paintEvent( painter, viewport, renderPos, layer );
    }
    return true;
}



// see MapScaleFloatItem::calcScaleBar()
void ElevationProfileFloatItem::calcScaleX( const qreal distance )
{
    qreal magnitude = 1;

    // First we calculate the exact length of the whole area that is possibly
    // available to the scalebar in kilometers
    int  magValue = (int)( distance );

    // We calculate the two most significant digits of the km-scalebar-length
    // and store them in magValue.
    while ( magValue >= 100 ) {
        magValue  /= 10;
        magnitude *= 10;
    }

    const int minDivisor = 4;
    const int maxDivisor = 8;

    m_bestDivisorX = 4;
    int  bestMagValue = 1;

    for ( int i = 0; i < magValue; i++ ) {
        // We try to find the lowest divisor between minDivisor and maxDivisor that
        // divides magValue without remainder.
        for ( int j = minDivisor; j < maxDivisor + 1; j++ ) {
            if ( ( magValue - i ) % j == 0 ) {
                // We store the very first result we find and store
                // m_bestDivisor and bestMagValue as a final result.
                m_bestDivisorX = j;
                bestMagValue  = magValue - i;

                // Stop all for loops and end search
                i = magValue;
                j = maxDivisor + 1;
            }
        }

        // If magValue doesn't divide through values between minDivisor and maxDivisor
        // (e.g. because it's a prime number) try again with magValue
        // decreased by i.
    }

    m_pixelIntervalX = (int)( m_eleGraphWidth * (qreal)( bestMagValue )
                              / (qreal)( magValue ) / m_bestDivisorX );
    m_valueIntervalX = (int)( bestMagValue * magnitude / m_bestDivisorX );
}



// see MapScaleFloatItem::calcScaleBar()
void ElevationProfileFloatItem::calcScaleY( const qreal distance )
{
    qreal magnitude = 1;

    // First we calculate the exact length of the whole area that is possibly
    // available to the scalebar in kilometers
    int  magValue = (int)( distance );

    // We calculate the two most significant digits of the km-scalebar-length
    // and store them in magValue.
    while ( magValue >= 100 ) {
        magValue  /= 10;
        magnitude *= 10;
    }

    const int minDivisor = 4;
    const int maxDivisor = m_eleGraphHeight / ( m_fontHeight * 1.5 );

    m_bestDivisorY = minDivisor;
    int  bestMagValue = 1;

    for ( int i = 0; i < magValue; i++ ) {
        // We try to find the lowest divisor between minDivisor and maxDivisor that
        // divides magValue without remainder.
        for ( int j = minDivisor; j <= maxDivisor; j++ ) {
            if ( ( magValue - i ) % j == 0 ) {
                // We store the very first result we find and store
                // m_bestDivisor and bestMagValue as a final result.
                m_bestDivisorY = j;
                bestMagValue  = magValue - i;

                // Stop all for loops and end search
                i = magValue;
                j = maxDivisor + 1;
            }
        }

        // If magValue doesn't divide through values between minDivisor and maxDivisor
        // (e.g. because it's a prime number) try again with magValue
        // decreased by i.
    }

    m_pixelIntervalY = (int)( m_eleGraphHeight * (qreal)( bestMagValue )
                              / (qreal)( magValue ) / m_bestDivisorY );
    m_valueIntervalY = (int)( bestMagValue * magnitude / m_bestDivisorY );
}



QDialog *ElevationProfileFloatItem::configDialog() //TODO
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::ElevationProfileConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        readSettings();

        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ), SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ), SLOT( readSettings() ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ), this, SLOT( writeSettings() ) );
    }
    return m_configDialog;
}



void ElevationProfileFloatItem::contextMenuEvent( QWidget *w, QContextMenuEvent *e )
{
    if ( !m_contextMenu ) {
        m_contextMenu = contextMenu();

        foreach( QAction *action, m_contextMenu->actions() ) {
            if ( action->text() == tr( "&Configure..." ) ) {
                m_contextMenu->removeAction( action );
                break;
            }
        }

        QAction *toggleAction = m_contextMenu->addAction( tr("&Zoom to viewport"), this,
                                SLOT( toggleZoomToViewport() ) );
        toggleAction->setCheckable( true );
        toggleAction->setChecked( m_zoomToViewport );
    }

    Q_ASSERT( m_contextMenu );
    m_contextMenu->exec( w->mapToGlobal( e->pos() ) );
}



bool ElevationProfileFloatItem::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>( object );
    if ( !widget ) {
        return AbstractFloatItem::eventFilter(object,e);
    }

    if ( widget && !m_marbleWidget ) {
        m_marbleWidget = widget;
        m_routingModel = m_marbleWidget->model()->routingManager()->routingModel();
        m_routingLayer = m_marbleWidget->routingLayer();
        connect( m_routingModel, SIGNAL( currentRouteChanged() ), this, SLOT( updateData() ) );
        connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
                 this, SLOT( updateVisiblePoints() ) );
        connect( this, SIGNAL( dataUpdated() ), this, SLOT( updateVisiblePoints() ) );
        updateData();
    }

    bool cursorAboveFloatItem(false);
    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF graphRect = QRectF (m_leftGraphMargin, 0, m_eleGraphWidth, contentSize().height());
        graphRect.translate(positivePosition());
        graphRect.translate(padding(), padding());

        // for antialiasing: increase size by 1 px to each side
        graphRect.translate(-1, -1);
        graphRect.setSize(graphRect.size() + QSize(2, 2) );

        if ( graphRect.contains(event->pos()) ) {
            cursorAboveFloatItem = true;

            // Double click triggers recentering the map at the specified position
            if ( e->type() == QEvent::MouseButtonDblClick && !m_zoomToViewport ) {
                // TODO: do the math for m_zoomToViewport == true
                QRectF mapRect( contentRect() );
                QPointF pos = event->pos() - graphRect.topLeft();
                GeoDataLineString points = m_routingModel->route().path();
                int i = pos.x() / m_eleGraphWidth * ( points.size() - 1 );
                if ( i >= points.size() ) {
                    i = points.size() - 1;
                }
                /*
                qreal lon = points[i].longitude( GeoDataCoordinates::Degree );
                qreal lat = points[i].latitude ( GeoDataCoordinates::Degree );
                widget->centerOn( lon, lat, true );
                */
                // The above would be more easy, but then Marble zooms in on every double click.
                // (see MarbleWidgetDefaultInputHandler::eventFilter)
                // This is not very convenient if you are "browsing" your route via the elevation
                // profile.
                // So, instead we use the following code and undo marble's zooming:

                qreal lon = points[i].longitude();
                qreal lat = points[i].latitude ();
                GeoDataLookAt lookAt;
                lookAt.setLongitude( lon );
                lookAt.setLatitude( lat );
                lookAt.setAltitude( 0.0 );
                // see MarbleWidgetDefaultInputHandler::eventFilter's MouseButtonDblClick routine
                // for the "magic" 0.67 to undo zooming
                lookAt.setRange( widget->distance() * 1/0.67 * KM2METER );

                widget->flyTo( lookAt );

                return true;
            }
        }

        if ( ( cursorAboveFloatItem && e->type() == QEvent::MouseMove
                && !event->buttons() & Qt::LeftButton )
                || m_mouseInWidget != cursorAboveFloatItem
           )
        {
            // Cross hair cursor when moving above the float item
            // and mark the position on the graph
            widget->setCursor(QCursor(Qt::CrossCursor));
            if ( m_cursorPositionX != event->pos().x() - graphRect.left() ) {
                m_cursorPositionX = event->pos().x() - graphRect.left();
                m_mouseInWidget = cursorAboveFloatItem;
                forceRepaint();
            }

            return true;
        }
    }

    return AbstractFloatItem::eventFilter(object,e);
}



void ElevationProfileFloatItem::updateData()
{
    m_routeAvailable = m_routingModel && m_routingModel->rowCount() > 0;
    m_points = m_routeAvailable ? m_routingModel->route().path() : GeoDataLineString();
    m_eleData = calculateElevationData( m_points );
    calculateStatistics( m_eleData );
    emit dataUpdated();

    forceRepaint();
}



void ElevationProfileFloatItem::updateVisiblePoints()
{
    if ( ! ( m_routeAvailable && m_routingModel ) ) {
        return;
    }
    GeoDataLineString points = m_routingModel->route().path();
    if ( points.size() < 2 ) {
        return;
    }

    // find the longest visible route section on screen
    QList<QList<int> > routeSegments;
    QList<int> currentRouteSegment;
    for ( int i = 0; i < m_eleData.count(); i++ ) {
        qreal lon = points[i].longitude(GeoDataCoordinates::Degree);
        qreal lat = points[i].latitude (GeoDataCoordinates::Degree);
        qreal x = 0;
        qreal y = 0;

        if ( m_marbleWidget->screenCoordinates(lon, lat, x, y) ) {
            // on screen --> add point to list
            currentRouteSegment.append(i);
        } else {
            // off screen --> start new list
            if ( !currentRouteSegment.isEmpty() ) {
                routeSegments.append( currentRouteSegment );
                currentRouteSegment.empty();
            }
        }
    }
    routeSegments.append( currentRouteSegment ); // in case the route ends pn screen

    int maxLenght = 0;
    foreach ( currentRouteSegment, routeSegments ) {
        if ( currentRouteSegment.size() > maxLenght ) {
            maxLenght = currentRouteSegment.size() ;
            m_firstVisiblePoint = currentRouteSegment.first();
            m_lastVisiblePoint  = currentRouteSegment.last();
        }
    }
    if ( m_firstVisiblePoint < 0 ) {
        m_firstVisiblePoint = 0;
    }
    if ( m_lastVisiblePoint < 0 || m_lastVisiblePoint >= m_eleData.count() ) {
        m_lastVisiblePoint = m_eleData.count() - 1;
    }
    return;
}



QList<QPointF> ElevationProfileFloatItem::calculateElevationData( const GeoDataLineString &lineString ) const
{
    // TODO: Don't re-calculate the whole route if only a small part of it was changed
    QList<QPointF> result;

    GeoDataLineString path;
    for ( int i = 0; i < lineString.size(); i++ ) {
        path.append( lineString[i] );

        const qreal lat = lineString[i].latitude ( GeoDataCoordinates::Degree );
        const qreal lon = lineString[i].longitude( GeoDataCoordinates::Degree );
        qreal ele = marbleModel()->elevationModel()->height( lon, lat );
        if ( ele == 32768 ) { // no data
            ele = 0;
        }
        result.append( QPointF( path.length( EARTH_RADIUS ), ele ) );
    }

    return result;
}



void ElevationProfileFloatItem::calculateStatistics( const QList<QPointF> &eleData )
{
    const int averageOrder = 5;

    qreal lastAverage = 0;
    m_maxElevation = -1;
    m_gain = 0;
    m_loss = 0;
    for ( int i = 0; i < eleData.size(); i++ ) {
        m_maxElevation = qMax( m_maxElevation, eleData.value(i).y() );

        // Low-pass filtering (moving average) of the elevation profile to calculate gain and loss values
        // not always the best method, see for example
        // http://www.ikg.uni-hannover.de/fileadmin/ikg/staff/thesis/finished/documents/StudArb_Schulze.pdf
        // (German), chapter 4.2

        if ( i >= averageOrder ) {
            qreal average = 0;
            for( int j = 0; j < averageOrder; j++ ) {
                average += eleData.value(i-j).y();
            }
            average /= averageOrder;
            if ( i == averageOrder ) {
                lastAverage = average; // else the initial elevation would be counted as gain
            }
            if ( average > lastAverage ) {
                m_gain += average - lastAverage;
            } else {
                m_loss += lastAverage - average;
            }
            lastAverage = average;
        }
    }
}



void ElevationProfileFloatItem::forceRepaint()
{
    // We add one pixel as antialiasing could result into painting on these pixels to.
    QRectF floatItemRect = QRectF( positivePosition() - QPoint( 1, 1 ),
                                   size() + QSize( 2, 2 ) );
    repaintRegion( floatItemRect.toRect() );
    update();
}



void ElevationProfileFloatItem::repaintRegion( QRegion dirtyRegion )
{
    if ( m_marbleWidget ) {
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground,  false );
        m_marbleWidget->update(dirtyRegion);
        m_marbleWidget->setAttribute( Qt::WA_NoSystemBackground,
                                      m_marbleWidget->viewport()->mapCoversViewport() );
    }
}



void ElevationProfileFloatItem::readSettings()
{
    if ( !m_configDialog )
        return;

    if ( m_zoomToViewport ) {
        ui_configWidget->m_zoomToViewportCheckBox->setCheckState( Qt::Checked );
    }
    else {
        ui_configWidget->m_zoomToViewportCheckBox->setCheckState( Qt::Unchecked );
    }
}

void ElevationProfileFloatItem::writeSettings()
{
    if ( ui_configWidget->m_zoomToViewportCheckBox->checkState() == Qt::Checked ) {
        m_zoomToViewport = true;
    } else {
        m_zoomToViewport = false;
    }

    emit settingsChanged( nameId() );
}

void ElevationProfileFloatItem::toggleZoomToViewport()
{
    m_zoomToViewport = !m_zoomToViewport;
    readSettings();
    emit settingsChanged( nameId() );
}


}

Q_EXPORT_PLUGIN2(ElevationProfileFloatItem, Marble::ElevationProfileFloatItem)

#include "ElevationProfileFloatItem.moc"
