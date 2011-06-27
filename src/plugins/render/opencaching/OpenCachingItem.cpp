//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

#include "MarbleDebug.h"

namespace Marble
{

// That's the font we will use to paint.
QFont OpenCachingItem::s_font = QFont( "Sans Serif", 8 );

OpenCachingItem::OpenCachingItem( const OpenCachingCache& cache, QObject *parent )
    : AbstractDataPluginItem( parent ), m_cache( cache )
{
    // The size of an item without a text is 0
    setSize( QSize( 0, 0 ) );
    s_font.setBold( true );
}

OpenCachingItem::~OpenCachingItem()
{
    // nothing to do
}

QString OpenCachingItem::itemType() const
{
    // Our itemType:
    return "OpenCachingItem";
}

bool OpenCachingItem::initialized()
{
    return m_cache.id() != -1;
}

bool OpenCachingItem::operator<( const AbstractDataPluginItem *other ) const
{
    // FIXME shorter distance to current position?
    const OpenCachingItem* item = dynamic_cast<const OpenCachingItem*>( other );
    return item ? m_cache.id() < item->m_cache.id() : false;
}

void OpenCachingItem::paint( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    // Save the old painter state.
    painter->save();
    painter->autoMapQuality();

    // Draw the arch into the given rect.
    qreal width = m_cache.difficulty() * 10;
    qreal height = m_cache.difficulty() * 10;

    // Draws the circle with circles' center as rectangle's top-left corner.
    QRect arcRect( 0, 0, width, height );
    QColor color = oxygenBrickRed4;
    if ( m_cache.difficulty() < 2.0 ) {
        color = oxygenSunYellow6;
    } else if ( m_cache.difficulty() < 4.0 ) {
        color = oxygenHotOrange4;
    }
    painter->setPen( QPen( Qt::NoPen ) );
    QBrush brush( color );
    brush.setColor( color );
    painter->setBrush( brush );
    painter->drawEllipse( arcRect );

    // Draws difficulty of the cache
    QFontMetrics metrics( s_font );
    QString difficultyText = QString::number( m_cache.difficulty() );
    QRect difficultyRect = metrics.boundingRect( difficultyText );
    painter->setBrush( QBrush() );
    painter->setPen( QPen() );
    painter->setFont( s_font );
    painter->drawText( QPoint( (arcRect.width() - difficultyRect.width()) / 2, (arcRect.height() - difficultyRect.height()) / 2 + metrics.ascent() ), difficultyText );

    // Restore the old painter state.
    painter->restore();
}

void OpenCachingItem::updateTooltip()
{
    QString html = "<table cellpadding=\"2\">";
    if ( m_cache.id() != -1 ) {
        html += "<tr><td align=\"right\">Date hidden</td>";
        html += "<td>" + m_cache.dateHidden().toString( Qt::SystemLocaleShortDate ) + "</td></tr>";
        html += "<tr><td align=\"right\">User name</td><td>" + m_cache.userName() + "</td></tr>";
        if ( !m_cache.cacheName().isEmpty() ) {
            html += "<tr><td align=\"right\">Cache name</td><td>" + m_cache.cacheName() + "</td></tr>";
        }
        html += "<tr><td align=\"right\">Difficulty</td><td>" + QString::number( m_cache.difficulty() ) + "</td></tr>";
        html += "<tr><td align=\"right\">Size</td><td>" + m_cache.sizeString() + "</td></tr>";
        html += "</table>";
        setToolTip( html );
    }
}

}

#include "OpenCachingItem.moc"
