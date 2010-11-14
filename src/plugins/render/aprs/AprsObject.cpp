//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <marble@ws6z.com>
//

#include "AprsObject.h"

#include <QtGui/QPixmap>

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoAprsCoordinates.h"

using namespace Marble;

AprsObject::AprsObject( const GeoAprsCoordinates &at, QString &name )
    : m_myName( name ),
      m_seenFrom( GeoAprsCoordinates::FromNowhere ),
      m_havePixmap ( false ),
      m_pixmapFilename( ),
      m_pixmap( 0 )
{
    m_history.push_back( at );
}

AprsObject::AprsObject( const qreal &lon, const qreal &lat,
                        const QString &name, int where )
    : m_myName( name ),
      m_seenFrom( where ),
      m_havePixmap ( false ),
      m_pixmapFilename( ),
      m_pixmap( 0 )
{
    m_history.push_back( GeoAprsCoordinates( lon, lat, 0,
                                             GeoAprsCoordinates::Degree ) );
}

AprsObject::~AprsObject()
{
    if ( m_pixmap )
        delete m_pixmap;
}

GeoAprsCoordinates
AprsObject::location()
{
    return m_history.last();
}

void
AprsObject::setLocation( GeoAprsCoordinates location )
{
    // Not ideal but it's unlikely they'll jump to the *exact* same spot again
    if ( !m_history.contains( location ) ) {
        m_history.push_back( location );
        mDebug() << "  moved: " << m_myName.toLocal8Bit().data();
    } else {
        int index = m_history.indexOf( location );
        QTime now;
        m_history[index].setTimestamp( now );
        m_history[index].addSeenFrom( location.seenFrom() );
    }
}

void
AprsObject::setLocation( qreal lon, qreal lat, int from )
{
    setLocation( GeoAprsCoordinates( lon, lat, 0,
                                     GeoAprsCoordinates::Degree, from ) );
}

void
AprsObject::setPixmapId( QString &pixmap )
{
    QString pixmapFilename = MarbleDirs::path( pixmap );
    if ( QFile( pixmapFilename ).exists() ) {
        m_havePixmap = true;
        m_pixmapFilename = pixmapFilename;
        // We can't load the pixmap here since it's used in a different thread
    }
    else {
        m_havePixmap = false;
    }
}

void
AprsObject::setSeenFrom( int where )
{
    m_seenFrom = ( m_seenFrom | where );
}

QColor
AprsObject::calculatePaintColor( GeoPainter *painter ) const
{
    QTime now;
    return calculatePaintColor( painter, m_seenFrom, now );
}

QColor
AprsObject::calculatePaintColor( GeoPainter *painter, int from, const QTime &time,
                           int fadeTime ) const
{
    QColor color;
    if ( from & GeoAprsCoordinates::Directly ) {
        color = QColor( 0, 201, 0, 255 ); // oxygen green if direct
    } else if ( (from & ( GeoAprsCoordinates::FromTCPIP | GeoAprsCoordinates::FromTTY ) ) == ( GeoAprsCoordinates::FromTCPIP | GeoAprsCoordinates::FromTTY ) ) {
        color = QColor( 176, 0, 141, 255 ); // oxygen purple if both
    } else if  ( from & GeoAprsCoordinates::FromTCPIP ) {
        color = QColor( 255, 0, 0, 255 ); // oxygen red if net
    } else if  ( from & GeoAprsCoordinates::FromTTY ) {
        color = QColor( 0, 0, 201, 255 ); // oxygen blue if TNC TTY relay
    } else if ( from & ( GeoAprsCoordinates::FromFile ) ) {
        color = QColor( 255, 255, 0, 255 ); // oxygen yellow if file only
    } else {
        mDebug() << "**************************************** unknown from: "
                 << from;
        color = QColor( 0, 0, 0, 255 ); // shouldn't happen but a user
                                        // could mess up I suppose we
                                        // should at least draw it in
                                        // something.
    }

    if ( fadeTime > 0 && time.elapsed() > fadeTime ) { // 5 min ( 600000 ms )
        color.setAlpha( 160 );
    }

    painter->setPen( color );
    return color;
}

void
AprsObject::render( GeoPainter *painter, ViewportParams *viewport,
                    const QString& renderPos, GeoSceneLayer * layer,
                    int fadeTime, int hideTime )
{
    Q_UNUSED( viewport );
    Q_UNUSED( layer );
    Q_UNUSED( renderPos );

    if ( hideTime > 0 && m_history.last().timestamp().elapsed() > hideTime )
        return;

    QColor baseColor = calculatePaintColor( painter, m_seenFrom,
                                      m_history.last().timestamp(),
                                      fadeTime );

    if ( m_history.count() > 1 ) {
    
        QList<GeoAprsCoordinates>::iterator spot = m_history.begin();
        QList<GeoAprsCoordinates>::iterator endSpot = m_history.end();
        QList<GeoAprsCoordinates>::iterator lastspot;
        
        for( ++spot, lastspot = m_history.begin();
             spot != endSpot;
             ++spot, ++lastspot ) {

            if ( hideTime > 0 && ( *spot ).timestamp().elapsed() > hideTime )
                break;

            // draw the line in the base color
            painter->setPen( baseColor );
            painter->drawLine( *lastspot, *spot );

            // draw the new circle in whatever is appropriate for that point
            calculatePaintColor( painter, ( *spot ).seenFrom(), ( *spot ).timestamp(),
                           fadeTime );
            painter->drawRect( *spot, 5, 5, false );
        }
    }
    

    // Always draw the symbol then the text last so it's on top
    if ( m_havePixmap ) {
        if ( ! m_pixmap )
            m_pixmap = new QPixmap ( m_pixmapFilename );
        if ( m_pixmap && ! m_pixmap->isNull() )
            painter->drawPixmap( m_history.last(), *m_pixmap ); 
        else
            painter->drawRect( m_history.last(), 6, 6 );
    }
    else
        painter->drawRect( m_history.last(), 6, 6 );

    painter->setPen( baseColor );
    painter->drawText( m_history.last(), m_myName );
}
