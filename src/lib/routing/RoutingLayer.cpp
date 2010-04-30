//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingLayer.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoPainter.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetPopupMenu.h"
#include "RoutingModel.h"
#include "RouteSkeleton.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtGui/QPixmap>
#include <QtGui/QMenu>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QAbstractProxyModel>
#include <QtGui/QItemSelectionModel>

namespace Marble {

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
    typedef PaintRegion<int> SkeletonRegion;

public:
    RoutingLayer *q;

    QList<ModelRegion> m_instructionRegions;

    QList<SkeletonRegion> m_regions;

    QList<ModelRegion> m_placemarks;

    QRegion m_routeRegion;

    QAbstractProxyModel *m_proxyModel;

    int m_movingIndex;

    MarbleWidget *m_marbleWidget;

    QPixmap m_targetPixmap;

    QPixmap m_viaPixmap;

    QRect m_movingIndexDirtyRect;

    QPoint m_insertStopOver;

    bool m_dragStopOver;

    bool m_pointSelection;

    RoutingModel *m_routingModel;

    MarblePlacemarkModel *m_placemarkModel;

    QItemSelectionModel *m_selectionModel;

    bool m_routeDirty;

    QSize m_pixmapSize;

    RouteSkeleton *m_routeSkeleton;

    MarbleWidgetPopupMenu *m_contextMenu;

    QAction *m_removeViaPointAction;

    int m_activeMenuIndex;

    /** Constructor */
    explicit RoutingLayerPrivate( RoutingLayer *parent, MarbleWidget *widget );

    /** Show a context menu at the specified position */
    void showContextMenu( const QPoint &position );

    // The following methods are mostly only called at one place in the code, but often
    // Inlined to avoid the function call overhead. Having functions here is just to
    // keep the code clean

    /** Paint icons for each placemark in the placemark model */
    inline void renderPlacemarks( GeoPainter *painter );

    /** Paint waypoint polygon */
    inline void renderRoute( GeoPainter *painter );

    /** Paint icons for trip points etc */
    inline void renderSkeleton( GeoPainter *painter );

    /** Insert via points or emit position signal, if appropriate */
    inline bool handleMouseButtonRelease( QMouseEvent *e );

    /** Select route instructions points, start dragging trip points */
    inline bool handleMouseButtonPress( QMouseEvent *e );

    /** Dragging trip points, route polygon hovering */
    inline bool handleMouseMove( QMouseEvent *e );

    /** Escape to stop selecting points */
    inline bool handleKeyEvent( QKeyEvent *e );

    /** True if the given point (screen coordinates) is among the route instruction points */
    inline bool isInfoPoint( const QPoint &point );

    /** Paint the stopover indicator pixmap at the given position. Also repaints the old position */
    inline void paintStopOver( QRect position );

    /** Removes the stopover indicator pixmap. Also repaints its old position */
    inline void clearStopOver();
};

RoutingLayerPrivate::RoutingLayerPrivate( RoutingLayer *parent, MarbleWidget *widget ) :
  q( parent ), m_proxyModel( 0 ), m_movingIndex( -1 ), m_marbleWidget( widget ), m_targetPixmap( ":/data/bitmaps/routing_pick.png" ),
  m_viaPixmap( ":/data/bitmaps/routing_via.png" ), m_dragStopOver( false ), m_pointSelection( false ),
  m_routingModel( 0 ), m_placemarkModel( 0 ), m_selectionModel( 0 ), m_routeDirty( false ), m_pixmapSize( 22,22 ),
  m_routeSkeleton( 0 ), m_activeMenuIndex( -1 )
{
    m_contextMenu = new MarbleWidgetPopupMenu( m_marbleWidget, m_marbleWidget->model() );
    m_removeViaPointAction = new QAction( QObject::tr( "&Remove this destination" ), q );
    QObject::connect( m_removeViaPointAction, SIGNAL( triggered() ), q, SLOT( removeViaPoint() ) );
    m_contextMenu->addAction( Qt::RightButton, m_removeViaPointAction );
    QAction *exportAction = new QAction( QObject::tr( "&Export route..." ), q );
    QObject::connect( exportAction, SIGNAL( triggered() ), q, SIGNAL( exportRequested() ) );
    m_contextMenu->addAction( Qt::RightButton, exportAction );
}

