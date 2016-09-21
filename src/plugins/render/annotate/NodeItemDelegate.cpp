//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu  <stanciumarius94@gmail.com>
//

// Self
#include "NodeItemDelegate.h"

// Qt
#include <QSize>
#include <QStyleOptionViewItem>

// Marble
#include "LatLonEdit.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"

namespace  Marble
{

QSize NodeItemDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    Q_UNUSED( option );
    Q_UNUSED( index );
    return QSize( 25, 25 );
}

NodeItemDelegate::NodeItemDelegate( GeoDataPlacemark* placemark, QTreeView* view ):
    m_placemark( placemark ), m_view( view )
{
}

QWidget* NodeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    Q_UNUSED( option );
    Q_UNUSED( index );
    LatLonEdit *editor = new LatLonEdit( parent );
    connect( this, SIGNAL(closeEditor(QWidget*)),
             this, SLOT(unsetCurrentEditor(QWidget*)) );
    return editor;
}

void NodeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    LatLonEdit *latLonEditWidget = static_cast<LatLonEdit*>(editor);
    qreal value = 0;

    if( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {

        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( m_placemark->geometry() );

        GeoDataLinearRing outerBoundary = polygon->outerBoundary();

        // Setting the latlonedit spinboxes values
        if( index.column() == 1 ) {
            latLonEditWidget->setDimension( Marble::Longitude );
            value = outerBoundary.at( index.row() ).longitude( GeoDataCoordinates::Degree );
        }
        else {
            latLonEditWidget->setDimension( Marble::Latitude );
            value = outerBoundary.at( index.row() ).latitude( GeoDataCoordinates::Degree );
        }
    }
    else if ( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {

        GeoDataLineString *lineString = static_cast<GeoDataLineString*>( m_placemark->geometry() );

        // Setting the latlonedit spinboxes values
        if( index.column() == 1 ) {
            latLonEditWidget->setDimension( Marble::Longitude );
            value = lineString->at( index.row() ).longitude( GeoDataCoordinates::Degree );
        }
        else {
            latLonEditWidget->setDimension( Marble::Latitude );
            value = lineString->at( index.row() ).latitude(GeoDataCoordinates::Degree );
        }
    }

    latLonEditWidget->setValue( value );

    connect( latLonEditWidget, SIGNAL(valueChanged(qreal)),
             this, SLOT(previewNodeMove(qreal)) );
    m_indexBeingEdited = index;

}

void NodeItemDelegate::setModelData( QWidget* editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    Q_UNUSED( editor );
    Q_UNUSED( model );
    Q_UNUSED( index );

    // The dialogs already have a function that updates the NodeModel
    emit modelChanged( m_placemark );
}

void NodeItemDelegate::previewNodeMove( qreal value )
{
    if( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( m_placemark->geometry() );
        GeoDataLinearRing outerBoundary = polygon->outerBoundary();

        GeoDataCoordinates* coordinates = new GeoDataCoordinates( outerBoundary[m_indexBeingEdited.row()] );

        if( m_indexBeingEdited.column() == 1) {
            coordinates->setLongitude( value, GeoDataCoordinates::Degree );
        }
        else {
            coordinates->setLatitude( value, GeoDataCoordinates::Degree );
        }

        outerBoundary[ m_indexBeingEdited.row() ] = *coordinates;
        polygon->setOuterBoundary( outerBoundary );
    }
    else if ( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {

        GeoDataLineString *lineString = static_cast<GeoDataLineString*>( m_placemark->geometry() );
        GeoDataCoordinates* coordinates = new GeoDataCoordinates( lineString->at( m_indexBeingEdited.row() ) );

        if( m_indexBeingEdited.column() == 1) {
            coordinates->setLongitude( value, GeoDataCoordinates::Degree );
        }
        else {
            coordinates->setLatitude( value, GeoDataCoordinates::Degree );
        }

        lineString->at( m_indexBeingEdited.row() ) = *coordinates;

    }

    // Updating chagnes ( repainting graphics )
    emit geometryChanged();
}

void NodeItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if( index.column() == 1) {
         m_view->setColumnWidth( 1, 200 );
         m_view->setColumnWidth( 2, 100 );
    }
    else {
        m_view->setColumnWidth( 2, 200 );
        m_view->setColumnWidth( 1, 100 );
    }

    editor->setGeometry( option.rect );
}

void NodeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    drawBackground( painter, option, index );

    // The LatLonEdit widget is transparent, so we have to disable the text behind it
    // for esthetic reasons.
    if ( !( index == m_indexBeingEdited ) || !( index == m_view->currentIndex() ) ) {
        drawDisplay( painter, option, option.rect, index.data().toString() );
    }
}

void NodeItemDelegate::unsetCurrentEditor(QWidget* widget)
{
    Q_UNUSED( widget );
    m_indexBeingEdited = QModelIndex();
    m_view->viewport()->update();
}

}

#include "moc_NodeItemDelegate.cpp"
