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

#include <QPixmap>

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "GeoDataLineString.h"
#include "GeoPainter.h"
#include "GeoAprsCoordinates.h"

using namespace Marble;

AprsObject::AprsObject( const GeoAprsCoordinates &at, const QString &name )
    : m_myName( name ),
      m_seenFrom( at.seenFrom() ),
      m_havePixmap ( false ),
      m_pixmapFilename( ),
      m_pixmap( 0 )
{
    m_history.push_back( at );
}

AprsObject::~AprsObject()
{
    delete m_pixmap;
}

GeoAprsCoordinates
AprsObject::location() const
{
    return m_history.last();
}

void
AprsObject::setLocation( const GeoAprsCoordinates &location )
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

    m_seenFrom = ( m_seenFrom | location.seenFrom() );
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

QColor
AprsObject::calculatePaintColor( int from, const QTime &time, int fadeTime )
{
    QColor color;
    if ( from & GeoAprsCoordinates::Directly ) {
        color = Oxygen::emeraldGreen4; // oxygen green if direct
    } else if ( (from & ( GeoAprsCoordinates::FromTCPIP | GeoAprsCoordinates::FromTTY ) ) == ( GeoAprsCoordinates::FromTCPIP | GeoAprsCoordinates::FromTTY ) ) {
        color = Oxygen::burgundyPurple4; // oxygen purple if both
    } else if  ( from & GeoAprsCoordinates::FromTCPIP ) {
        color = Oxygen::brickRed4; // oxygen red if net
    } else if  ( from & GeoAprsCoordinates::FromTTY ) {
        color = Oxygen::seaBlue4; // oxygen blue if TNC TTY relay
    } else if ( from & ( GeoAprsCoordinates::FromFile ) ) {
        color = Oxygen::sunYellow3; // oxygen yellow if file only
    } else {
        mDebug() << "**************************************** unknown from: "
                 << from;
        color = Oxygen::aluminumGray5;  // shouldn't happen but a user
                                        // could mess up I suppose we
                                        // should at least draw it in
                                        // something.
    }

    if ( fadeTime > 0 && time.elapsed() > fadeTime ) { // 5 min ( 600000 ms )
        color.setAlpha( 160 );
    }

    return color;
}

void
AprsObject::render( GeoPainter *painter, ViewportParams *viewport,
                    int fadeTime, int hideTime )
{
    Q_UNUSED( viewport );

    if ( hideTime > 0 && m_history.last().timestamp().elapsed() > hideTime )
        return;

    QColor baseColor = calculatePaintColor( m_seenFrom,
                                      m_history.last().timestamp(),
                                      fadeTime );

    if ( m_history.count() > 1 ) {
    
        QList<GeoAprsCoordinates>::iterator spot = m_history.begin();
        QList<GeoAprsCoordinates>::iterator endSpot = m_history.end();
        
        GeoDataLineString lineString;
        lineString.setTessellate( true );
        lineString << *spot; // *spot exists because m_history.count() > 1

        for( ++spot; spot != endSpot; ++spot ) {

            if ( hideTime > 0 && ( *spot ).timestamp().elapsed() > hideTime )
                break;

            lineString << *spot;

            // draw the new circle in whatever is appropriate for that point
            const QColor penColor = calculatePaintColor( spot->seenFrom(), spot->timestamp(), fadeTime );
            painter->setPen( penColor );
            painter->drawRect( *spot, 5, 5 );
        }

        // draw the line in the base color
        painter->setPen( baseColor );
        painter->drawPolyline( lineString );
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