void RoutingLayerPrivate::showContextMenu( const QPoint &pos )
{
    m_contextMenu->showRmbMenu( pos.x(), pos.y() );
}

void RoutingLayerPrivate::renderPlacemarks( GeoPainter *painter )
{
    m_placemarks.clear();
    painter->setPen( QColor( Qt::black ) );
    for ( int i=0; i<m_placemarkModel->rowCount(); ++i ) {
        QModelIndex index = m_placemarkModel->index( i,0 );
        QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );
        if ( index.isValid() && !data.isNull() ) {
            GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>( data );

            QPixmap pixmap = qVariantValue<QPixmap>( index.data( Qt::DecorationRole ) );
            if ( !pixmap.isNull() && m_selectionModel->isSelected( index ) ) {
                QIcon selected = QIcon( pixmap );
                QPixmap result = selected.pixmap( m_pixmapSize, QIcon::Selected, QIcon::On );
                painter->drawPixmap( pos, result );
            } else {
                painter->drawPixmap( pos, pixmap );
            }

            QRegion region = painter->regionFromRect( pos, m_targetPixmap.width(), m_targetPixmap.height() );
            m_placemarks.push_back( ModelRegion( index,region ) );
        }
    }
}

void RoutingLayerPrivate::renderRoute( GeoPainter *painter )
{
    m_instructionRegions.clear();
    GeoDataLineString waypoints;

    for ( int i=0; i<m_routingModel->rowCount(); ++i ) {
        QModelIndex index = m_routingModel->index( i,0 );
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>( index.data( RoutingModel::CoordinateRole ) );
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>( index.data( RoutingModel::TypeRole ) );

        if ( type == RoutingModel::WayPoint )
        {
            waypoints << pos;
        }
    }

    QPen bluePen( QColor::fromRgb( 0,87,174,200 ) ); // blue, oxygen palette
    bluePen.setWidth( 5 );
    if ( m_routeDirty ) {
        bluePen.setStyle( Qt::DotLine );
    }
    painter->setPen( bluePen );

    painter->drawPolyline( waypoints );
    m_routeRegion = painter->regionFromPolyline( waypoints, 8 );

    bluePen.setWidth( 2 );
    painter->setPen( bluePen );
    painter->setBrush( QBrush( QColor::fromRgb( 136,138,133,200 ) ) ); // gray, oxygen palette

    if ( !m_insertStopOver.isNull() ) {
        int dx = 1 + m_pixmapSize.width() / 2;
        int dy = 1 + m_pixmapSize.height() / 2;
        QPoint center = m_insertStopOver - QPoint( dx,dy );
        painter->drawPixmap( center, m_targetPixmap );
    }

    for ( int i=0; i<m_routingModel->rowCount(); ++i )
    {
        QModelIndex index = m_routingModel->index( i,0 );
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>( index.data( RoutingModel::CoordinateRole ) );
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>( index.data( RoutingModel::TypeRole ) );

        if ( type == RoutingModel::Instruction ) {

            painter->setBrush( QBrush( QColor::fromRgb( 136,138,133,200 ) ) ); // gray, oxygen palette
            QModelIndex proxyIndex = m_proxyModel->mapFromSource( index );
            if ( m_selectionModel->selection().contains( proxyIndex ) )
            {
                painter->setPen( QColor( Qt::black ) );
                painter->setBrush( QBrush( QColor::fromRgb( 227,173,0,200 ) ) ); // yellow, oxygen palette
                painter->drawAnnotation( pos, index.data().toString(), QSize( 120,60 ), 10, 30, 15, 15 );

                painter->setPen( bluePen );
                painter->setBrush( QBrush( QColor::fromRgb( 236,115,49,200 ) ) ); // orange, oxygen palette
            }

            QRegion region = painter->regionFromEllipse( pos, 12, 12 );
            m_instructionRegions.push_front( ModelRegion( index,region ) );
            painter->drawEllipse( pos, 8, 8 );
        }
    }
}

