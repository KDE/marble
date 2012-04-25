//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#include "ElevationProfileFloatItem.h"

#include <QtCore/QRect>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

#include "ui_ElevationProfileConfigWidget.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "routing/RoutingModel.h"
#include "routing/RoutingManager.h"
#include "MarbleDirs.h"
#include "ElevationModel.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleMath.h"

namespace Marble
{

ElevationProfileFloatItem::ElevationProfileFloatItem()
    : AbstractFloatItem( 0 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 )
{
}

ElevationProfileFloatItem::ElevationProfileFloatItem( const MarbleModel *marbleModel )
        : AbstractFloatItem( marbleModel, QPointF( 220, 10.5 ), QSizeF( 0.0, 50.0 ) ),
        m_configDialog( 0 ),
        ui_configWidget( 0 ),
        m_leftGraphMargin( 0 ),
        m_eleGraphWidth( 0 ),
        m_viewportWidth( 0 ),
        m_shrinkFactorY( 1.2 ),
        m_fontHeight( 10 ),
        m_currentPoint(),
        m_cursorPositionX( 0 ),
        m_isInitialized( false ),
        m_contextMenu( 0 ),
        m_marbleWidget( 0 ),
        m_routingModel( 0 ),
        m_routeAvailable( false ),
        m_firstVisiblePoint( 0 ),
        m_lastVisiblePoint( 0 ),
        m_zoomToViewport( false ),
        m_markerIconContainer(),
        m_markerTextContainer(),
        m_markerIcon( &m_markerIconContainer ),
        m_markerText( &m_markerTextContainer )
{
    setVisible( false );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        setPosition( QPointF( 10.5, 10.5 ) );
    }
    bool const highRes = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::HighResolution;
    m_eleGraphHeight = highRes ? 100 : 50;

    setPadding( 1 );
    m_markerIcon.setImage( QImage( ":/flag-red-mirrored.png" ) );

    MarbleGraphicsGridLayout *topLayout1 = new MarbleGraphicsGridLayout( 1, 1 );
    m_markerIconContainer.setLayout( topLayout1 );
    topLayout1->addItem( &m_markerIcon, 0, 0 );

    MarbleGraphicsGridLayout *topLayout2 = new MarbleGraphicsGridLayout( 1, 1 );
    m_markerTextContainer.setLayout( topLayout2 );
    m_markerText.setFrame( RoundedRectFrame );
    m_markerText.setPadding( 1 );
    topLayout2->setAlignment( Qt::AlignCenter );
    topLayout2->addItem( &m_markerText, 0, 0 );
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
    return 3.0;
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

QString ElevationProfileFloatItem::version() const
{
    return "1.2";
}

QString ElevationProfileFloatItem::description() const
{
    return tr("This is a float item that provides a route/track elevation profile.");
}

QString ElevationProfileFloatItem::copyrightYears() const
{
    return "2011, 2012";
}

QList<PluginAuthor> ElevationProfileFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8 ( "Florian Eßer" ), "f.esser@rwth-aachen.de" );
}

QIcon ElevationProfileFloatItem::icon () const
{
    return QIcon(":/icons/elevationprofile.png");
}

void ElevationProfileFloatItem::initialize ()
{
    connect( marbleModel()->elevationModel(), SIGNAL( updateAvailable() ), SLOT( updateData() ) );

    m_routingModel = marbleModel()->routingManager()->routingModel();
    connect( m_routingModel, SIGNAL( currentRouteChanged() ), this, SLOT( updateData() ) );

    m_fontHeight = QFontMetricsF( font() ).ascent() + 1;
    m_leftGraphMargin = QFontMetricsF( font() ).width( "0000 m" ); // TODO make this dynamic according to actual need
    connect( this, SIGNAL( dataUpdated() ), SLOT( forceRepaint() ) );

    updateData();
}

bool ElevationProfileFloatItem::isInitialized () const
{
    return m_isInitialized;
}

