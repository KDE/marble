//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingLayer.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoPainter.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetPopupMenu.h"
#include "RoutingModel.h"
#include "Route.h"
#include "RouteRequest.h"
#include "MarbleModel.h"
#include "AlternativeRoutesModel.h"
#include "RoutingManager.h"
#include "Maneuver.h"
#include "RenderState.h"

#include <QAbstractItemModel>
#include <QIcon>
#include <QItemSelectionModel>
#include <QAction>
#include <QMouseEvent>
#include <QPixmap>
#include <QFileDialog>

namespace Marble
{

class RoutingLayerPrivate
{
    template<class T>
    struct PaintRegion {
        T index;
        QRegion region;

        PaintRegion( const T &index_, const QRegion &region_ ) :
                index( index_ ), region( region_ )
        {
            // nothing to do
        }
    };

    typedef PaintRegion<QModelIndex> ModelRegion;
    typedef PaintRegion<int> RequestRegion;

public:
    RoutingLayer *const q;

    QList<ModelRegion> m_instructionRegions;

    QList<RequestRegion> m_regions;

    QList<RequestRegion> m_alternativeRouteRegions;

    QList<ModelRegion> m_placemarks;

    QRegion m_routeRegion;

    int m_movingIndex;

    MarbleWidget *const m_marbleWidget;

    QPixmap m_targetPixmap;

    QRect m_dirtyRect;

    QPoint m_dropStopOver;

    QPoint m_dragStopOver;

    int m_dragStopOverRightIndex;

    RoutingModel *const m_routingModel;

    MarblePlacemarkModel *m_placemarkModel;

    QItemSelectionModel *m_selectionModel;

    QSize m_pixmapSize;

    RouteRequest *const m_routeRequest;

    MarbleWidgetPopupMenu *m_contextMenu;

    QAction *m_removeViaPointAction;

    int m_activeMenuIndex;

    AlternativeRoutesModel *const m_alternativeRoutesModel;

    ViewContext m_viewContext;

    bool m_viewportChanged;

    bool m_isInteractive;

    /** Constructor */
    explicit RoutingLayerPrivate( RoutingLayer *parent, MarbleWidget *widget );

    /** Update the cached drag position. Use an empty point to clear it. */
    void storeDragPosition( const QPoint &position );

    // The following methods are mostly only called at one place in the code, but often
    // Inlined to avoid the function call overhead. Having functions here is just to
    // keep the code clean

    /** Returns the same color as the given one with its alpha channel adjusted to the given value */
    static inline QColor alphaAdjusted( const QColor &color, int alpha );

    /**
      * Returns the start or destination position if Ctrl key is among the
      * provided modifiers, the cached insert position otherwise
      */
    inline int viaInsertPosition( Qt::KeyboardModifiers modifiers ) const;

    /** Paint icons for each placemark in the placemark model */
    inline void renderPlacemarks( GeoPainter *painter );

    /** Paint waypoint polygon */
    inline void renderRoute( GeoPainter *painter );

    /** Paint turn instruction for selected items */
    inline void renderAnnotations( GeoPainter *painter ) const;

    /** Paint alternative routes in gray */
    inline void renderAlternativeRoutes( GeoPainter *painter );

    /** Paint icons for trip points etc */
    inline void renderRequest( GeoPainter *painter );

    /** Insert via points or emit position signal, if appropriate */
    inline bool handleMouseButtonRelease( QMouseEvent *e );

    /** Select route instructions points, start dragging trip points */
    inline bool handleMouseButtonPress( QMouseEvent *e );

    /** Dragging trip points, route polygon hovering */
    inline bool handleMouseMove( QMouseEvent *e );

    /** True if the given point (screen coordinates) is among the route instruction points */
    inline bool isInfoPoint( const QPoint &point );

    /** True if the given point (screen coordinates) is above an alternative route */
    inline bool isAlternativeRoutePoint( const QPoint &point );

    /** Paint the stopover indicator pixmap at the given position. Also repaints the old position */
    inline void paintStopOver( QRect position );

