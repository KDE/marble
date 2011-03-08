//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

#include "PlacemarkPainter.h"

#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "GeoDataStyle.h"
#include "MarblePlacemarkModel.h"
#include "ViewportParams.h"
#include "VisiblePlacemark.h"

using namespace Marble;

PlacemarkPainter::PlacemarkPainter( QObject* parent )
    : QObject( parent )
{
    m_useXWorkaround = testXBug();
    mDebug() << "Use workaround: " << ( m_useXWorkaround ? "1" : "0" );

    m_defaultLabelColor = Qt::black;
}

PlacemarkPainter::~PlacemarkPainter()
{
}

void PlacemarkPainter::setDefaultLabelColor( const QColor& color )
{
    m_defaultLabelColor = color;
}

void PlacemarkPainter::drawPlacemarks( QPainter* painter, 
                                       QVector<VisiblePlacemark*> visiblePlacemarks,
                                       const QItemSelection &selection, 
                                       ViewportParams *viewport )
{
    QVector<VisiblePlacemark*>::const_iterator visit = visiblePlacemarks.constEnd();
    QVector<VisiblePlacemark*>::const_iterator itEnd = visiblePlacemarks.constBegin();

    VisiblePlacemark *mark = 0;
    int imageWidth = viewport->width();

    while ( visit != itEnd ) {
	--visit;
	mark = *visit;

	if ( mark->labelPixmap().isNull() ) {
            bool isSelected = false;
            foreach ( QModelIndex index, selection.indexes() ) {
                GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) ));
                if (mark->placemark() == placemark ) {
                    isSelected = true;
                    break;
                }
            }
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

inline void PlacemarkPainter::drawLabelText(QPainter& labelPainter,
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

inline void PlacemarkPainter::drawLabelPixmap( VisiblePlacemark *mark, bool isSelected )
{

    QPainter labelPainter;
    QPixmap labelPixmap;

    const GeoDataPlacemark *placemark = mark->placemark();
    GeoDataStyle* style = placemark->style();

    QString labelName = mark->name();
    QRect  labelRect  = mark->labelRect();
    QFont  labelFont  = style->labelStyle().font();
    QColor labelColor = style->labelStyle().color();

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

bool PlacemarkPainter::testXBug()
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

#include "PlacemarkPainter.moc"