void ElevationProfileFloatItem::changeViewport( ViewportParams *viewport )
{
    if ( !( viewport->width() == m_viewportWidth && m_isInitialized ) ) {
        bool const highRes = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::HighResolution;
        int const widthRatio = highRes ? 2 : 3;
        setContentSize( QSizeF( viewport->width() / widthRatio,
                                m_eleGraphHeight + m_fontHeight * 2.5 ) );
        m_eleGraphWidth = contentSize().width() - m_leftGraphMargin;
        m_axisX.setLength( m_eleGraphWidth );
        m_axisY.setLength( m_eleGraphHeight );
        m_axisX.setTickCount( 3, m_eleGraphWidth / ( m_leftGraphMargin * 1.5 ) );
        m_axisY.setTickCount( 2, m_eleGraphHeight / m_fontHeight );
        m_viewportWidth = viewport->width();
        bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
        if ( !m_isInitialized && !smallScreen ) {
            setPosition( QPointF( (viewport->width() - contentSize().width()) / 2 , 10.5 ) );
        }
        m_isInitialized = true;
    }

    update();
}

void ElevationProfileFloatItem::paintContent( GeoPainter *painter,
        ViewportParams *viewport,
        const QString& renderPos,
        GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setFont( font() );

    m_fontHeight = QFontMetricsF( font() ).ascent() + 1;

    if ( ! ( m_routeAvailable && m_isInitialized && m_eleData.size() > 0 ) ) {
        painter->setPen( QColor( Qt::black ) );
        QString text = tr( "Create a route to view its elevation profile." );
        painter->drawText( contentRect().toRect(), Qt::TextWordWrap | Qt::AlignCenter, text );
        painter->restore();
        return;
    }
    if ( m_zoomToViewport && ( m_lastVisiblePoint - m_firstVisiblePoint < 5 ) ) {
        painter->setPen( QColor( Qt::black ) );
        QString text = tr( "Not enough points in the current viewport.\nTry to disable 'Zoom to viewport'." );
        painter->drawText( contentRect().toRect(), Qt::TextWordWrap | Qt::AlignCenter, text );
        painter->restore();
        return;
    }

    QString intervalStr;
    int lastStringEnds;

    // draw viewport bounds
    if ( ! m_zoomToViewport && ( m_firstVisiblePoint > 0 || m_lastVisiblePoint < m_eleData.size() - 1 ) ) {
        QColor color( Qt::black );
        color.setAlpha( 64 );
        QRect rect;
        rect.setLeft( m_leftGraphMargin + m_eleData.value( m_firstVisiblePoint ).x() * m_eleGraphWidth / m_axisX.range() );
        rect.setTop( 0 );
        rect.setWidth( ( m_eleData.value( m_lastVisiblePoint ).x() - m_eleData.value( m_firstVisiblePoint ).x() ) * m_eleGraphWidth / m_axisX.range() );
        rect.setHeight( m_eleGraphHeight );
        painter->fillRect( rect, color );
    }

    // draw X and Y axis
    painter->setPen( oxygenAluminumGray4 );
    painter->drawLine( m_leftGraphMargin, m_eleGraphHeight, contentSize().width(), m_eleGraphHeight );
    painter->drawLine( m_leftGraphMargin, m_eleGraphHeight, m_leftGraphMargin, 0 );

    // draw Y grid and labels
    painter->setPen( QColor( Qt::black ) );
    QPen dashedPen( Qt::DashLine );
    dashedPen.setColor( oxygenAluminumGray4 );
    QRect labelRect( 0, 0, m_leftGraphMargin - 1, m_fontHeight + 2 );
    lastStringEnds = m_eleGraphHeight + m_fontHeight;
//     painter->drawText( m_leftGraphMargin + 1, m_fontHeight, "[" + m_axisY.unit() + "]" );
    foreach ( const AxisTick &tick, m_axisY.ticks() ) {
        const int posY = m_eleGraphHeight - tick.position;
        painter->setPen( dashedPen );
        painter->drawLine( m_leftGraphMargin, posY, contentSize().width(), posY );

        labelRect.moveCenter( QPoint( labelRect.center().x(), posY ) );
        if ( labelRect.top() < 0 ) {
            // don't cut off uppermost label
            labelRect.moveTop( 0 );
        }
        if ( labelRect.bottom() >= lastStringEnds ) {
            // Don't print overlapping labels
            continue;
        }
        lastStringEnds = labelRect.top();
        painter->setPen( QColor( Qt::black ) );
        intervalStr.setNum( tick.value * m_axisY.scale() );
        painter->drawText( labelRect, Qt::AlignRight, intervalStr );
    }

    // draw X grid and labels
    painter->setPen( QColor( Qt::black ) );
    labelRect.moveTop( m_eleGraphHeight + 1 );
    lastStringEnds = 0;
    foreach ( const AxisTick &tick, m_axisX.ticks() ) {
        const int posX = m_leftGraphMargin + tick.position;
        painter->setPen( dashedPen );
        painter->drawLine( posX, 0, posX, m_eleGraphHeight );

        intervalStr.setNum( tick.value * m_axisX.scale() );
        if ( tick.position == m_axisX.ticks().last().position ) {
            intervalStr += " " + m_axisX.unit();
        }
        labelRect.setWidth( QFontMetricsF( font() ).width( intervalStr ) * 1.5 );
        labelRect.moveCenter( QPoint( posX, labelRect.center().y() ) );
        if ( labelRect.right() > m_leftGraphMargin + m_eleGraphWidth ) {
            // don't cut off rightmost label
            labelRect.moveRight( m_leftGraphMargin + m_eleGraphWidth );
        }
        if ( labelRect.left() <= lastStringEnds ) {
            // Don't print overlapping labels
            continue;
        }
        lastStringEnds = labelRect.right();
        painter->setPen( QColor( Qt::black ) );
        painter->drawText( labelRect, Qt::AlignCenter, intervalStr );
    }

    // display elevation gain/loss data
    painter->setPen( QColor( Qt::black ) );
    intervalStr = tr( "Difference: %1 %2" )
                   .arg( QString::number( m_gain - m_loss, 'f', 0 ) )
                   .arg( m_axisY.unit() );
    intervalStr += QString::fromUtf8( "  (↗ %1 %3  ↘ %2 %3)" )
                   .arg( QString::number( m_gain, 'f', 0 ) )
                   .arg( QString::number( m_loss, 'f', 0 ) )
                   .arg( m_axisY.unit() );
    painter->drawText( contentRect().toRect(), Qt::AlignBottom | Qt::AlignCenter, intervalStr );

    // draw elevation profile
    painter->setPen( QColor( Qt::black ) );
    bool const highRes = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::HighResolution;
    QPen pen = painter->pen();
    pen.setWidth( highRes ? 2 : 1 );
    painter->setPen( pen );

    QLinearGradient fillGradient( 0, 0, 0, m_eleGraphHeight );
    QColor startColor = oxygenForestGreen4;
    QColor endColor = oxygenBrownOrange4;
    startColor.setAlpha( 200 );
    endColor.setAlpha( 32 );
    fillGradient.setColorAt( 0.0, startColor );
    fillGradient.setColorAt( 1.0, endColor );
    QBrush brush = QBrush( fillGradient );
    painter->setBrush( brush );

    QPoint oldPos;
    oldPos.setX( m_leftGraphMargin );
    oldPos.setY( ( m_axisY.minValue() - m_axisY.minValue() )
                 * m_eleGraphHeight / ( m_axisY.range() / m_shrinkFactorY ) );
    oldPos.setY( m_eleGraphHeight - oldPos.y() );
    QPainterPath path;
    path.moveTo( oldPos.x(), m_eleGraphHeight );
    path.lineTo( oldPos.x(), oldPos.y() );

    const int start = m_zoomToViewport ? m_firstVisiblePoint : 0;
    const int end = m_zoomToViewport ? m_lastVisiblePoint : m_eleData.size() - 1;
    for ( int i = start; i <= end; ++i ) {
        QPoint newPos;
        if ( i == start ) {
            // make sure the plot always starts at the y-axis
            newPos.setX( 0 );
        } else {
            newPos.setX( ( m_eleData.value(i).x() - m_axisX.minValue() ) * m_eleGraphWidth / m_axisX.range() );
        }
        newPos.rx() += m_leftGraphMargin;
        if ( newPos.x() != oldPos.x() || newPos.y() != oldPos.y()  ) {
            newPos.setY( ( m_eleData.value(i).y() - m_axisY.minValue() )
                         * m_eleGraphHeight / ( m_axisY.range() * m_shrinkFactorY ) );
            newPos.setY( m_eleGraphHeight - newPos.y() );
            path.lineTo( newPos.x(), newPos.y() );
            oldPos = newPos;
        }
    }
    path.lineTo( oldPos.x(), m_eleGraphHeight );
    // fill
    painter->setPen( QPen( Qt::NoPen ) );
    painter->drawPath( path );
    // contour
    // "remove" the first and last path element first, they are only used to fill down to the bottom
    painter->setBrush( QBrush( Qt::NoBrush ) );
    path.setElementPositionAt( 0, path.elementAt( 1 ).x,  path.elementAt( 1 ).y );
    path.setElementPositionAt( path.elementCount()-1,
                               path.elementAt( path.elementCount()-2 ).x,
                               path.elementAt( path.elementCount()-2 ).y );
    painter->setPen( pen );
    painter->drawPath( path );

    pen.setWidth( 1 );
    painter->setPen( pen );

    // draw interactive cursor
    if ( m_currentPoint.isValid() ) {
        painter->setPen( QColor( Qt::white ) );
        painter->drawLine( m_leftGraphMargin + m_cursorPositionX, 0,
                           m_leftGraphMargin + m_cursorPositionX, m_eleGraphHeight );
        qreal xpos = m_axisX.minValue() + ( m_cursorPositionX / m_eleGraphWidth ) * m_axisX.range();
        qreal ypos = m_eleGraphHeight - ( ( m_currentPoint.altitude() - m_axisY.minValue() ) / ( m_axisY.range() * m_shrinkFactorY ) ) * m_eleGraphHeight;

        painter->drawLine( m_leftGraphMargin + m_cursorPositionX - 5, ypos,
                           m_leftGraphMargin + m_cursorPositionX + 5, ypos );
        intervalStr.setNum( xpos * m_axisX.scale(), 'f', 2 );
        intervalStr += " " + m_axisX.unit();
        int currentStringBegin = m_leftGraphMargin + m_cursorPositionX
                             - QFontMetricsF( font() ).width( intervalStr ) / 2;
        painter->drawText( currentStringBegin, contentSize().height() - 1.5 * m_fontHeight, intervalStr );

        intervalStr.setNum( m_currentPoint.altitude(), 'f', 1 );
        intervalStr += " " + m_axisY.unit();
        if ( m_cursorPositionX + QFontMetricsF( font() ).width( intervalStr ) + m_leftGraphMargin
                < m_eleGraphWidth ) {
            currentStringBegin = ( m_leftGraphMargin + m_cursorPositionX + 5 + 2 );
        } else {
            currentStringBegin = m_leftGraphMargin + m_cursorPositionX - 5
                                 - QFontMetricsF( font() ).width( intervalStr ) * 1.5;
        }
        // Make sure the text still fits into the window
        while ( ypos < m_fontHeight ) {
            ypos++;
        }
        painter->drawText( currentStringBegin, ypos + m_fontHeight / 2, intervalStr );
    }

    painter->restore();
}

