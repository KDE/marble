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
    : AbstractDataPluginItem( parent ), m_magnitude( 0.0 ), m_depth( 0.0 )
{
    // The size of an item without a text is 0
    setSize( QSize( 0, 0 ) );
    s_font.setBold( true );
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
    setSize( QSize( m_magnitude * 10, m_magnitude * 10 ) );
    updateTooltip();
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
    QRect arcRect( 0, 0, width, height );
    QColor color = oxygenBrickRed4;
    if ( magnitude() < 5.0 ) {
        color = oxygenSunYellow6;
    } else if ( magnitude() < 6.0 ) {
        color = oxygenHotOrange4;
    }
    painter->setPen( QPen( Qt::NoPen ) );
    QBrush brush( color );
    brush.setColor( color );
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
    painter->drawPixmap( 0, 0, m_seismograph );

    // Draws magnitude of the earthquake
    QFontMetrics metrics( s_font );
    QString magnitudeText = QString::number( magnitude() );
    QRect magnitudeRect = metrics.boundingRect( magnitudeText );
    painter->setBrush( QBrush() );
    painter->setPen( QPen() );
    painter->setFont( s_font );
    painter->drawText( QPoint( (arcRect.width() - magnitudeRect.width()) / 2, (arcRect.height() - magnitudeRect.height()) / 2 + metrics.ascent() ), magnitudeText );

    // Restore the old painter state.
    painter->restore();
}

void EarthquakeItem::setDateTime( const QDateTime &dateTime )
{
    m_dateTime = dateTime;
    updateTooltip();
}

QDateTime EarthquakeItem::dateTime() const
{
    return m_dateTime;
}

double EarthquakeItem::depth() const
{
    return m_depth;
}

void EarthquakeItem::setDepth( double depth )
{
    m_depth = depth;
    updateTooltip();
}

void EarthquakeItem::updateTooltip()
{
    QString html = "<table cellpadding=\"2\">";
    if ( m_dateTime.isValid() ) {
        html += "<tr><td align=\"right\">Date</td>";
        html += "<td>" + m_dateTime.toString( Qt::SystemLocaleShortDate ) + "</td></tr>";
    }
    html += "<tr><td align=\"right\">Magnitude</td><td>" + QString::number( m_magnitude ) + "</td></tr>";
    html += "<tr><td align=\"right\">Depth</td><td>" + QString::number( m_depth ) + " km</td></tr>";
    html += "</table>";
    setToolTip( html );
}

}

#include "EarthquakeItem.moc"
