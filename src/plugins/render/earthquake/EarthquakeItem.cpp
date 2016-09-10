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
#include "ViewportParams.h"

#include <QFontMetrics>
#include <QPainter>
#include <QSvgRenderer>
#include <QLocale>

namespace Marble
{

// That's the font we will use to paint.
const QFont EarthquakeItem::s_font = QFont( QStringLiteral( "Sans Serif" ), 8, QFont::Bold );

EarthquakeItem::EarthquakeItem( QObject *parent )
    : AbstractDataPluginItem( parent ), m_magnitude( 0.0 ), m_depth( 0.0 )
{
    // The size of an item without a text is 0
    setSize( QSize( 0, 0 ) );
    setCacheMode( ItemCoordinateCache );
}

EarthquakeItem::~EarthquakeItem()
{
    // nothing to do
}

bool EarthquakeItem::initialized() const
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

void EarthquakeItem::paint( QPainter *painter )
{
    // Save the old painter state.
    painter->save();

    // Draw the arch into the given rect.
    qreal width = magnitude() * 10;
    qreal height = magnitude() * 10;

    // Draws the circle with circles' center as rectangle's top-left corner.
    QRect arcRect( 0, 0, width, height );
    QColor color = Oxygen::brickRed4;
    if ( magnitude() < 5.0 ) {
        color = Oxygen::sunYellow6;
    } else if ( magnitude() < 6.0 ) {
        color = Oxygen::hotOrange4;
    }
    painter->setPen( QPen( Qt::NoPen ) );
    QBrush brush( color );
    brush.setColor( color );
    painter->setBrush( brush );
    painter->drawEllipse( arcRect );

    // Draws the seismograph
    QSvgRenderer renderer(QStringLiteral(":/seismograph.svg"));
    renderer.render( painter, QRectF( 0.0, 0.0, width, height ) );

    // Draws magnitude of the earthquake
    QFontMetrics metrics( s_font );
    const QString magnitudeText = QLocale::system().toString(m_magnitude);
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
    QLocale locale = QLocale::system();
    QString html = QLatin1String("<table cellpadding=\"2\">");
    if ( m_dateTime.isValid() ) {
        html += QLatin1String("<tr><td align=\"right\">") + tr("Date:") + QLatin1String("</td><td>") + locale.toString(m_dateTime, QLocale::ShortFormat) + QLatin1String("</td></tr>");
    }
    html +=
        QLatin1String("<tr><td align=\"right\">") + tr("Magnitude:") + QLatin1String("</td><td>") + locale.toString(m_magnitude) + QLatin1String("</td></tr><tr><td align=\"right\">") + tr("Depth:") + QLatin1String("</td><td>") + locale.toString(m_depth) + QLatin1String(" km</td></tr></table>");
    setToolTip( html );
}

}

#include "moc_EarthquakeItem.cpp"
