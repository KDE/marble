//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
//

#include "MarbleGeoDataView.h"

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

// Qt
#include <QtCore/QDebug>
#include <QtGui/QPaintEvent>

using namespace Marble;

MarbleGeoDataView::MarbleGeoDataView( QWidget * parent ) 
  : QAbstractItemView( parent )
{
}

void MarbleGeoDataView::setGeoPainter( GeoPainter* painter )
{
    m_painter = painter;
    /* the paintEvent function has to called by hand as the view is not 
     * connected to a widget (where you normally would get the event from) */
    if( model() ) paintEvent( 0 );
}

QRect MarbleGeoDataView::visualRect( const QModelIndex &index ) const
{
    return QRect();
}

void MarbleGeoDataView::scrollTo( const QModelIndex &index, ScrollHint hint )
{
}

QModelIndex MarbleGeoDataView::indexAt( const QPoint &point ) const
{
    return QModelIndex();
}

QModelIndex MarbleGeoDataView::moveCursor( QAbstractItemView::CursorAction cursorAction,
                    Qt::KeyboardModifiers modifiers )
{
    return QModelIndex();
}

bool MarbleGeoDataView::isIndexHidden( const QModelIndex &index ) const
{
    return false;
}

void MarbleGeoDataView::setSelection( const QRect&, QItemSelectionModel::SelectionFlags command )
{
}

void MarbleGeoDataView::paintEvent( QPaintEvent *event )
{
    QModelIndex index = rootIndex();
    renderIndex( index );
}

void MarbleGeoDataView::renderIndex( QModelIndex &index )
{
    /*
     * "render" a specific index - this means going through all children and if
     * one can be rendered (if it is a Geometry object which is not a container)
     * then call the real render function. For the rest iterate through the
     * children and recurse.
     */
    for ( int row = 0; row < model()->rowCount( index ); ++row )
    {
        QModelIndex childIndex = model()->index( row, 0, index );
        QString output = model()->data( childIndex ).toString();
        GeoDataObject* object = model()->data( childIndex, Qt::UserRole + 11 ).value<Marble::GeoDataObject*>();

        if( dynamic_cast<GeoDataGeometry*>( object ) ) {
            if( static_cast<GeoDataGeometry*>( object )->geometryId() != GeoDataMultiGeometryId ) {
                renderGeoDataGeometry( static_cast<GeoDataGeometry*>( object ), styleUrl );
            } else {
                if( childIndex.isValid() && model()->rowCount( childIndex ) > 0 ) {
                    renderIndex( childIndex );
                }
            }
        }
        else if( dynamic_cast<GeoDataFeature*>( object ) ) {
            if( dynamic_cast<GeoDataFeature*>( object )->featureId() == GeoDataPlacemarkId ) {
                GeoDataPlacemark placemark( *static_cast<GeoDataFeature*>( object ) );
                styleUrl = placemark.styleUrl();
            }

            if( childIndex.isValid() && model()->rowCount( childIndex ) > 0 ) {
                renderIndex( childIndex );
            }
        }
    }
}

QRegion MarbleGeoDataView::visualRegionForSelection( const QItemSelection &selection ) const
{
    return QRegion();
}

void MarbleGeoDataView::setBrushStyle( QString mapped )
{
    /* this part has to be reworked:
     * Currently the style has to be accessible from the root object of the 
     * model. This might not be wanted. On the other hand - is a copy of the
     * style within every Placemark wanted and how should this be called here?
     */
    if( m_currentBrush.color() != m_root->style( mapped ).polyStyle().color() ) {
/*            qDebug() << "BrushColor:" 
                 << m_root->style( mapped ).polyStyle()->color() 
                 << m_currentBrush.color();*/
        m_currentBrush.setColor( m_root->style( mapped ).polyStyle().color() );
        m_painter->setBrush( m_currentBrush.color() );
    }
}

void MarbleGeoDataView::setPenStyle( QString mapped )
{
    /*
     * see the note in the setBrushStyle function
     */
    if( m_currentPen.color() != m_root->style( mapped ).lineStyle().color() || 
        m_currentPen.widthF() != m_root->style( mapped ).lineStyle().width() ) {
/*            qDebug() << "PenColor:" 
                 << m_root->style( mapped ).lineStyle().color() 
                 << m_currentPen.color();
        qDebug() << "PenWidth:" 
                 << m_root->style( mapped ).lineStyle().width() 
                 << m_currentPen.widthF();*/
        m_currentPen.setColor( m_root->style( mapped ).lineStyle().color() );
        m_currentPen.setWidthF( m_root->style( mapped ).lineStyle().width() );
    }
    if ( m_painter->mapQuality() != Marble::High && m_painter->mapQuality() != Marble::Print )
    {
//            m_currentPen.setWidth( 0 );
        QColor penColor = m_currentPen.color();
        penColor.setAlpha( 255 );
        m_currentPen.setColor( penColor );
    }
    m_painter->setPen( m_currentPen );
}

bool MarbleGeoDataView::renderGeoDataGeometry( GeoDataGeometry *object, QString styleUrl )
{
    m_painter->save();
    m_painter->autoMapQuality();

    m_root = dynamic_cast<GeoDataDocument*>( model()->data( rootIndex(), 
                 Qt::UserRole + 11 ).value<Marble::GeoDataObject*>() );

    if( !m_root ) {
        qWarning() << "root seems to be 0!!!";
        return false;
    }

    /// hard coded to use only the "normal" style
    QString mapped = styleUrl;
    const GeoDataStyleMap& styleMap = m_root->styleMap( styleUrl.remove( '#' ) );

    mapped = styleMap.value( QString( "normal" ) );
    mapped.remove( '#' );

    if( object->geometryId() == GeoDataPolygonId ) {
        setBrushStyle( mapped );
        setPenStyle( mapped );
        m_painter->drawPolygon( *static_cast<GeoDataPolygon*>( object ) );
    }
    if( object->geometryId() == GeoDataLinearRingId ) {
        m_painter->setBrush( QColor( 0, 0, 0, 0 ) );
        setPenStyle( mapped );
        m_painter->drawPolygon( *static_cast<GeoDataLinearRing*>( object ) );
    }
    if( object->geometryId() == GeoDataLineStringId ) {
        setPenStyle( mapped );
        m_painter->drawPolyline( *static_cast<GeoDataLineString*>( object ) );
    }
    /* Note: GeoDataMultiGeometry is handled within the model */
    m_painter->restore();
    return true;
}