void RoutingLayerPrivate::renderSkeleton( GeoPainter *painter )
{
    m_regions.clear();
    for ( int i=0; i<m_routeSkeleton->size(); ++i ) {
        GeoDataCoordinates pos = m_routeSkeleton->at( i );
        if ( pos.longitude() != 0.0 && pos.latitude() != 0.0 ) {
            QPixmap pixmap = m_routeSkeleton->pixmap( i );
            painter->drawPixmap( pos, pixmap );
            QRegion region = painter->regionFromRect( pos, pixmap.width(), pixmap.height() );
            m_regions.push_front( SkeletonRegion( i,region ) );
        }
    }
}

bool RoutingLayerPrivate::handleMouseButtonPress( QMouseEvent *e )
{
    if ( m_pointSelection ) {
        return true;
    }

    foreach( const SkeletonRegion &region, m_regions ) {
        if ( region.region.contains( e->pos() ) ) {
            if ( e->button() == Qt::LeftButton ) {
                m_movingIndex = region.index;
                m_insertStopOver = QPoint();
                m_dragStopOver = false;
                return true;
            }
            else if ( e->button() == Qt::RightButton ) {
                m_removeViaPointAction->setEnabled( true );
                m_activeMenuIndex = region.index;
                showContextMenu( e->pos() );
                return true;
            }
            else
                return false;
        }
    }

    foreach( const ModelRegion &region, m_instructionRegions ) {
        if ( region.region.contains( e->pos() ) ) {
            if ( e->button() == Qt::LeftButton ) {
                QModelIndex index = m_proxyModel->mapFromSource( region.index );
                m_selectionModel->select( index, QItemSelectionModel::ClearAndSelect );
                m_insertStopOver = QPoint();
                m_dragStopOver = false;
                return true;
            }
            else if ( e->button() == Qt::RightButton ) {
                m_removeViaPointAction->setEnabled( false );
                showContextMenu( e->pos() );
                return true;
            }
            else
                return false;
        }
    }

    if ( m_routeRegion.contains( e->pos() ) ) {
        if ( e->button() == Qt::LeftButton ) {
            /** @todo: Determine the neighbored via points and insert in order */
            m_insertStopOver = e->pos();
            m_dragStopOver = true;
            return true;
        }
        else if ( e->button() == Qt::RightButton ) {
            m_removeViaPointAction->setEnabled( false );
            showContextMenu( e->pos() );
            return true;
        }
        else
            return false;
    }

    if ( e->button() != Qt::LeftButton ) {
        return false;
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

    if ( m_pointSelection ) {
        qreal lon( 0.0 ), lat( 0.0 );
        if ( m_marbleWidget->geoCoordinates( e->pos().x(), e->pos().y(),
                                           lon, lat, GeoDataCoordinates::Radian ) ) {
            emit q->pointSelected( GeoDataCoordinates( lon,lat ) );
            return true;
        }
    }

    if ( m_movingIndex >= 0 ) {
        m_movingIndex = -1;
        clearStopOver();
        emit q->routeDirty();
        return true;
    }

    if ( !m_insertStopOver.isNull() ) {
        qreal lon( 0.0 ), lat( 0.0 );
        if ( m_marbleWidget->geoCoordinates( m_insertStopOver.x(), m_insertStopOver.y(), lon, lat, GeoDataCoordinates::Radian ) ) {
            GeoDataCoordinates position( lon, lat );
            m_routeSkeleton->addVia( position );
            clearStopOver();
            emit q->routeDirty();
            return true;
        }
    }

    return false;
}

bool RoutingLayerPrivate::handleMouseMove( QMouseEvent *e )
{
    if ( m_pointSelection ) {
        m_marbleWidget->setCursor( Qt::CrossCursor );
        return true;
    }

    if ( !m_routingModel ) {
        return false;
    }

    qreal lon( 0.0 ), lat( 0.0 );
    if ( m_marbleWidget->geoCoordinates( e->pos().x(), e->pos().y(),
                                       lon, lat, GeoDataCoordinates::Radian ) ) {

        if ( m_movingIndex >= 0 ) {
            GeoDataCoordinates moved( lon,lat );
            m_routeSkeleton->setPosition( m_movingIndex, moved );
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        }
        else if ( m_dragStopOver ) {
            m_insertStopOver = e->pos();
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( isInfoPoint( e->pos() ) ) {
            clearStopOver();
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( m_routeRegion.contains( e->pos() ) ) {
            m_insertStopOver = e->pos();
            m_marbleWidget->setCursor( Qt::ArrowCursor );
        } else if ( !m_insertStopOver.isNull() ) {
            clearStopOver();
        } else {
            return false;
        }

        // Update pixmap in the map (old and new position needs repaint)
        paintStopOver( QRect( e->pos(), m_pixmapSize ) );
        return true;
    }

    return false;
}

bool RoutingLayerPrivate::handleKeyEvent( QKeyEvent *e )
{
    if ( m_pointSelection && e->key() == Qt::Key_Escape ) {
        m_pointSelection = false;
        emit q->pointSelectionAborted();
        return true;
    }

    return false;
}

bool RoutingLayerPrivate::isInfoPoint( const QPoint &point )
{
    foreach( const SkeletonRegion &region, m_regions ) {
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

void RoutingLayerPrivate::paintStopOver( QRect dirty )
{
    m_marbleWidget->repaint( m_movingIndexDirtyRect );
    int dx = 1 + m_pixmapSize.width() / 2;
    int dy = 1 + m_pixmapSize.height() / 2;
    dirty.adjust( -dx, -dy, -dx, -dy );
    m_marbleWidget->repaint( dirty );
    m_movingIndexDirtyRect = dirty;
}

void RoutingLayerPrivate::clearStopOver()
{
    m_insertStopOver = QPoint();
    m_dragStopOver = false;
    m_marbleWidget->repaint( m_movingIndexDirtyRect );
}

RoutingLayer::RoutingLayer( MarbleWidget *widget, QWidget *parent ) :
        QObject( parent ), d( new RoutingLayerPrivate( this, widget ) )
{
    widget->installEventFilter( this );
}

RoutingLayer::~RoutingLayer()
{
    delete d;
}

QStringList RoutingLayer::renderPosition() const
{
    return QStringList() << "HOVERS_ABOVE_SURFACE";
}

bool RoutingLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                 const QString& renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, true );

    if ( d->m_placemarkModel) {
        d->renderPlacemarks( painter );
    }

    if ( d->m_routingModel) {
        d->renderRoute( painter );
    }

    if ( d->m_routeSkeleton) {
        d->renderSkeleton( painter );
    }

    painter->restore();
    return true;
}

bool RoutingLayer::eventFilter( QObject *obj, QEvent *event )
{
    Q_UNUSED( obj )

    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *e = static_cast<QKeyEvent*>( event );
        return d->handleKeyEvent( e );
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

void RoutingLayer::setModel ( RoutingModel *model )
{
    d->m_placemarkModel = 0;
    d->m_routingModel = model;
}

void RoutingLayer::setModel ( MarblePlacemarkModel *model )
{
    d->m_routingModel = 0;
    d->m_placemarkModel = model;
}

void RoutingLayer::synchronizeWith( QAbstractProxyModel *model, QItemSelectionModel *selection )
{
    d->m_selectionModel = selection;
    d->m_proxyModel = model;
}

void RoutingLayer::setPointSelectionEnabled( bool enabled )
{
    d->m_pointSelection = enabled;
}

void RoutingLayer::setRouteDirty( bool dirty )
{
    d->m_routeDirty = dirty;

    /** @todo: The full repaint can be avoided. The route however has changed
      * and the exact bounding box needs to be recalculated before doing
      * a partly repaint, otherwise we might end up repainting only parts of the route
      */
    // d->m_marbleWidget->repaint( d->m_routeRegion );
    d->m_marbleWidget->repaint();
}

void RoutingLayer::setRouteSkeleton( RouteSkeleton *skeleton )
{
    d->m_routeSkeleton = skeleton;
}

void RoutingLayer::removeViaPoint()
{
    if ( d->m_activeMenuIndex >= 0 ) {
        d->m_routeSkeleton->remove( d->m_activeMenuIndex );
        d->m_activeMenuIndex = -1;
        setRouteDirty( true );
        emit routeDirty();
    }
}

} // namespace Marble

#include "RoutingLayer.moc"
