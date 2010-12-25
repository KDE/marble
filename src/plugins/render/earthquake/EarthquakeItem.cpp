//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#include "EarthquakeItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

namespace Marble
{

// That's the font we will use to paint.
QFont EarthquakeItem::s_font = QFont( "Sans Serif", 8 );

EarthquakeItem::EarthquakeItem( QObject *parent )
    : AbstractDataPluginItem( parent ), m_magnitude( 0.0 )
{
    // The size of an item without a text is 0
    setSize( QSize( 0, 0 ) );
}

EarthquakeItem::~EarthquakeItem()
{
    // nothing to do
}

QString EarthquakeItem::itemType() const
{
    // Our itemType:
    return "earthquakeItem";
}

bool EarthquakeItem::initialized()
{
    return m_magnitude > 0.0;
}

bool EarthquakeItem::operator<( const AbstractDataPluginItem *other ) const
{
    // Larger magnitude first
    const EarthquakeItem* item = dynamic_cast<const EarthquakeItem*>( other );
    return item ? magnitude() > item->magnitude() : false;
}

double EarthquakeItem::magnitude() const
{
    return m_magnitude;
}

void EarthquakeItem::setMagnitude( double magnitude )
{
    m_magnitude = magnitude;
}

void EarthquakeItem::paint( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    // Save the old painter state.
    painter->save();
    painter->autoMapQuality();

    // Draw the arch into the given rect.
    qreal width = magnitude() * 10;
    qreal height = magnitude() * 10;

    // Draws the circle with circles' center as rectangle's top-left corner.
    QRect arcRect( width / -2, height / -2, width, height );
    QRadialGradient gradient( 0, 0, width / 2 );
    QColor outerRed = oxygenBrickRed4;
    outerRed.setAlpha( 200 );
    gradient.setColorAt( 0.0, outerRed );
    QColor innerRed = oxygenBrickRed4;
    innerRed.setAlpha( 100 );
    gradient.setColorAt( 1.0, innerRed );
    painter->setPen( QPen( Qt::NoPen ) );
    QBrush brush( gradient );
    brush.setColor( oxygenBrickRed4 );
    painter->setBrush( brush );
    painter->drawEllipse( arcRect );

    // Draws the seismograph
    if ( m_seismograph.isNull() ) {
        m_seismograph = QPixmap( width, height );
        QSvgRenderer renderer( QString( ":/seismograph.svg" ) );
        m_seismograph.fill( Qt::transparent );
        QPainter pixmapPainter( &m_seismograph );
        renderer.render( &pixmapPainter, QRectF( 0.0, 0.0, width, height ) );
    }
    painter->drawPixmap( -width / 2, -height / 2, m_seismograph );

    // Draws magnitude of the earthquake
    QFontMetrics metrics( s_font );
    QString magnitudeText = QString::number( magnitude() );
    QRect magnitudeRect = metrics.boundingRect( magnitudeText );
    painter->setBrush( QBrush() );
    painter->setPen( QPen() );
    painter->drawText( QPoint( magnitudeRect.width() / -2, magnitudeRect.height() / 2 ), magnitudeText );

    // Restore the old painter state.
    painter->restore();
}

}

#include "EarthquakeItem.moc"
