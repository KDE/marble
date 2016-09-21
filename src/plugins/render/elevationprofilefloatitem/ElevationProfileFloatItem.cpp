//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013      Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#include "ElevationProfileFloatItem.h"

#include "ElevationProfileContextMenu.h"
#include "ui_ElevationProfileConfigWidget.h"

#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"
#include "ElevationModel.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"

#include <QContextMenuEvent>
#include <QRect>
#include <QPainter>
#include <QPushButton>
#include <QMenu>
#include <QMouseEvent>

namespace Marble
{

ElevationProfileFloatItem::ElevationProfileFloatItem( const MarbleModel *marbleModel )
        : AbstractFloatItem( marbleModel, QPointF( 220, 10.5 ), QSizeF( 0.0, 50.0 ) ),
        m_activeDataSource(0),
        m_routeDataSource( marbleModel ? marbleModel->routingManager()->routingModel() : 0, marbleModel ? marbleModel->elevationModel() : 0, this ),
        m_trackDataSource( marbleModel ? marbleModel->treeModel() : 0, this ),
        m_configDialog( 0 ),
        ui_configWidget( 0 ),
        m_leftGraphMargin( 0 ),
        m_eleGraphWidth( 0 ),
        m_viewportWidth( 0 ),
        m_shrinkFactorY( 1.2 ),
        m_fontHeight( 10 ),
        m_markerPlacemark( new GeoDataPlacemark ),
        m_documentIndex( -1 ),
        m_cursorPositionX( 0 ),
        m_isInitialized( false ),
        m_contextMenu( 0 ),
        m_marbleWidget( 0 ),
        m_firstVisiblePoint( 0 ),
        m_lastVisiblePoint( 0 ),
        m_zoomToViewport( false )
{
    setVisible( false );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        setPosition( QPointF( 10.5, 10.5 ) );
    }
    bool const highRes = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::HighResolution;
    m_eleGraphHeight = highRes ? 100 : 50; /// TODO make configurable

    setPadding( 1 );

    m_markerDocument.setDocumentRole( UnknownDocument );
    m_markerDocument.setName(QStringLiteral("Elevation Profile"));

    m_markerPlacemark->setName(QStringLiteral("Elevation Marker"));
    m_markerPlacemark->setVisible( false );
    m_markerDocument.append( m_markerPlacemark );

    m_contextMenu = new ElevationProfileContextMenu(this);
    connect( &m_trackDataSource, SIGNAL(sourceCountChanged()), m_contextMenu, SLOT(updateContextMenuEntries()) );
    connect( &m_routeDataSource, SIGNAL(sourceCountChanged()), m_contextMenu, SLOT(updateContextMenuEntries()) );
}

ElevationProfileFloatItem::~ElevationProfileFloatItem()
{
}

QStringList ElevationProfileFloatItem::backendTypes() const
{
    return QStringList(QStringLiteral("elevationprofile"));
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
    return QStringLiteral("elevationprofile");
}

QString ElevationProfileFloatItem::version() const
{
    return QStringLiteral("1.2"); // TODO: increase to 1.3 ?
}

QString ElevationProfileFloatItem::description() const
{
    return tr( "A float item that shows the elevation profile of the current route." );
}

QString ElevationProfileFloatItem::copyrightYears() const
{
    return QStringLiteral("2011, 2012, 2013");
}

QVector<PluginAuthor> ElevationProfileFloatItem::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Florian Eßer"),QStringLiteral("f.esser@rwth-aachen.de"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"))
            << PluginAuthor(QStringLiteral("Roman Karlstetter"), QStringLiteral("roman.karlstetter@googlemail.com"));
}

QIcon ElevationProfileFloatItem::icon () const
{
    return QIcon(QStringLiteral(":/icons/elevationprofile.png"));
}

void ElevationProfileFloatItem::initialize ()
{
    connect( marbleModel()->elevationModel(), SIGNAL(updateAvailable()), &m_routeDataSource, SLOT(requestUpdate()) );
    connect( marbleModel()->routingManager()->routingModel(), SIGNAL(currentRouteChanged()), &m_routeDataSource, SLOT(requestUpdate()) );
    connect( this, SIGNAL(dataUpdated()), SLOT(forceRepaint()) );
    switchDataSource(&m_routeDataSource);

    m_fontHeight = QFontMetricsF( font() ).ascent() + 1;
    m_leftGraphMargin = QFontMetricsF( font() ).width( "0000 m" ); /// TODO make this dynamic according to actual need

    m_isInitialized = true;
}