    /** Removes the stopover indicator pixmap. Also repaints its old position */
    inline void clearStopOver();
};

RoutingLayerPrivate::RoutingLayerPrivate( RoutingLayer *parent, MarbleWidget *widget ) :
        q( parent ), m_movingIndex( -1 ), m_marbleWidget( widget ),
        m_targetPixmap(QStringLiteral(":/data/bitmaps/routing_pick.png")),
        m_dragStopOverRightIndex(-1),
        m_routingModel( widget->model()->routingManager()->routingModel() ),
        m_placemarkModel( 0 ),
        m_selectionModel( 0 ),
        m_pixmapSize( 22, 22 ),
        m_routeRequest( widget->model()->routingManager()->routeRequest() ),
        m_activeMenuIndex( -1 ),
        m_alternativeRoutesModel( widget->model()->routingManager()->alternativeRoutesModel() ),
        m_viewContext( Still ),
        m_viewportChanged( true ),
        m_isInteractive( true )
{
    m_contextMenu = new MarbleWidgetPopupMenu( m_marbleWidget, m_marbleWidget->model() );
    m_removeViaPointAction = new QAction( QObject::tr( "&Remove this destination" ), q );
    QObject::connect( m_removeViaPointAction, SIGNAL(triggered()), q, SLOT(removeViaPoint()) );
    m_contextMenu->addAction( Qt::RightButton, m_removeViaPointAction );
    QAction *exportAction = new QAction( QObject::tr( "&Export route..." ), q );
    QObject::connect( exportAction, SIGNAL(triggered()), q, SLOT(exportRoute()) );
    m_contextMenu->addAction( Qt::RightButton, exportAction );
    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        m_pixmapSize = QSize( 38, 38 );
    }

}

int RoutingLayerPrivate::viaInsertPosition( Qt::KeyboardModifiers modifiers ) const
{
    if ( modifiers & Qt::ControlModifier ) {
        bool leftHand = m_routeRequest->size() / 2 >= m_dragStopOverRightIndex;
        if ( leftHand && m_routeRequest->size() > 2 ) {
            return 0;
        } else {
            return m_routeRequest->size();
        }
    } else {
        return m_dragStopOverRightIndex;
    }
}

void RoutingLayerPrivate::renderPlacemarks( GeoPainter *painter )
{
    m_placemarks.clear();
    painter->setPen( QColor( Qt::black ) );
    for ( int i = 0; i < m_placemarkModel->rowCount(); ++i ) {
        QModelIndex index = m_placemarkModel->index( i, 0 );
        QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );
        if ( index.isValid() && !data.isNull() ) {
            GeoDataCoordinates pos = data.value<GeoDataCoordinates>();

            QPixmap pixmap = index.data( Qt::DecorationRole ).value<QPixmap>();
            if ( !pixmap.isNull() && m_selectionModel->isSelected( index ) ) {
                QIcon selected = QIcon( pixmap );
                QPixmap result = selected.pixmap( m_pixmapSize, QIcon::Selected, QIcon::On );
                painter->drawPixmap( pos, result );
            } else {
                painter->drawPixmap( pos, pixmap );
            }

            const QRegion region = painter->regionFromPixmapRect(pos, m_targetPixmap.width(), m_targetPixmap.height());
            m_placemarks.push_back( ModelRegion( index, region ) );
        }
    }
}

void RoutingLayerPrivate::renderAlternativeRoutes( GeoPainter *painter )
{
    QPen alternativeRoutePen( m_marbleWidget->model()->routingManager()->routeColorAlternative() );
    alternativeRoutePen.setWidth( 5 );
    painter->setPen( alternativeRoutePen );

    for ( int i=0; i<m_alternativeRoutesModel->rowCount(); ++i ) {
        GeoDataDocument* route = m_alternativeRoutesModel->route( i );
        if ( route && route != m_alternativeRoutesModel->currentRoute() ) {
            const GeoDataLineString* points = AlternativeRoutesModel::waypoints( route );
            if ( points ) {
                painter->drawPolyline( *points );
                if ( m_viewportChanged && m_isInteractive && m_viewContext == Still ) {
                    QRegion region = painter->regionFromPolyline( *points, 8 );
                    m_alternativeRouteRegions.push_back( RequestRegion( i, region ) );
                }
            }
        }
    }
}

