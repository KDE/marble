//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>"
//

#include "PlaceMarkPainter.h"

#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

#include "AbstractProjection.h"
#include "GeoDataStyle.h"
#include "MarblePlacemarkModel.h"
#include "ViewportParams.h"
#include "VisiblePlaceMark.h"

using namespace Marble;

PlaceMarkPainter::PlaceMarkPainter( QObject* parent )
    : QObject( parent )
{
    m_useXWorkaround = testXBug();
    qDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );

    m_defaultLabelColor = Qt::black;
}

PlaceMarkPainter::~PlaceMarkPainter()
{
}

void PlaceMarkPainter::setDefaultLabelColor( const QColor& color ){
    m_defaultLabelColor = color;
}

void PlaceMarkPainter::drawPlaceMarks( QPainter* painter, 
                                       QVector<VisiblePlaceMark*> visiblePlaceMarks,
                                       const QItemSelection &selection, 
                                       ViewportParams *viewport )
{
    QVector<VisiblePlaceMark*>::const_iterator visit = visiblePlaceMarks.constEnd();
    QVector<VisiblePlaceMark*>::const_iterator itEnd = visiblePlaceMarks.constBegin();

    VisiblePlaceMark *mark = 0;
    int imageWidth = viewport->width();

    while ( visit != itEnd ) {
	--visit;
	mark = *visit;

	if ( mark->labelPixmap().isNull() ) {
	    bool isSelected = selection.contains( mark->modelIndex() );
	    drawLabelPixmap( mark, isSelected );
	}

	painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
	painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
    }

    visit = visiblePlaceMarks.constEnd();

    while ( visit != itEnd ) {
	--visit;
	mark = *visit;

	if ( mark->labelPixmap().isNull() ) {
	    bool isSelected = selection.contains( mark->modelIndex() );
	    drawLabelPixmap( mark, isSelected );
	}

	painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
	painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );

	if ( ! viewport->currentProjection()->repeatX() )
	    continue;

	int tempSymbol = mark->symbolPosition().x();
	int tempText =   mark->labelRect().x();

	for ( int i = tempSymbol - 4 * viewport->radius();
	      i >= 0;
	      i -= 4 * viewport->radius() )
	{
	    QRect labelRect( mark->labelRect() );
	    labelRect.moveLeft(i - tempSymbol + tempText );
	    mark->setLabelRect( labelRect );

	    QPoint symbolPos( mark->symbolPosition() );
	    symbolPos.setX( i );
	    mark->setSymbolPosition( symbolPos );

	    painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
	    painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
	}

	for ( int i = tempSymbol;
	      i <= imageWidth;
	      i += 4 * viewport->radius() )
        {
	    QRect labelRect( mark->labelRect() );
	    labelRect.moveLeft(i - tempSymbol + tempText );
	    mark->setLabelRect( labelRect );

	    QPoint symbolPos( mark->symbolPosition() );
	    symbolPos.setX( i );
	    mark->setSymbolPosition( symbolPos );

	    painter->drawPixmap( mark->symbolPosition(), mark->symbolPixmap() );
	    painter->drawPixmap( mark->labelRect(), mark->labelPixmap() );
	}
    }
}

inline void PlaceMarkPainter::drawLabelText(QPainter& labelPainter,
                                            const QString &name, const QFont &labelFont )
{
    QFont font = labelFont;
    font.setWeight( 75 );

    QPen    outlinepen( Qt::white );
    outlinepen.setWidthF( s_labelOutlineWidth );
    QBrush  outlinebrush( Qt::black );

    QPainterPath   outlinepath;

    int fontascent = QFontMetrics( font ).ascent();
    const QPointF  baseline( s_labelOutlineWidth / 2.0, fontascent );
    outlinepath.addText( baseline, font, name );
    labelPainter.setRenderHint( QPainter::Antialiasing, true );
    labelPainter.setPen( outlinepen );
    labelPainter.setBrush( outlinebrush );
    labelPainter.drawPath( outlinepath );
    labelPainter.setPen( Qt::NoPen );
    labelPainter.drawPath( outlinepath );
    labelPainter.setRenderHint( QPainter::Antialiasing, false );
}

inline void PlaceMarkPainter::drawLabelPixmap( VisiblePlaceMark *mark, bool isSelected )
{

    QPainter labelPainter;
    QPixmap labelPixmap;
    QModelIndex index = mark->modelIndex();
    GeoDataStyle* style = ( ( MarblePlacemarkModel* )index.model() )->styleData( index );
//    GeoDataStyle* style = index.data( MarblePlacemarkModel::StyleRole ).value<GeoDataStyle*>();

    QString labelName = mark->name();
    QRect  labelRect  = mark->labelRect();
    QFont  labelFont  = style->labelStyle()->font();
    QColor labelColor = style->labelStyle()->color();

    // FIXME: To be removed after MapTheme / KML refactoring
    if ( ( labelColor == Qt::black || labelColor == QColor( "#404040" ) )
	 && m_defaultLabelColor != Qt::black )
        labelColor = m_defaultLabelColor;

    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases (at least with Qt 4.2).
    if ( !m_useXWorkaround ) {
        labelPixmap = QPixmap( labelRect.size() );
        labelPixmap.fill( Qt::transparent );

        labelPainter.begin( &labelPixmap );

        if ( !isSelected ) {
            labelPainter.setFont( labelFont );
            labelPainter.setPen( labelColor );
            int fontascent = QFontMetrics( labelFont ).ascent();
            labelPainter.drawText( 0, fontascent, labelName );
        }
        else {
            drawLabelText( labelPainter, labelName, labelFont  );
        }

        labelPainter.end();
    } else {

        QImage image( labelRect.size(),
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        labelPainter.begin( &image );

        if ( !isSelected ) {
            labelPainter.setFont( labelFont );
            labelPainter.setPen( labelColor );
            int fontascent = QFontMetrics( labelFont ).ascent();
            labelPainter.drawText( 0, fontascent, labelName );
        }
        else {
            drawLabelText( labelPainter, labelName, labelFont );
        }

        labelPainter.end();

        labelPixmap = QPixmap::fromImage( image );
    }

    mark->setLabelPixmap( labelPixmap );
}


// Test if there a bug in the X server which makes 
// text fully transparent if it gets written on 
// QPixmaps that were initialized by filling them 
// with Qt::transparent

bool PlaceMarkPainter::testXBug()
{
    QString  testchar( "K" );
    QFont    font( "Sans Serif", 10 );

    int fontheight = QFontMetrics( font ).height();
    int fontwidth  = QFontMetrics( font ).width(testchar);
    int fontascent = QFontMetrics( font ).ascent();

    QPixmap  pixmap( fontwidth, fontheight );
    pixmap.fill( Qt::transparent );

    QPainter textpainter;
    textpainter.begin( &pixmap );
    textpainter.setPen( QColor( 0, 0, 0, 255 ) );
    textpainter.setFont( font );
    textpainter.drawText( 0, fontascent, testchar );
    textpainter.end();

    QImage image = pixmap.toImage();

    for ( int x = 0; x < fontwidth; ++x ) {
        for ( int y = 0; y < fontheight; ++y ) {
            if ( qAlpha( image.pixel( x, y ) ) > 0 )
                return false;
        }
    }

    return true;
}

#include "PlaceMarkPainter.moc"