bool ElevationProfileFloatItem::isInitialized () const
{
    return m_isInitialized;
}

void ElevationProfileFloatItem::setProjection( const ViewportParams *viewport )
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
    }

    update();

    AbstractFloatItem::setProjection( viewport );
}

void ElevationProfileFloatItem::paintContent( QPainter *painter )
{
    // do not try to draw if not initialized
    if(!isInitialized()) {
        return;
    }
    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setFont( font() );

    if ( ! ( m_activeDataSource->isDataAvailable() && m_eleData.size() > 0 ) ) {
        painter->setPen( QColor( Qt::black ) );
        QString text = tr( "Create a route or load a track from file to view its elevation profile." );
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
    painter->setPen( Oxygen::aluminumGray4 );
    painter->drawLine( m_leftGraphMargin, m_eleGraphHeight, contentSize().width(), m_eleGraphHeight );
    painter->drawLine( m_leftGraphMargin, m_eleGraphHeight, m_leftGraphMargin, 0 );

    // draw Y grid and labels
    painter->setPen( QColor( Qt::black ) );
    QPen dashedPen( Qt::DashLine );
    dashedPen.setColor( Oxygen::aluminumGray4 );
    QRect labelRect( 0, 0, m_leftGraphMargin - 1, m_fontHeight + 2 );
    lastStringEnds = m_eleGraphHeight + m_fontHeight;
//     painter->drawText(m_leftGraphMargin + 1, m_fontHeight, QLatin1Char('[') + m_axisY.unit() + QLatin1Char(']'));
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
            intervalStr += QLatin1Char(' ') + m_axisX.unit();
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
    QColor startColor = Oxygen::forestGreen4;
    QColor endColor = Oxygen::hotOrange4;
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
    const GeoDataCoordinates currentPoint = m_markerPlacemark->coordinate();
    if ( currentPoint.isValid() ) {
        painter->setPen( QColor( Qt::white ) );
        painter->drawLine( m_leftGraphMargin + m_cursorPositionX, 0,
                           m_leftGraphMargin + m_cursorPositionX, m_eleGraphHeight );
        qreal xpos = m_axisX.minValue() + ( m_cursorPositionX / m_eleGraphWidth ) * m_axisX.range();
        qreal ypos = m_eleGraphHeight - ( ( currentPoint.altitude() - m_axisY.minValue() ) / ( qMax<qreal>( 1.0, m_axisY.range() ) * m_shrinkFactorY ) ) * m_eleGraphHeight;

        painter->drawLine( m_leftGraphMargin + m_cursorPositionX - 5, ypos,
                           m_leftGraphMargin + m_cursorPositionX + 5, ypos );
        intervalStr.setNum( xpos * m_axisX.scale(), 'f', 2 );
        intervalStr += QLatin1Char(' ') + m_axisX.unit();
        int currentStringBegin = m_leftGraphMargin + m_cursorPositionX
                             - QFontMetricsF( font() ).width( intervalStr ) / 2;
        painter->drawText( currentStringBegin, contentSize().height() - 1.5 * m_fontHeight, intervalStr );

        intervalStr.setNum( currentPoint.altitude(), 'f', 1 );
        intervalStr += QLatin1Char(' ') + m_axisY.unit();
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

QDialog *ElevationProfileFloatItem::configDialog() //FIXME TODO Make a config dialog? /// TODO what is this comment?
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::ElevationProfileConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        readSettings();

        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()), SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()), SLOT(readSettings()) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()), this, SLOT(writeSettings()) );
    }
    return m_configDialog;
}

