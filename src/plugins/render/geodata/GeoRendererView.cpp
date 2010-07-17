//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
//

#include "GeoRendererView.h"

// Marble
#include "GeoDataContainer.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataLineStyle.h"
#include "GeoDataObject.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "MarbleDebug.h"
#include "GeoDataTypes.h"

#include "GeoPainter.h"

// Qt
#include <QtGui/QPaintEvent>

using namespace Marble;

GeoRendererView::GeoRendererView( QWidget * parent ) 
  : QAbstractItemView( parent )
{
}

void GeoRendererView::setGeoPainter( GeoPainter* painter )
{
    m_painter = painter;
    /* the paintEvent function has to called by hand as the view is not 
     * connected to a widget ( where you normally would get the event from ) */
    if( model() ) paintEvent( 0 );
}

QRect GeoRendererView::visualRect( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return QRect();
}

void GeoRendererView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
    Q_UNUSED( index )
    Q_UNUSED( hint )
}

QModelIndex GeoRendererView::indexAt( const QPoint &point ) const
{
    Q_UNUSED( point )

    return QModelIndex();
}

QModelIndex GeoRendererView::moveCursor( QAbstractItemView::CursorAction cursorAction,
                    Qt::KeyboardModifiers modifiers )
{
    Q_UNUSED( cursorAction )
    Q_UNUSED( modifiers )

    return QModelIndex();
}

bool GeoRendererView::isIndexHidden( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return false;
}

void GeoRendererView::setSelection( const QRect&, QItemSelectionModel::SelectionFlags command )
{
    Q_UNUSED( command )
}

void GeoRendererView::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event )

    QModelIndex index = rootIndex();
    renderIndex( index );
}

void GeoRendererView::renderIndex( QModelIndex &index )
{
    /*
     * "render" a specific index - this means going through all children and if
     * one can be rendered ( if it is a Geometry object which is not a container )
     * then call the real render function. For the rest iterate through the
     * children and recurse.
     */
    if ( index.isValid() ) {
        GeoDataObject* indexObject = static_cast<GeoDataObject*>( index.internalPointer() );        
        if ( dynamic_cast<GeoDataDocument*>( indexObject ) ) {
            m_root = static_cast<GeoDataDocument*>( indexObject );
            if ( !m_root->isVisible() ) {
                return;
            }
        }
        GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( indexObject );
        if ( feature && !feature->isVisible() ) {
            return;
        }
    }

    int rowCount = model()->rowCount( index );
   
    for ( int row = 0; row < rowCount; ++row )
    {
        QModelIndex childIndex = model()->index( row, 0, index );
        GeoDataObject* object = static_cast<GeoDataObject*>( childIndex.internalPointer() );

        if( dynamic_cast<GeoDataGeometry*>( object ) ) {
            if( static_cast<GeoDataGeometry*>( object )->geometryId() != GeoDataMultiGeometryId ) {
                renderGeoDataGeometry( static_cast<GeoDataGeometry*>( object ), m_styleUrl );
            } else {
                if( childIndex.isValid() && model()->rowCount( childIndex ) > 0 ) {
                    renderIndex( childIndex );
                }
            }
        }
        else if( dynamic_cast<GeoDataFeature*>( object ) ) {
            if( dynamic_cast<GeoDataPlacemark*>( object ) ) {
                GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
                m_styleUrl = placemark->styleUrl();
            }

            if( childIndex.isValid() && model()->rowCount( childIndex ) > 0 ) {
                renderIndex( childIndex );
            }
        }
    }
}

QRegion GeoRendererView::visualRegionForSelection( const QItemSelection &selection ) const
{
    Q_UNUSED( selection )

    return QRegion();
}

void GeoRendererView::setBrushStyle( QString mapped )
{
    /* this part has to be reworked:
     * Currently the style has to be accessible from the root object of the 
     * model. This might not be wanted. On the other hand - is a copy of the
     * style within every Placemark wanted and how should this be called here?
     */
    if( !m_root->style( mapped ).polyStyle().fill() ) {
        if( m_painter->brush().color() != Qt::transparent )
        m_painter->setBrush( QColor( Qt::transparent ) );
        return;
    }

    if( m_painter->brush().color() != m_root->style( mapped ).polyStyle().color() ) {
/*        mDebug() << "BrushColor:" 
                 << m_root->style( mapped ).polyStyle().color() 
                 << m_painter->brush().color();*/
        m_painter->setBrush( m_root->style( mapped ).polyStyle().color() );
    }
}

void GeoRendererView::setPenStyle( QString mapped )
{
    /*
     * see the note in the setBrushStyle function
     */
    if( !m_root->style( mapped ).polyStyle().outline() ) {
        m_currentPen.setColor( Qt::transparent );
        if( m_painter->pen() != m_currentPen ) m_painter->setPen( m_currentPen );
        return;
    }

    if( m_currentPen.color() != m_root->style( mapped ).lineStyle().color() || 
        m_currentPen.widthF() != m_root->style( mapped ).lineStyle().width() ) {
/*            mDebug() << "PenColor:" 
                 << m_root->style( mapped ).lineStyle().color() 
                 << m_currentPen.color();
        mDebug() << "PenWidth:" 
                 << m_root->style( mapped ).lineStyle().width() 
                 << m_currentPen.widthF();*/
        m_currentPen.setColor( m_root->style( mapped ).lineStyle().color() );
        m_currentPen.setWidthF( m_root->style( mapped ).lineStyle().width() );
    }

    if (    m_painter->mapQuality() != Marble::HighQuality
         && m_painter->mapQuality() != Marble::PrintQuality )
    {
//            m_currentPen.setWidth( 0 );
        QColor penColor = m_currentPen.color();
        penColor.setAlpha( 255 );
        m_currentPen.setColor( penColor );
    }

    if( m_painter->pen() != m_currentPen ) m_painter->setPen( m_currentPen );
}

bool GeoRendererView::renderGeoDataGeometry( GeoDataGeometry *object, QString styleUrl )
{
    m_painter->save();
    m_painter->autoMapQuality();

    if( !m_root ) {
        qWarning() << "root seems to be 0!!!";
        return false;
    }

    QString mapped = styleUrl;
    const GeoDataStyleMap& styleMap = m_root->styleMap( styleUrl.remove( '#' ) );

    /// hard coded to use only the "normal" style
    if( !styleMap.value( QString( "normal" ) ).isEmpty() ) {
        mapped = styleMap.value( QString( "normal" ) );
    }

    mapped.remove( '#' );

    if( object->geometryId() == GeoDataPolygonId ) {
        setBrushStyle( mapped );
        setPenStyle( mapped );
        // geometries are implicitly shared, this shouldn't hurt
        GeoDataPolygon polygon( *object );
        m_painter->drawPolygon( polygon );
    }
    if( object->geometryId() == GeoDataLinearRingId ) {
        m_painter->setBrush( QColor( 0, 0, 0, 0 ) );
        setPenStyle( mapped );
        GeoDataLinearRing linearRing( *object );
        m_painter->drawPolygon( linearRing );
    }
    if( object->geometryId() == GeoDataLineStringId ) {
        setPenStyle( mapped );
        GeoDataLineString lineString( *object );
        m_painter->drawPolyline( lineString );
    }
    /* Note: GeoDataMultiGeometry is handled within the model */
    m_painter->restore();
    return true;
}