void RoutingLayerPrivate::renderRoute( GeoPainter *painter )
{
    GeoDataLineString waypoints = m_routingModel->route().path();

    QPen standardRoutePen( m_marbleWidget->model()->routingManager()->routeColorStandard() );
    standardRoutePen.setWidth( 5 );
    if ( m_marbleWidget->model()->routingManager()->state() == RoutingManager::Downloading ) {
        standardRoutePen.setStyle( Qt::DotLine );
    }
    painter->setPen( standardRoutePen );

    painter->drawPolyline( waypoints );
    if ( m_viewportChanged && m_viewContext == Still ) {
        int const offset = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ? 24 : 8;
        if ( m_isInteractive ) {
            m_routeRegion = painter->regionFromPolyline( waypoints, offset );
        }
    }


    standardRoutePen.setWidth( 2 );
    painter->setPen( standardRoutePen );

    // Map matched position
    //painter->setBrush( QBrush( Oxygen::brickRed4) );
    //painter->drawEllipse( m_routingModel->route().positionOnRoute(), 8, 8 );

    painter->setBrush( QBrush( m_marbleWidget->model()->routingManager()->routeColorAlternative() ) );

    if ( !m_dropStopOver.isNull() ) {
        int dx = 1 + m_pixmapSize.width() / 2;
        int dy = 1 + m_pixmapSize.height() / 2;
        QPoint center = m_dropStopOver - QPoint( dx, dy );
        painter->drawPixmap( center, m_targetPixmap );

        if ( !m_dragStopOver.isNull() && m_dragStopOverRightIndex >= 0 && m_dragStopOverRightIndex <= m_routeRequest->size() ) {
            QPoint moved = m_dropStopOver - m_dragStopOver;
            if ( moved.manhattanLength() > 10 ) {
                qreal lon( 0.0 ), lat( 0.0 );
                if ( m_marbleWidget->geoCoordinates( m_dropStopOver.x(), m_dropStopOver.y(),
                                                     lon, lat, GeoDataCoordinates::Radian ) ) {
                    GeoDataCoordinates drag( lon, lat );
                    standardRoutePen.setStyle( Qt::DotLine );
                    painter->setPen( standardRoutePen );
                    GeoDataLineString lineString;
                    if ( m_dragStopOverRightIndex > 0 ) {
                        lineString << m_routeRequest->at( m_dragStopOverRightIndex-1 );
                    }
                    lineString << drag;
                    if ( m_dragStopOverRightIndex < m_routeRequest->size() ) {
                        lineString << m_routeRequest->at( m_dragStopOverRightIndex );
                    }
                    painter->drawPolyline( lineString );
                    standardRoutePen.setStyle( Qt::SolidLine );
                    painter->setPen( standardRoutePen );
                }
            }
        }
    }

    if ( m_viewContext == Animation ) {
        return;
    }

    if( m_routingModel->rowCount() == m_routingModel->route().size() ) {
        m_instructionRegions.clear();
        for ( int i = 0; i < m_routingModel->rowCount(); ++i ) {
            QModelIndex index = m_routingModel->index( i, 0 );
            GeoDataCoordinates pos = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();

            painter->setBrush( QBrush( m_marbleWidget->model()->routingManager()->routeColorAlternative() ) );
            if ( m_selectionModel && m_selectionModel->selection().contains( index ) ) {
                const RouteSegment &segment = m_routingModel->route().at( i );
                const GeoDataLineString currentRoutePoints = segment.path();

                QPen activeRouteSegmentPen( m_marbleWidget->model()->routingManager()->routeColorHighlighted() );

                activeRouteSegmentPen.setWidth( 6 );
                if ( m_marbleWidget->model()->routingManager()->state() == RoutingManager::Downloading ) {
                    activeRouteSegmentPen.setStyle( Qt::DotLine );
                }
                painter->setPen( activeRouteSegmentPen );
                painter->drawPolyline( currentRoutePoints );

                painter->setPen( standardRoutePen );
                painter->setBrush( QBrush( alphaAdjusted( Oxygen::hotOrange4, 200 ) ) );
            }
            painter->drawEllipse( pos, 6, 6 );

            if ( m_isInteractive ) {
                QRegion region = painter->regionFromEllipse( pos, 12, 12 );
                m_instructionRegions.push_front( ModelRegion( index, region ) );
            }
        }
    }

    if( !m_routingModel->deviatedFromRoute() ) {
        GeoDataCoordinates location = m_routingModel->route().currentSegment().nextRouteSegment().maneuver().position();
        QString nextInstruction = m_routingModel->route().currentSegment().nextRouteSegment().maneuver().instructionText();
        if( !nextInstruction.isEmpty() ) {
            painter->setBrush( QBrush( Oxygen::hotOrange4 ) );
            painter->drawEllipse( location, 6, 6 );
        }
    }
}