void ElevationProfileFloatItem::contextMenuEvent( QWidget *w, QContextMenuEvent *e )
{
    Q_ASSERT( m_contextMenu );
    m_contextMenu->getMenu()->exec( w->mapToGlobal( e->pos() ) );
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
        connect( this, SIGNAL(dataUpdated()), this, SLOT(updateVisiblePoints()) );
        connect( m_marbleWidget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                 this, SLOT(updateVisiblePoints()) );
        connect( this, SIGNAL(settingsChanged(QString)), this, SLOT(updateVisiblePoints()) );
    }

    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        GeoDataTreeModel *const treeModel = const_cast<MarbleModel *>( marbleModel() )->treeModel();

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

            if ( e->type() == QEvent::MouseMove && !(event->buttons() & Qt::LeftButton) ) {
                // Cross hair cursor when moving above the float item
                // and mark the position on the graph
                widget->setCursor(QCursor(Qt::CrossCursor));
                if ( m_cursorPositionX != event->pos().x() - plotRect.left() ) {
                    m_cursorPositionX = event->pos().x() - plotRect.left();
                    const qreal xpos = m_axisX.minValue() + ( m_cursorPositionX / m_eleGraphWidth ) * m_axisX.range();
                    GeoDataCoordinates currentPoint; // invalid coordinates
                    for ( int i = start; i < end; ++i) {
                        if ( m_eleData.value(i).x() >= xpos ) {
                            currentPoint = m_points[i];
                            currentPoint.setAltitude( m_eleData.value(i).y() );
                            break;
                        }
                    }
                    m_markerPlacemark->setCoordinate( currentPoint );
                    if ( m_documentIndex < 0 ) {
                        m_documentIndex = treeModel->addDocument( &m_markerDocument );
                    }
                    emit repaintNeeded();
                }

                return true;
            }
        }
        else {
            if ( m_documentIndex >= 0 ) {
                m_markerPlacemark->setCoordinate( GeoDataCoordinates() ); // set to invalid
                treeModel->removeDocument( &m_markerDocument );
                m_documentIndex = -1;
                emit repaintNeeded();
            }
        }
    }

    return AbstractFloatItem::eventFilter(object,e);
}

void ElevationProfileFloatItem::handleDataUpdate(const GeoDataLineString &points, const QVector<QPointF> &eleData)
{
    m_eleData = eleData;
    m_points = points;
    calculateStatistics( m_eleData );
    if ( m_eleData.length() >= 2 ) {
        m_axisX.setRange( m_eleData.first().x(), m_eleData.last().x() );
        m_axisY.setRange( qMin( m_minElevation, qreal( 0.0 ) ), m_maxElevation );
    }

    emit dataUpdated();
}

