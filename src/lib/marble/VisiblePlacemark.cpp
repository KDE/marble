//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "VisiblePlacemark.h"

#include "MarbleDebug.h"
#include "RemoteIconLoader.h"

#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "PlacemarkLayer.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>

using namespace Marble;

VisiblePlacemark::VisiblePlacemark( const GeoDataPlacemark *placemark, const GeoDataCoordinates &coordinates, const GeoDataStyle::ConstPtr &style )
    : m_placemark( placemark ),
      m_selected( false ),
      m_labelDirty(true),
      m_style(style),
      m_coordinates(coordinates)
{
    const RemoteIconLoader *remoteLoader = style->iconStyle().remoteIconLoader();
    QObject::connect( remoteLoader, SIGNAL(iconReady()),
                     this, SLOT(setSymbolPixmap()) );

    setSymbolPixmap();
}

const GeoDataPlacemark* VisiblePlacemark::placemark() const
{
    return m_placemark;
}

const QPixmap& VisiblePlacemark::symbolPixmap() const
{
    if (!m_symbolId.isEmpty() && m_symbolPixmap.isNull()) {
        if ( !m_symbolCache.find( m_symbolId, &m_symbolPixmap ) ) {
            m_symbolPixmap = QPixmap::fromImage(m_style->iconStyle().scaledIcon());
            m_symbolCache.insert( m_symbolId, m_symbolPixmap);
        }
    }
    return m_symbolPixmap;
}

const QString& VisiblePlacemark::symbolId() const
{
    return m_symbolId;
}

bool VisiblePlacemark::selected() const
{
    return m_selected;
}

void VisiblePlacemark::setSelected( bool selected )
{
    if (selected != m_selected) {
        m_selected = selected;
        m_labelDirty = true;
    }
}

const QPointF& VisiblePlacemark::symbolPosition() const
{
    return m_symbolPosition;
}

const QPointF VisiblePlacemark::hotSpot() const
{
    const QSize iconSize = m_style->iconStyle().scaledIcon().size();

    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;
    QPointF pixelHotSpot = m_style->iconStyle().hotSpot( xunits, yunits );

    switch ( xunits ) {
    case GeoDataHotSpot::Fraction:
        pixelHotSpot.setX( iconSize.width() * pixelHotSpot.x() );
        break;
    case GeoDataHotSpot::Pixels:
        /* nothing to do */
        break;
    case GeoDataHotSpot::InsetPixels:
        pixelHotSpot.setX( iconSize.width() - pixelHotSpot.x() );
        break;
    }

    switch ( yunits ) {
    case GeoDataHotSpot::Fraction:
        pixelHotSpot.setY( iconSize.height() * ( 1.0 - pixelHotSpot.y() ) );
        break;
    case GeoDataHotSpot::Pixels:
        /* nothing to do */
        break;
    case GeoDataHotSpot::InsetPixels:
        pixelHotSpot.setY( iconSize.height() - pixelHotSpot.y() );
        break;
    }

    return pixelHotSpot;
}

void VisiblePlacemark::setSymbolPosition( const QPointF& position )
{
    m_symbolPosition = position;
}

const QPixmap& VisiblePlacemark::labelPixmap()
{
    if (m_labelDirty) {
        drawLabelPixmap();
    }

    return m_labelPixmap;
}

void VisiblePlacemark::setSymbolPixmap()
{
    if (m_style) {
        m_symbolId = m_style->iconStyle().iconPath() + QString::number(m_style->iconStyle().scale());
        if (m_style->iconStyle().iconPath().isEmpty()) {
            m_symbolId.clear();
            m_symbolPixmap = QPixmap::fromImage(m_style->iconStyle().scaledIcon());
        }
        emit updateNeeded();
    }
    else {
        mDebug() << "Style pointer is Null";
    }
}

const QRectF& VisiblePlacemark::labelRect() const
{
    return m_labelRect;
}

void VisiblePlacemark::setLabelRect( const QRectF& labelRect )
{
    m_labelRect = labelRect;
}

void VisiblePlacemark::setStyle(const GeoDataStyle::ConstPtr &style)
{
    m_style = style;
    m_labelDirty = true;
    setSymbolPixmap();
}