bool ElevationProfileFloatItem::renderOnMap(GeoPainter* painter, ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer)
{
    if ( renderPos != "HOVERS_ABOVE_SURFACE" )
        return false;

    if ( m_currentPoint.isValid() ) {
        // mark position on the map
        qreal x;
        qreal y;
        qreal lon;
        qreal lat;
        // move the icon by some pixels, so that the pole of the flag sits at the exact point
        int dx = -4;
        int dy = -6;
        viewport->screenCoordinates( m_currentPoint.longitude( Marble::GeoDataCoordinates::Radian ),
                                     m_currentPoint.latitude ( Marble::GeoDataCoordinates::Radian ),
                                           x, y );
        viewport->geoCoordinates( x + dx, y + dy, lon, lat, Marble::GeoDataCoordinates::Radian );
        m_markerIconContainer.setCoordinate( GeoDataCoordinates( lon, lat, m_currentPoint.altitude(),
                                                            Marble::GeoDataCoordinates::Radian ) );
        // move the text label, so that it sits next to the flag with a small spacing
        dx += m_markerIconContainer.size().width() / 2 + m_markerTextContainer.size().width() / 2 + 2;
        viewport->geoCoordinates( x + dx, y + dy, lon, lat, Marble::GeoDataCoordinates::Radian );
        m_markerTextContainer.setCoordinate( GeoDataCoordinates( lon, lat, m_currentPoint.altitude(),
                                                            Marble::GeoDataCoordinates::Radian ) );

        QString intervalStr;
        intervalStr.setNum( m_currentPoint.altitude(), 'f', 1 );
        intervalStr += " " + m_axisY.unit();
        m_markerText.setText( intervalStr );

        // drawing area of flag
        m_markerIconContainer.paintEvent( painter, viewport, renderPos, layer );
        m_markerTextContainer.paintEvent( painter, viewport, renderPos, layer );
    }

    return true;
}

