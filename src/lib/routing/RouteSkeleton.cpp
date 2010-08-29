//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RouteSkeleton.h"

#include "GeoDataLineString.h"
#include "MarbleDirs.h"

#include <QtCore/QMap>
#include <QtGui/QPainter>

namespace Marble
{

class RouteSkeletonPrivate
{
public:
    QVector<GeoDataCoordinates> m_route;

    QMap<int, QPixmap> m_pixmapCache;

    RouteSkeleton::RoutePreference m_routePreference;

    RouteSkeleton::AvoidFeatures m_avoidFeatures;

    int m_fontSize;

    /** Determines a suitable index for inserting a via point */
    int viaIndex( const GeoDataCoordinates &position ) const;

    RouteSkeletonPrivate();
};

RouteSkeletonPrivate::RouteSkeletonPrivate() :
        m_routePreference( RouteSkeleton::CarFastest ),
        m_avoidFeatures( RouteSkeleton::AvoidNone ),
        m_fontSize( 0 )
{
    // nothing to do
}

int RouteSkeletonPrivate::viaIndex( const GeoDataCoordinates &position ) const
{
    /** @todo: Works, but does not look elegant at all */

    // Iterates over all ordered trip point pairs (P,Q) and finds the triple
    // (P,position,Q) or (P,Q,position) with minimum length
    qreal minLength = -1.0;
    int result = 0;
    GeoDataLineString viaFirst;
    GeoDataLineString viaSecond;
    for ( int i = 0; i < m_route.size(); ++i ) {
        Q_ASSERT( viaFirst.size() < 4 && viaSecond.size() < 4 );
        if ( viaFirst.size() == 3 ) {
            viaFirst.remove( 0 );
            viaFirst.remove( 0 );
        }

        if ( viaSecond.size() == 3 ) {
            viaSecond.remove( 0 );
            viaSecond.remove( 0 );
        }

        if ( viaFirst.size() == 1 ) {
            viaFirst.append( position );
        }

        viaFirst.append( m_route[i] );
        viaSecond.append( m_route[i] );

        if ( viaSecond.size() == 2 ) {
            viaSecond.append( position );
        }

        if ( viaFirst.size() == 3 ) {
            qreal len = viaFirst.length( EARTH_RADIUS );
            if ( minLength < 0.0 || len < minLength ) {
                minLength = len;
                result = i;
            }
        }

        /** @todo: Assumes that destination is the last point */
        if ( viaSecond.size() == 3 && i + 1 < m_route.size() ) {
            qreal len = viaSecond.length( EARTH_RADIUS );
            if ( minLength < 0.0 || len < minLength ) {
                minLength = len;
                result = i + 1;
            }
        }
    }

    Q_ASSERT( 0 <= result && result <= m_route.size() );
    return result;
}

RouteSkeleton::RouteSkeleton( QObject *parent ) :
        QObject( parent ), d( new RouteSkeletonPrivate )
{
    // nothing to do
}

RouteSkeleton::~RouteSkeleton()
{
    delete d;
}

int RouteSkeleton::size() const
{
    return d->m_route.size();
}

GeoDataCoordinates RouteSkeleton::source() const
{
    GeoDataCoordinates result;
    if ( d->m_route.size() ) {
        result = d->m_route.first();
    }
    return result;
}

GeoDataCoordinates RouteSkeleton::destination() const
{
    GeoDataCoordinates result;
    if ( d->m_route.size() ) {
        result = d->m_route.last();
    }
    return result;
}

GeoDataCoordinates RouteSkeleton::at( int position ) const
{
    return d->m_route.at( position );
}

QPixmap RouteSkeleton::pixmap( int position ) const
{
    if ( d->m_pixmapCache.contains( position ) ) {
        return d->m_pixmapCache[position];
    }

    // Transparent background
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const iconSize = smallScreen ? 32 : 16;
    QImage result( iconSize, iconSize, QImage::Format_ARGB32_Premultiplied );
    result.fill( qRgba( 0, 0, 0, 0 ) );

    // Paint a green circle
    QPainter painter( &result );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor( Qt::black ) );
    painter.setBrush( QBrush( oxygenForestGreen4 ) );
    painter.drawEllipse( 1, 1, iconSize-2, iconSize-2 );
    painter.setBrush( QColor( Qt::black ) );

    char text = char( 'A' + position );

    // Choose a suitable font size once (same for all pixmaps)
    if ( d->m_fontSize == 0 ) {
        QFont font = painter.font();
        d->m_fontSize = 20;
        while ( d->m_fontSize-- > 0 ) {
            font.setPointSize( d->m_fontSize );
            QFontMetrics fontMetric( font );
            if ( fontMetric.width( text ) <= iconSize-3 && fontMetric.height( ) <= iconSize-3 ) {
                break;
            }
        }
    }

    Q_ASSERT( d->m_fontSize );
    QFont font = painter.font();
    font.setPointSize( d->m_fontSize );
    painter.setFont( font );

    // Paint a character denoting the position (0=A, 1=B, 2=C, ...)
    painter.drawText( 2, 2, iconSize-3, iconSize-3, Qt::AlignCenter, QString( text ) );

    d->m_pixmapCache.insert( position, QPixmap::fromImage( result ) );
    return pixmap( position );
}

void RouteSkeleton::clear()
{
    d->m_route.clear();
}

void RouteSkeleton::insert( int index, const GeoDataCoordinates &coordinates )
{
    d->m_route.insert( index, coordinates );
    emit positionAdded( index );
}

void RouteSkeleton::append( const GeoDataCoordinates &coordinates )
{
    d->m_route.append( coordinates );
}

void RouteSkeleton::remove( int index )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        d->m_route.remove( index );
        emit positionRemoved( index );
    }
}

void RouteSkeleton::addVia( const GeoDataCoordinates &position )
{
    int index = d->viaIndex( position );
    d->m_route.insert( index, position );
    emit positionAdded( index );
}

void RouteSkeleton::setPosition( int index, const GeoDataCoordinates &position )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        d->m_route[index] = position;
        emit positionChanged( index, position );
    }
}

void RouteSkeleton::setAvoidFeatures( RouteSkeleton::AvoidFeatures features )
{
    d->m_avoidFeatures = features;
}

RouteSkeleton::AvoidFeatures RouteSkeleton::avoidFeatures() const
{
    return d->m_avoidFeatures;
}

void RouteSkeleton::setRoutePreference( RouteSkeleton::RoutePreference preference )
{
    d->m_routePreference = preference;
}

RouteSkeleton::RoutePreference RouteSkeleton::routePreference() const
{
    return d->m_routePreference;
}

} // namespace Marble

#include "RouteSkeleton.moc"
