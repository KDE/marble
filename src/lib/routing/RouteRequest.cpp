//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RouteRequest.h"

#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "MarbleDirs.h"

#include <QtCore/QMap>
#include <QtGui/QPainter>

namespace Marble
{

struct PixmapElement
{
    int index;

    int size;

    explicit PixmapElement( int index=-1, int size=0 );

    bool operator < ( const PixmapElement &other ) const;
};

class RouteRequestPrivate
{
public:
    QVector<GeoDataPlacemark> m_route;

    QMap<PixmapElement, QPixmap> m_pixmapCache;

    RoutingProfile m_routingProfile;

    /** Determines a suitable index for inserting a via point */
    int viaIndex( const GeoDataCoordinates &position ) const;
};

PixmapElement::PixmapElement( int index_, int size_ ) :
    index( index_ ), size( size_ )
{
    // nothing to do
}

bool PixmapElement::operator <(const PixmapElement &other) const
{
    return index < other.index || size < other.size;
}

int RouteRequestPrivate::viaIndex( const GeoDataCoordinates &position ) const
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

        viaFirst.append( m_route[i].coordinate() );
        viaSecond.append( m_route[i].coordinate() );

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

RouteRequest::RouteRequest( QObject *parent ) :
        QObject( parent ), d( new RouteRequestPrivate )
{
    // nothing to do
}

RouteRequest::~RouteRequest()
{
    delete d;
}

int RouteRequest::size() const
{
    return d->m_route.size();
}

GeoDataCoordinates RouteRequest::source() const
{
    GeoDataCoordinates result;
    if ( d->m_route.size() ) {
        result = d->m_route.first().coordinate();
    }
    return result;
}

GeoDataCoordinates RouteRequest::destination() const
{
    GeoDataCoordinates result;
    if ( d->m_route.size() ) {
        result = d->m_route.last().coordinate();
    }
    return result;
}

GeoDataCoordinates RouteRequest::at( int position ) const
{
    return d->m_route.at( position ).coordinate();
}

QPixmap RouteRequest::pixmap(int position, int size, int margin ) const
{
    PixmapElement const element( position, size );
    if ( d->m_pixmapCache.contains( element ) ) {
        return d->m_pixmapCache[element];
    }

    // Transparent background
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const imageSize = size > 0 ? size : ( smallScreen ? 32 : 16 );
    QImage result( imageSize, imageSize, QImage::Format_ARGB32_Premultiplied );
    result.fill( qRgba( 0, 0, 0, 0 ) );

    // Paint a colored circle
    QPainter painter( &result );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor( Qt::black ) );
    bool const isVisited = visited( position );
    QColor const backgroundColor = isVisited ? Oxygen::aluminumGray4 : Oxygen::forestGreen4;
    painter.setBrush( QBrush( backgroundColor ) );
    painter.setPen( Qt::black );
    int const iconSize = imageSize - 2 * margin;
    painter.drawEllipse( margin, margin, iconSize, iconSize );

    char text = char( 'A' + position );

    // Choose a suitable font size
    QFont font = painter.font();
    int fontSize = 20;
    while ( fontSize-- > 0 ) {
        font.setPointSize( fontSize );
        QFontMetrics fontMetric( font );
        if ( fontMetric.width( text ) <= iconSize && fontMetric.height( ) <= iconSize ) {
            break;
        }
    }

    Q_ASSERT( fontSize );
    font.setPointSize( fontSize );
    painter.setFont( font );

    // Paint a character denoting the position (0=A, 1=B, 2=C, ...)
    painter.drawText( 0, 0, imageSize, imageSize, Qt::AlignCenter, QString( text ) );

    d->m_pixmapCache.insert( element, QPixmap::fromImage( result ) );
    return pixmap( position, size );
}

void RouteRequest::clear()
{
    for ( int i=d->m_route.size()-1; i>=0; --i ) {
        remove( i );
    }
}

void RouteRequest::insert( int index, const GeoDataCoordinates &coordinates, const QString &name )
{
    GeoDataPlacemark placemark;
    placemark.setCoordinate( GeoDataPoint( coordinates ) );
    d->m_route.insert( index, placemark );
    setName( index, name );
    emit positionAdded( index );
}

void RouteRequest::append( const GeoDataCoordinates &coordinates, const QString &name )
{
    GeoDataPlacemark placemark;
    placemark.setCoordinate( GeoDataPoint( coordinates ) );
    d->m_route.append( placemark );
    setName( d->m_route.size()-1, name );
    emit positionAdded( d->m_route.size()-1 );
}

void RouteRequest::remove( int index )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        d->m_route.remove( index );
        emit positionRemoved( index );
    }
}

void RouteRequest::addVia( const GeoDataCoordinates &position )
{
    int index = d->viaIndex( position );
    GeoDataPlacemark placemark;
    placemark.setCoordinate( GeoDataPoint( position ) );
    d->m_route.insert( index, placemark );
    emit positionAdded( index );
}

void RouteRequest::setPosition( int index, const GeoDataCoordinates &position, const QString &name )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        GeoDataPlacemark placemark;
        placemark.setCoordinate( GeoDataPoint( position ) );
        d->m_route[index] = placemark;
        setName( index, name );
        setVisited( index, false );
        emit positionChanged( index, position );
    }
}

void RouteRequest::setName( int index, const QString &name )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        d->m_route[index].setName( name );
    }
}

QString RouteRequest::name( int index ) const
{
    QString result;
    if ( index >= 0 && index < d->m_route.size() ) {
        result = d->m_route[index].name();
    }
    return result;
}

void RouteRequest::setVisited( int index, bool visited )
{
    if ( index >= 0 && index < d->m_route.size() ) {
        d->m_route[index].extendedData().addValue( GeoDataData( "routingVisited", visited ) );
        QMap<PixmapElement, QPixmap>::iterator iter = d->m_pixmapCache.begin();
        while ( iter != d->m_pixmapCache.end() ) {
             if ( iter.key().index == index ) {
                 iter = d->m_pixmapCache.erase( iter );
             } else {
                 ++iter;
             }
         }
        emit positionChanged( index, d->m_route[index].coordinate() );
    }
}

bool RouteRequest::visited( int index ) const
{
    bool visited = false;
    if ( index >= 0 && index < d->m_route.size() ) {
        if ( d->m_route[index].extendedData().contains( "routingVisited" ) ) {
            visited = d->m_route[index].extendedData().value( "routingVisited" ).value().toBool();
        }
    }
    return visited;
}

void RouteRequest::reverse()
{
    int const total = d->m_route.size();
    int const upper = total / 2;
    for( int i=0; i<upper; ++i ) {
        qSwap( d->m_route[i], d->m_route[total-i-1] );
        setVisited( i, false );
        setVisited( total-i-1, false );
    }
}

void RouteRequest::setRoutingProfile( const RoutingProfile &profile )
{
    d->m_routingProfile = profile;
    emit routingProfileChanged();
}

RoutingProfile RouteRequest::routingProfile() const
{
    return d->m_routingProfile;
}

} // namespace Marble

#include "RouteRequest.moc"