void ElevationProfileFloatItem::updateVisiblePoints()
{
    if ( ! m_activeDataSource->isDataAvailable() || m_points.size() < 2 ) {
        return;
    }

    // find the longest visible route section on screen
    QList<QList<int> > routeSegments;
    QList<int> currentRouteSegment;
    for ( int i = 0; i < m_eleData.count(); i++ ) {
        qreal lon = m_points[i].longitude(GeoDataCoordinates::Degree);
        qreal lat = m_points[i].latitude (GeoDataCoordinates::Degree);
        qreal x = 0;
        qreal y = 0;

        if ( m_marbleWidget->screenCoordinates(lon, lat, x, y) ) {
            // on screen --> add point to list
            currentRouteSegment.append(i);
        } else {
            // off screen --> start new list
            if ( !currentRouteSegment.isEmpty() ) {
                routeSegments.append( currentRouteSegment );
                currentRouteSegment.clear();
            }
        }
    }
    routeSegments.append( currentRouteSegment ); // in case the route ends on screen

    int maxLenght = 0;
    foreach ( const QList<int> &currentRouteSegment, routeSegments ) {
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

    // include setting range to statistics and test for m_zoomToViewport in calculateStatistics();
    if ( m_zoomToViewport ) {
        calculateStatistics( m_eleData );
        m_axisX.setRange( m_eleData.value( m_firstVisiblePoint ).x(),
                          m_eleData.value( m_lastVisiblePoint  ).x() );
        m_axisY.setRange( m_minElevation, m_maxElevation );
    }

    return;
}

void ElevationProfileFloatItem::calculateStatistics(const QVector<QPointF> &eleData)
{
    // This basically calculates the important peaks of the moving average filtered elevation and
    // calculates the elevation data based on this points.
    // This is done by always placing the averaging window in a way that it starts or ends at an
    // original data point. This should ensure that all minima/maxima of the moving average
    // filtered data are covered.
    const qreal averageDistance = 200.0;

    m_maxElevation = 0.0;
    m_minElevation = invalidElevationData;
    m_gain = 0.0;
    m_loss = 0.0;
    const int start = m_zoomToViewport ? m_firstVisiblePoint : 0;
    const int end = m_zoomToViewport ? m_lastVisiblePoint + 1 : eleData.size();

    if( start < end ) {
        qreal lastX = eleData.value( start ).x();
        qreal lastY = eleData.value( start ).y();
        qreal nextX = eleData.value( start + 1 ).x();
        qreal nextY = eleData.value( start + 1 ).y();

        m_maxElevation = qMax( lastY, nextY );
        m_minElevation = qMin( lastY, nextY );

        int averageStart = start;
        if(lastX + averageDistance < eleData.value( start + 2 ).x())
            ++averageStart;

        for ( int index = start + 2; index <= end; ++index ) {
            qreal indexX = index < end ? eleData.value( index ).x() : eleData.value( end - 1 ).x() + averageDistance;
            qreal indexY = eleData.value( qMin( index, end - 1 ) ).y();
            m_maxElevation = qMax( m_maxElevation, indexY );
            m_minElevation = qMin( m_minElevation, indexY );

            // Low-pass filtering (moving average) of the elevation profile to calculate gain and loss values
            // not always the best method, see for example
            // http://www.ikg.uni-hannover.de/fileadmin/ikg/staff/thesis/finished/documents/StudArb_Schulze.pdf
            // (German), chapter 4.2

            // Average over the part ending with the previous point.
            // Do complete recalculation to avoid accumulation of floating point artifacts.
            nextY = 0;
            qreal averageX = nextX - averageDistance;
            for( int averageIndex = averageStart; averageIndex < index; ++averageIndex ) {
                qreal nextAverageX = eleData.value( averageIndex ).x();
                qreal ratio = ( nextAverageX - averageX ) / averageDistance; // Weighting of original data based on covered distance
                nextY += eleData.value( qMax( averageIndex - 1, 0 ) ).y() * ratio;
                averageX = nextAverageX;
            }

            while( averageStart < index ) {
                // This handles the part ending with the previous point on the first iteration and the parts starting with averageStart afterwards
                if ( nextY > lastY ) {
                    m_gain += nextY - lastY;
                } else {
                    m_loss += lastY - nextY;
                }

                // Here we split the data into parts that average over the same data points
                // As soon as the end of the averaging window reaches the current point we reached the end of the current part
                lastX = nextX;
                lastY = nextY;
                nextX = eleData.value( averageStart ).x() + averageDistance;
                if( nextX >= indexX ) {
                    break;
                }

                // We don't need to recalculate the average completely, just remove the reached point
                qreal ratio = (nextX - lastX) / averageDistance;
                nextY += ( eleData.value( index - 1 ).y() - eleData.value( qMax( averageStart - 1, 0 ) ).y() ) * ratio;
                ++averageStart;
            }

            // This is for the next part already, the end of the averaging window is at the following point
            nextX = indexX;
        }

        // Also include the last point
        nextY = eleData.value( end - 1 ).y();
        if ( nextY > lastY ) {
            m_gain += nextY - lastY;
        } else {
            m_loss += lastY - nextY;
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

void ElevationProfileFloatItem::switchToRouteDataSource()
{
    switchDataSource(&m_routeDataSource);
}

void ElevationProfileFloatItem::switchToTrackDataSource(int index)
{
    m_trackDataSource.setSourceIndex(index);
    switchDataSource(&m_trackDataSource);
}

void ElevationProfileFloatItem::switchDataSource(ElevationProfileDataSource* source)
{
    if (m_activeDataSource) {
        disconnect(m_activeDataSource, SIGNAL(dataUpdated(GeoDataLineString,QVector<QPointF>)),0,0);
    }
    m_activeDataSource = source;
    connect(m_activeDataSource, SIGNAL(dataUpdated(GeoDataLineString,QVector<QPointF>)), this, SLOT(handleDataUpdate(GeoDataLineString,QVector<QPointF>)));
    m_activeDataSource->requestUpdate();
}

}

#include "moc_ElevationProfileFloatItem.cpp"