QDialog *ElevationProfileFloatItem::configDialog() //FIXME TODO Make a config dialog?
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
        connect( this, SIGNAL( dataUpdated() ), this, SLOT( updateVisiblePoints() ) );
        connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
                 this, SLOT( updateVisiblePoints() ) );
        connect( this, SIGNAL( settingsChanged( QString ) ), this, SLOT( updateVisiblePoints() ) );
    }

    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>( e  );
        QRectF plotRect = QRectF ( m_leftGraphMargin, 0, m_eleGraphWidth, contentSize().height() );
        plotRect.translate( positivePosition() );
        plotRect.translate( padding(), padding() );

        // for antialiasing: increase size by 1 px to each side
        plotRect.translate(-1, -1);
        plotRect.setSize(plotRect.size() + QSize(2, 2) );

        const bool cursorAboveFloatItem = plotRect.contains(event->pos());

        if ( cursorAboveFloatItem ) {
            const int start = m_zoomToViewport ? m_firstVisiblePoint : 0;
            const int end = m_zoomToViewport ? m_lastVisiblePoint : m_eleData.size();

            // Double click triggers recentering the map at the specified position
            if ( e->type() == QEvent::MouseButtonDblClick ) {
                const QPointF mousePosition = event->pos() - plotRect.topLeft();
                const int xPos = mousePosition.x();
                for ( int i = start; i < end; ++i) {
                    const int plotPos = ( m_eleData.value(i).x() - m_axisX.minValue() ) * m_eleGraphWidth / m_axisX.range();
                    if ( plotPos >= xPos ) {
                        widget->centerOn( m_points[i], true );
                        break;
                    }
                }
                return true;
            }

            if ( e->type() == QEvent::MouseMove && !event->buttons() & Qt::LeftButton ) {
                // Cross hair cursor when moving above the float item
                // and mark the position on the graph
                widget->setCursor(QCursor(Qt::CrossCursor));
                if ( m_cursorPositionX != event->pos().x() - plotRect.left() ) {
                    m_cursorPositionX = event->pos().x() - plotRect.left();
                    const qreal xpos = m_axisX.minValue() + ( m_cursorPositionX / m_eleGraphWidth ) * m_axisX.range();
                    m_currentPoint = GeoDataCoordinates();  // set to invalid
                    for ( int i = start; i < end; ++i) {
                        if ( m_eleData.value(i).x() >= xpos ) {
                            m_currentPoint = m_points[i];
                            m_currentPoint.setAltitude( m_eleData.value(i).y() );
                            break;
                        }
                    }
                    emit repaintNeeded();
                }

                return true;
            }
        }
        else {
            if ( m_currentPoint.isValid() ) {
                m_currentPoint = GeoDataCoordinates();  // set to invalid
                emit repaintNeeded();
            }
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
    if ( m_eleData.length() >= 2 ) {
        m_axisX.setRange( m_eleData.first().x(), m_eleData.last().x() );
        m_axisY.setRange( qMin( m_minElevation, qreal( 0.0 ) ), m_maxElevation );
    }
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
    routeSegments.append( currentRouteSegment ); // in case the route ends on screen

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

    if ( m_zoomToViewport ) {
        calculateStatistics( m_eleData );
        m_axisX.setRange( m_eleData.value( m_firstVisiblePoint ).x(),
                          m_eleData.value( m_lastVisiblePoint  ).x() );
        m_axisY.setRange( m_minElevation, m_maxElevation );
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
        if ( ele == invalidElevationData ) { // no data
            ele = 0;
        }

        // result.append( QPointF( path.length( EARTH_RADIUS ), ele ) );
        // The code below does the same as the line above, but is much faster - O(1) instead of O(n)
        if ( i ) {
            Q_ASSERT( !result.isEmpty() ); // The else part below appended something in the first run
            qreal const distance = EARTH_RADIUS * distanceSphere( lineString[i-1], lineString[i] );
            result.append( QPointF( result.last().x() + distance, ele ) );
        } else {
            result.append( QPointF( 0, ele ) );
        }
    }

    return result;
}