void RoutingLayerPrivate::renderAnnotations( GeoPainter *painter ) const
{
    if ( !m_selectionModel || m_selectionModel->selection().isEmpty() ) {
        // nothing to do
        return;
    }

    for ( int i = 0; i < m_routingModel->rowCount(); ++i ) {
        QModelIndex index = m_routingModel->index( i, 0 );

        if ( m_selectionModel->selection().contains( index ) ) {
            bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
            GeoDataCoordinates pos = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();
            painter->setPen( QColor( Qt::black ) );
            painter->setBrush( QBrush( Oxygen::sunYellow6 ) );
            painter->drawAnnotation( pos, index.data().toString(), QSize( smallScreen ? 240 : 120, 0 ), 10, 30, 5, 5 );
        }
    }
}

void RoutingLayerPrivate::renderRequest( GeoPainter *painter )
{
    m_regions.clear();
    for ( int i = 0; i < m_routeRequest->size(); ++i ) {
        const GeoDataCoordinates pos = m_routeRequest->at( i );
        if ( pos.isValid() ) {
            QPixmap pixmap = m_routeRequest->pixmap( i );
            painter->drawPixmap( pos, pixmap );
            const QRegion region = painter->regionFromPixmapRect(pos, pixmap.width(), pixmap.height());
            m_regions.push_front( RequestRegion( i, region ) );
        }
    }
}

void RoutingLayerPrivate::storeDragPosition( const QPoint &pos )
{
    m_dragStopOver = pos;
    m_dragStopOverRightIndex = -1;

    qreal lon( 0.0 ), lat( 0.0 );
    if ( m_routeRequest && !pos.isNull()
        && m_marbleWidget->geoCoordinates( pos.x(), pos.y(), lon, lat, GeoDataCoordinates::Radian ) ) {
        GeoDataCoordinates waypoint( lon, lat );
        m_dragStopOverRightIndex = m_routingModel->rightNeighbor( waypoint, m_routeRequest );
    }
}

QColor RoutingLayerPrivate::alphaAdjusted( const QColor &color, int alpha )
{
    QColor result( color );
    result.setAlpha( alpha );
    return result;
}

bool RoutingLayerPrivate::handleMouseButtonPress( QMouseEvent *e )
{
    foreach( const RequestRegion &region, m_regions ) {
        if ( region.region.contains( e->pos() ) ) {
            if ( e->button() == Qt::LeftButton ) {
                m_movingIndex = region.index;
                m_dropStopOver = QPoint();
                m_dragStopOver = QPoint();
                return true;
            } else if ( e->button() == Qt::RightButton ) {
                m_removeViaPointAction->setEnabled( true );
                m_activeMenuIndex = region.index;
                m_contextMenu->showRmbMenu( e->x(), e->y() );
                return true;
            } else
                return false;
        }
    }

    foreach( const ModelRegion &region, m_instructionRegions ) {
        if ( region.region.contains( e->pos() ) && m_selectionModel ) {
            if ( e->button() == Qt::LeftButton ) {
                QItemSelectionModel::SelectionFlag command = QItemSelectionModel::ClearAndSelect;
                if ( m_selectionModel->isSelected( region.index ) ) {
                    command = QItemSelectionModel::Clear;
                }
                m_selectionModel->select( region.index, command );
                m_dropStopOver = e->pos();
                storeDragPosition( e->pos() );
                // annotation and old annotation are dirty, large region
                emit q->repaintNeeded();
                return true;
            } else if ( e->button() == Qt::RightButton ) {
                m_removeViaPointAction->setEnabled( false );
                m_contextMenu->showRmbMenu( e->x(), e->y() );
                return true;
            } else
                return false;
        }
    }

    if ( m_routeRegion.contains( e->pos() ) ) {
        if ( e->button() == Qt::LeftButton ) {
            /** @todo: Determine the neighbored via points and insert in order */
            m_dropStopOver = e->pos();
            storeDragPosition( e->pos() );
            return true;
        } else if ( e->button() == Qt::RightButton ) {
            m_removeViaPointAction->setEnabled( false );
            m_contextMenu->showRmbMenu( e->x(), e->y() );
            return true;
        } else
            return false;
    }

    if ( e->button() != Qt::LeftButton ) {
        return false;
    }

    foreach( const RequestRegion &region, m_alternativeRouteRegions ) {
        if ( region.region.contains( e->pos() ) ) {
            m_alternativeRoutesModel->setCurrentRoute( region.index );
            return true;
        }
    }

    foreach( const ModelRegion &region, m_placemarks ) {
        if ( region.region.contains( e->pos() ) ) {
            emit q->placemarkSelected( region.index );
            return true;
        }
    }

    return false;
}

