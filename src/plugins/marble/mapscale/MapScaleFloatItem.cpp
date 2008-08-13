//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MapScaleFloatItem.h"

#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtGui/QPixmap>

#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

namespace Marble
{

MapScaleFloatItem::MapScaleFloatItem( const QPointF &point, const QSizeF &size )
    : MarbleAbstractFloatItem( point, size ),
      m_radius(0),
      m_invScale(0.0),
      m_leftBarMargin(0),
      m_rightBarMargin(0),
      m_scaleBarWidth(0),
      m_viewportWidth(0),
      m_scaleBarHeight(5),
      m_scaleBarKm(0.0),
      m_bestDivisor(0),
      m_pixelInterval(0),
      m_valueInterval(0),
      m_unit(tr("km")),
      m_scaleInitDone( false )
{
}

MapScaleFloatItem::~MapScaleFloatItem()
{
}

QStringList MapScaleFloatItem::backendTypes() const
{
    return QStringList( "mapscale" );
}

QString MapScaleFloatItem::name() const
{
    return tr("Scale Bar");
}

QString MapScaleFloatItem::guiString() const
{
    return tr("&Scale Bar");
}

QString MapScaleFloatItem::nameId() const
{
    return QString( "scalebar" );
}

QString MapScaleFloatItem::description() const
{
    return tr("This is a float item that provides a map scale.");}

QIcon MapScaleFloatItem::icon () const
{
    return QIcon();
}


void MapScaleFloatItem::initialize ()
{
}

bool MapScaleFloatItem::isInitialized () const
{
    return true;
}

bool MapScaleFloatItem::needsUpdate( ViewportParams *viewport )
{
    int viewportWidth = viewport->width();

    if ( m_radius == viewport->radius() && viewportWidth == m_viewportWidth && m_scaleInitDone )
    {
        return false;
    }

    int fontHeight     = QFontMetrics( font() ).ascent();
    setSize( QSizeF( viewport->width() / 2, 2 * padding() + fontHeight + 3 + m_scaleBarHeight ) ); 

    m_leftBarMargin  = QFontMetrics( font() ).boundingRect( "0" ).width() / 2;
    m_rightBarMargin = QFontMetrics( font() ).boundingRect( "0000" ).width() / 2;

    m_scaleBarWidth = contentRect().width() - m_leftBarMargin - m_rightBarMargin;
    m_viewportWidth = viewport->width();
    m_radius = viewport->radius();
    m_scaleInitDone = true;

    return true;
}

bool MapScaleFloatItem::renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    painter->save();

    painter->setRenderHint( QPainter::Antialiasing, true );

    int fontHeight     = QFontMetrics( font() ).ascent();

    setSize( QSizeF( viewport->width() / 2, 2 * padding() + fontHeight + 3 + m_scaleBarHeight ) ); 

    m_scaleBarKm    = (double)(m_scaleBarWidth) * EARTH_RADIUS / 
                      (double)(viewport->radius());

    calcScaleBar();

    painter->setPen(   QColor( Qt::darkGray ) );
    painter->setBrush( QColor( Qt::darkGray ) );
    painter->drawRect( m_leftBarMargin, fontHeight + 3,
                       m_scaleBarWidth,
                       m_scaleBarHeight );

    painter->setPen(   QColor( Qt::black ) );
    painter->setBrush( QColor( Qt::white ) );
    painter->drawRect( m_leftBarMargin, fontHeight + 3,
                       m_bestDivisor * m_pixelInterval, m_scaleBarHeight );

    painter->setBrush( QColor( Qt::black ) );

    QString  intervalStr;
    int      lastStringEnds     = 0;
    int      currentStringBegin = 0;
 
    for ( int j = 0; j <= m_bestDivisor; j += 2 ) {
        if ( j < m_bestDivisor )
            painter->drawRect( m_leftBarMargin + j * m_pixelInterval,
                               fontHeight + 3, m_pixelInterval - 1,
                               m_scaleBarHeight );


            if ( m_bestDivisor * m_valueInterval > 10000 ) {
                m_unit = tr("km");
                intervalStr.setNum( j * m_valueInterval / 1000 );
            }
            else {
                m_unit = tr("m");
                intervalStr.setNum( j * m_valueInterval );
            }

        if ( j == 0 ) {
            painter->drawText( 0, fontHeight, "0 " + m_unit );
            lastStringEnds = QFontMetrics( font() ).width( "0 " + m_unit );
            continue;
        }

        currentStringBegin = ( j * m_pixelInterval 
                               - QFontMetrics( font() ).width( intervalStr ) / 2 );
        if ( lastStringEnds < currentStringBegin ) {
            painter->drawText( currentStringBegin, fontHeight, intervalStr );
            lastStringEnds = currentStringBegin + QFontMetrics( font() ).width( intervalStr );
        }
    }

    painter->restore();

    return true;
}

void MapScaleFloatItem::calcScaleBar()
{
    double  magnitude = 1;

    // First we calculate the exact length of the whole area that is possibly 
    // available to the scalebar in kilometers
    int  magValue = (int)( m_scaleBarKm );

    // We calculate the two most significant digits of the km-scalebar-length
    // and store them in magValue.
    while ( magValue >= 100 ) {
        magValue  /= 10;
        magnitude *= 10; 
    }

    m_bestDivisor = 4;
    int  bestMagValue = 1;

    for ( int i = 0; i < magValue; i++ ) {
        // We try to find the lowest divisor between 4 and 8 that
        // divides magValue without remainder. 
        for ( int j = 4; j < 9; j++ ) {
            if ( ( magValue - i ) % j == 0 ) {
                // We store the very first result we find and store
                // m_bestDivisor and bestMagValue as a final result.
                m_bestDivisor = j;
                bestMagValue  = magValue - i;

                // Stop all for loops and end search
                i = magValue; 
                j = 9;
            }
        }

        // If magValue doesn't divide through values between 4 and 8
        // (e.g. because it's a prime number) try again with magValue
        // decreased by i.
    }

    m_pixelInterval = (int)( m_scaleBarWidth * (double)( bestMagValue )
                             / (double)( magValue ) / m_bestDivisor );
    m_valueInterval = (int)( bestMagValue * magnitude / m_bestDivisor );
}

}

Q_EXPORT_PLUGIN2(MapScaleFloatItem, Marble::MapScaleFloatItem)

#include "MapScaleFloatItem.moc"