GeoDataStyle::ConstPtr VisiblePlacemark::style() const
{
    return m_style;
}

QRectF VisiblePlacemark::symbolRect() const
{
    return QRectF(m_symbolPosition, m_symbolPixmap.size());
}

QRectF VisiblePlacemark::boundingBox() const
{
    return m_labelRect.isEmpty() ? symbolRect() : m_labelRect.united(symbolRect());
}

const GeoDataCoordinates &VisiblePlacemark::coordinates() const
{
    return m_coordinates;
}

void VisiblePlacemark::drawLabelPixmap()
{
    m_labelDirty = false;
    QString labelName = m_placemark->displayName();
    if ( labelName.isEmpty() || m_style->labelStyle().color() == QColor(Qt::transparent) ) {
        m_labelPixmap = QPixmap();
        return;
    }

    QFont  labelFont  = m_style->labelStyle().scaledFont();
    QColor labelColor = m_style->labelStyle().color();

    LabelStyle labelStyle = Normal;
    if ( m_selected ) {
        labelStyle = Selected;
    } else if ( m_style->labelStyle().glow() ) {
        labelStyle = Glow;
    }

    int textHeight = QFontMetrics( labelFont ).height();

    int textWidth;
    if ( m_style->labelStyle().glow() ) {
        labelFont.setWeight( 75 ); // Needed to calculate the correct pixmap size;
        textWidth = ( QFontMetrics( labelFont ).width( labelName )
            + qRound( 2 * s_labelOutlineWidth ) );
    } else {
        textWidth = ( QFontMetrics( labelFont ).width( labelName ) );
    }


    // Due to some XOrg bug this requires a workaround via
    // QImage in some cases (at least with Qt 4.2).
    if ( !PlacemarkLayer::m_useXWorkaround ) {
        m_labelPixmap = QPixmap( QSize( textWidth, textHeight ) );
        m_labelPixmap.fill( Qt::transparent );

        QPainter labelPainter( &m_labelPixmap );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );
    } else {
        QImage image( QSize( textWidth, textHeight ),
                      QImage::Format_ARGB32_Premultiplied );
        image.fill( 0 );

        QPainter labelPainter( &image );

        drawLabelText( labelPainter, labelName, labelFont, labelStyle, labelColor );

        labelPainter.end();

        m_labelPixmap = QPixmap::fromImage( image );
    }
}

void VisiblePlacemark::drawLabelText(QPainter &labelPainter, const QString &text,
                                            const QFont &labelFont, LabelStyle labelStyle, const QColor &color )
{
    QFont font = labelFont;
    QFontMetrics metrics = QFontMetrics( font );
    int fontAscent = metrics.ascent();

    switch ( labelStyle ) {
    case Selected: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        QRect textRect( 0, 0, metrics.width( text ), metrics.height() );
        labelPainter.fillRect( textRect, QApplication::palette().highlight() );
        labelPainter.setPen( QPen( QApplication::palette().highlightedText(), 1 ) );
        labelPainter.drawText( 0, fontAscent, text );
        break;
    }
    case Glow: {
        font.setWeight( 75 );
        fontAscent = QFontMetrics( font ).ascent();

        QPen outlinepen( color == QColor( Qt::white ) ? Qt::black : Qt::white );
        outlinepen.setWidthF( s_labelOutlineWidth );
        QBrush  outlinebrush( color );

        QPainterPath outlinepath;

        const QPointF  baseline( s_labelOutlineWidth / 2.0, fontAscent );
        outlinepath.addText( baseline, font, text );
        labelPainter.setRenderHint( QPainter::Antialiasing, true );
        labelPainter.setPen( outlinepen );
        labelPainter.setBrush( outlinebrush );
        labelPainter.drawPath( outlinepath );
        labelPainter.setPen( Qt::NoPen );
        labelPainter.drawPath( outlinepath );
        labelPainter.setRenderHint( QPainter::Antialiasing, false );
        break;
    }
    default: {
        labelPainter.setPen( color );
        labelPainter.setFont( font );
        labelPainter.drawText( 0, fontAscent, text );
    }
    }
}

#include "moc_VisiblePlacemark.cpp"