bool RoutingLayerPrivate::handleMouseButtonRelease( QMouseEvent *e )
{
    if ( e->button() != Qt::LeftButton ) {
        return false;
    }

    if ( m_movingIndex >= 0 ) {
        m_movingIndex = -1;
        clearStopOver();
        m_marbleWidget->model()->routingManager()->retrieveRoute();
        return true;
    }

    if ( !m_dropStopOver.isNull() && !m_dragStopOver.isNull() ) {
        QPoint moved = e->pos() - m_dragStopOver;
        if ( moved.manhattanLength() < 10 ) {
            return false;
        }

        qreal lon( 0.0 ), lat( 0.0 );
        if ( m_dragStopOverRightIndex >= 0 && m_dragStopOverRightIndex <= m_routeRequest->size()
                && m_marbleWidget->geoCoordinates( m_dropStopOver.x(), m_dropStopOver.y(), lon, lat, GeoDataCoordinates::Radian ) ) {
            GeoDataCoordinates position( lon, lat );
            m_dragStopOverRightIndex = viaInsertPosition( e->modifiers() );
            m_routeRequest->insert( m_dragStopOverRightIndex, position );
            clearStopOver();
            m_marbleWidget->model()->routingManager()->retrieveRoute();
            return true;
        }
    }

    return false;
}