void ElevationProfileFloatItem::calculateStatistics( const QList<QPointF> &eleData )
{
    const int averageOrder = 5;

    qreal lastAverage = 0;
    m_maxElevation = 0.0;
    m_minElevation = invalidElevationData;
    m_gain = 0;
    m_loss = 0;
    const int start = m_zoomToViewport ? m_firstVisiblePoint : 0;
    const int end = m_zoomToViewport ? m_lastVisiblePoint : eleData.size();
    for ( int i = start; i < end; ++i ) {
        m_maxElevation = qMax( m_maxElevation, eleData.value( i ).y() );
        m_minElevation = qMin( m_minElevation, eleData.value( i ).y() );

        // Low-pass filtering (moving average) of the elevation profile to calculate gain and loss values
        // not always the best method, see for example
        // http://www.ikg.uni-hannover.de/fileadmin/ikg/staff/thesis/finished/documents/StudArb_Schulze.pdf
        // (German), chapter 4.2

        if ( i >= averageOrder ) {
            qreal average = 0;
            for( int j = 0; j < averageOrder; j++ ) {
                average += eleData.value( i-j ).y();
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
    update();
    emit repaintNeeded( floatItemRect.toRect() );
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
    m_zoomToViewport = ! m_zoomToViewport;
    calculateStatistics( m_eleData );
    if ( ! m_zoomToViewport ) {
        m_axisX.setRange( m_eleData.first().x(), m_eleData.last().x() );
        m_axisY.setRange( qMin( m_minElevation, qreal( 0.0 ) ), m_maxElevation );
    }
    readSettings();
    emit settingsChanged( nameId() );
}


}

Q_EXPORT_PLUGIN2(ElevationProfileFloatItem, Marble::ElevationProfileFloatItem)

#include "ElevationProfileFloatItem.moc"