bool RoutingLayerPrivate::handleMouseMove( QMouseEvent *e )
{
    qreal lon( 0.0 ), lat( 0.0 );
    if ( m_marbleWidget->geoCoordinates( e->pos().x(), e->pos().y(),
                                         lon, lat, GeoDataCoordinates::Radian ) ) {

        if ( m_movingIndex >= 0 ) {
            GeoDataCoordinates moved( lon, lat );
            m_routeRequest->setPosition( m_movingIndex, moved );
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( !m_dragStopOver.isNull() ) {
            // Repaint only that region of the map that is affected by the change
            m_dragStopOverRightIndex = viaInsertPosition( e->modifiers() );
            QRect dirty = m_routeRegion.boundingRect();
            dirty |= QRect( m_dropStopOver, m_pixmapSize );
            dirty |= QRect( e->pos(), m_pixmapSize );
            if ( e->buttons() & Qt::LeftButton ) {
                m_dropStopOver = e->pos();
            } else {
                m_dragStopOver = QPoint();
                m_dropStopOver = QPoint();
            }
            emit q->repaintNeeded( dirty );
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( isInfoPoint( e->pos() ) ) {
            clearStopOver();
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( m_routeRegion.contains( e->pos() ) ) {
            m_dropStopOver = e->pos();
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( !m_dropStopOver.isNull() ) {
            clearStopOver();
        } else if ( isAlternativeRoutePoint( e->pos() ) ) {
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        }
        else {
            return false;
        }

        // Update pixmap in the map (old and new position needs repaint)
        paintStopOver( QRect( e->pos(), m_pixmapSize ) );
        return true;
    }

    return false;
}

bool RoutingLayerPrivate::isInfoPoint( const QPoint &point )
{
    foreach( const RequestRegion &region, m_regions ) {
        if ( region.region.contains( point ) ) {
            return true;
        }
    }

    foreach( const ModelRegion &region, m_instructionRegions ) {
        if ( region.region.contains( point ) ) {
            return true;
        }
    }

    return false;
}

 bool RoutingLayerPrivate::isAlternativeRoutePoint( const QPoint &point )
 {
     foreach( const RequestRegion &region, m_alternativeRouteRegions ) {
         if ( region.region.contains( point ) ) {
             return true;
         }
     }

     return false;
 }

void RoutingLayerPrivate::paintStopOver( QRect dirty )
{
    emit q->repaintNeeded( m_dirtyRect );
    int dx = 1 + m_pixmapSize.width() / 2;
    int dy = 1 + m_pixmapSize.height() / 2;
    dirty.adjust( -dx, -dy, -dx, -dy );
    emit q->repaintNeeded( dirty );
    m_dirtyRect = dirty;
}

void RoutingLayerPrivate::clearStopOver()
{
    m_dropStopOver = QPoint();
    m_dragStopOver = QPoint();
    emit q->repaintNeeded( m_dirtyRect );
}

RoutingLayer::RoutingLayer( MarbleWidget *widget, QWidget *parent ) :
        QObject( parent ), d( new RoutingLayerPrivate( this, widget ) )
{
    connect( widget->model()->routingManager(), SIGNAL(stateChanged(RoutingManager::State)),
             this, SLOT(updateRouteState()) );
    connect( widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
            this, SLOT(setViewportChanged()) );
    connect( widget->model()->routingManager()->alternativeRoutesModel(), SIGNAL(currentRouteChanged(GeoDataDocument*)),
            this, SLOT(setViewportChanged()) );
    connect( widget->model()->routingManager()->alternativeRoutesModel(), SIGNAL(currentRouteChanged(GeoDataDocument*)),
             this, SIGNAL(repaintNeeded()) );
    connect( widget->model()->routingManager()->alternativeRoutesModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(showAlternativeRoutes()) );
}

RoutingLayer::~RoutingLayer()
{
    delete d;
}

QStringList RoutingLayer::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

qreal RoutingLayer::zValue() const
{
    return 1.0;
}

bool RoutingLayer::render( GeoPainter *painter, ViewportParams *viewport,
                           const QString& renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();

    if ( d->m_placemarkModel) {
        d->renderPlacemarks( painter );
    }

    if ( d->m_alternativeRoutesModel ) {
        d->renderAlternativeRoutes( painter );
    }

    d->renderRoute( painter );

    if ( d->m_routeRequest) {
        d->renderRequest( painter );
    }

    d->renderAnnotations( painter );

    painter->restore();
    if ( d->m_viewportChanged && d->m_viewContext == Still ) {
        d->m_viewportChanged = false;
    }
    return true;
}

RenderState RoutingLayer::renderState() const
{
    return RenderState(QStringLiteral("Routing"), d->m_marbleWidget->model()->routingManager()->state() == RoutingManager::Downloading ? WaitingForUpdate : Complete);
}

bool RoutingLayer::eventFilter( QObject *obj, QEvent *event )
{
    Q_UNUSED( obj )

    if ( !d->m_isInteractive ) {
        return false;
    }

    if ( event->type() == QEvent::MouseButtonPress ) {
        QMouseEvent *e = static_cast<QMouseEvent*>( event );
        return d->handleMouseButtonPress( e );
    }

    if ( event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent *e = static_cast<QMouseEvent*>( event );
        return d->handleMouseButtonRelease( e );
    }

    if ( event->type() == QEvent::MouseMove ) {
        QMouseEvent *e = static_cast<QMouseEvent*>( event );
        return d->handleMouseMove( e );
    }

    return false;
}

void RoutingLayer::setPlacemarkModel ( MarblePlacemarkModel *model )
{
    d->m_placemarkModel = model;
    setViewportChanged();
}

void RoutingLayer::synchronizeWith( QItemSelectionModel *selection )
{
    d->m_selectionModel = selection;
}

void RoutingLayer::removeViaPoint()
{
    if ( d->m_activeMenuIndex >= 0 ) {
        d->m_routeRequest->remove( d->m_activeMenuIndex );
        d->m_activeMenuIndex = -1;
        emit repaintNeeded();
        d->m_marbleWidget->model()->routingManager()->retrieveRoute();
    }
}

void RoutingLayer::showAlternativeRoutes()
{
    setViewportChanged();
    emit repaintNeeded();
}

void RoutingLayer::exportRoute()
{
    QString fileName = QFileDialog::getSaveFileName( d->m_marbleWidget,
                       tr( "Export Route" ), // krazy:exclude=qclasses
                       QDir::homePath(),
                       tr( "GPX and KML files (*.gpx *.kml)" ) );

    if ( !fileName.isEmpty() ) {
        if ( fileName.endsWith( QLatin1String( ".gpx" ), Qt::CaseInsensitive ) ) {
            QFile gpx( fileName );
            if ( gpx.open( QFile::WriteOnly) ) {
                d->m_routingModel->exportGpx( &gpx );
                gpx.close();
            }
        } else {
            d->m_marbleWidget->model()->routingManager()->saveRoute( fileName );
        }
    }
}

void RoutingLayer::updateRouteState()
{
    setViewportChanged();
    emit repaintNeeded();
}

void RoutingLayer::setViewportChanged()
{
    d->m_viewportChanged = true;
    d->m_routeRegion = QRegion();
    d->m_instructionRegions.clear();
    d->m_alternativeRouteRegions.clear();
}

void RoutingLayer::setViewContext( ViewContext viewContext )
{
    d->m_viewContext = viewContext;
}

void RoutingLayer::setInteractive( bool interactive )
{
    d->m_isInteractive = interactive;
}

bool RoutingLayer::isInteractive() const
{
    return d->m_isInteractive;
}

QString RoutingLayer::runtimeTrace() const
{
    return QStringLiteral("Routing Layer");
}

} // namespace Marble

#include "moc_RoutingLayer.cpp"
