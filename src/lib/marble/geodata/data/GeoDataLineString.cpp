//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
// Copyright 2009 Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataLineString.h"
#include "GeoDataLineString_p.h"

#include "GeoDataLinearRing.h"
#include "MarbleMath.h"
#include "Quaternion.h"
#include "MarbleDebug.h"

#include <QDataStream>


namespace Marble
{
GeoDataLineString::GeoDataLineString( TessellationFlags f )
  : GeoDataGeometry( new GeoDataLineStringPrivate( f ) )
{
//    mDebug() << "1) GeoDataLineString created:" << p();
}

GeoDataLineString::GeoDataLineString( GeoDataLineStringPrivate* priv )
  : GeoDataGeometry( priv )
{
//    mDebug() << "2) GeoDataLineString created:" << p();
}

GeoDataLineString::GeoDataLineString( const GeoDataGeometry & other )
  : GeoDataGeometry( other )
{
//    mDebug() << "3) GeoDataLineString created:" << p();
}

GeoDataLineString::~GeoDataLineString()
{
#ifdef DEBUG_GEODATA
    mDebug() << "delete Linestring";
#endif
}

GeoDataLineStringPrivate* GeoDataLineString::p()
{
    return static_cast<GeoDataLineStringPrivate*>(d);
}

const GeoDataLineStringPrivate* GeoDataLineString::p() const
{
    return static_cast<GeoDataLineStringPrivate*>(d);
}

void GeoDataLineStringPrivate::interpolateDateLine( const GeoDataCoordinates & previousCoords,
                                                    const GeoDataCoordinates & currentCoords,
                                                    GeoDataCoordinates & previousAtDateLine,
                                                    GeoDataCoordinates & currentAtDateLine,
                                                    TessellationFlags f ) const
{
    GeoDataCoordinates dateLineCoords;

//    mDebug() << Q_FUNC_INFO;

    if ( f.testFlag( RespectLatitudeCircle ) && previousCoords.latitude() == currentCoords.latitude() ) {
        dateLineCoords = currentCoords;
    }
    else {
        int recursionCounter = 0;
        dateLineCoords = findDateLine( previousCoords, currentCoords, recursionCounter );
    }

    previousAtDateLine = dateLineCoords;
    currentAtDateLine = dateLineCoords;

    if ( previousCoords.longitude() < 0 ) {
        previousAtDateLine.setLongitude( -M_PI );
        currentAtDateLine.setLongitude( +M_PI );
    }
    else {
        previousAtDateLine.setLongitude( +M_PI );
        currentAtDateLine.setLongitude( -M_PI );
    }
}

GeoDataCoordinates GeoDataLineStringPrivate::findDateLine( const GeoDataCoordinates & previousCoords,
                                             const GeoDataCoordinates & currentCoords,
                                             int recursionCounter ) const
{
    int currentSign = ( currentCoords.longitude() < 0.0 ) ? -1 : +1 ;
    int previousSign = ( previousCoords.longitude() < 0.0 ) ? -1 : +1 ;

    qreal longitudeDiff =   fabs( previousSign * M_PI  - previousCoords.longitude() )
                          + fabs( currentSign * M_PI - currentCoords.longitude() );

    if ( longitudeDiff < 0.001 || recursionCounter == 100 ) {
//        mDebug() << "stopped at recursion" << recursionCounter << " and longitude difference " << longitudeDiff;
        return currentCoords;
    }
    ++recursionCounter;

    qreal  lon = 0.0;
    qreal  lat = 0.0;

    qreal altDiff = currentCoords.altitude() - previousCoords.altitude();

    const Quaternion itpos = Quaternion::nlerp( previousCoords.quaternion(), currentCoords.quaternion(), 0.5 );
    itpos.getSpherical( lon, lat );

    qreal altitude = previousCoords.altitude() + 0.5 * altDiff;

    GeoDataCoordinates interpolatedCoords( lon, lat, altitude );

    int interpolatedSign = ( interpolatedCoords.longitude() < 0.0 ) ? -1 : +1 ;

/*
    mDebug() << "SRC" << previousCoords.toString();
    mDebug() << "TAR" << currentCoords.toString();
    mDebug() << "IPC" << interpolatedCoords.toString();
*/

    if ( interpolatedSign != currentSign ) {
        return findDateLine( interpolatedCoords, currentCoords, recursionCounter );
    }

    return findDateLine( previousCoords, interpolatedCoords, recursionCounter );
}

bool GeoDataLineString::isEmpty() const
{
    return p()->m_vector.isEmpty();
}

int GeoDataLineString::size() const
{
    return p()->m_vector.size();
}

GeoDataCoordinates& GeoDataLineString::at( int pos )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    return p()->m_vector[ pos ];
}

const GeoDataCoordinates& GeoDataLineString::at( int pos ) const
{
    return p()->m_vector.at( pos );
}

GeoDataCoordinates& GeoDataLineString::operator[]( int pos )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    return p()->m_vector[ pos ];
}

const GeoDataCoordinates& GeoDataLineString::operator[]( int pos ) const
{
    return p()->m_vector[ pos ];
}

GeoDataCoordinates& GeoDataLineString::last()
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    return p()->m_vector.last();
}

GeoDataCoordinates& GeoDataLineString::first()
{
    GeoDataGeometry::detach();
    return p()->m_vector.first();
}

const GeoDataCoordinates& GeoDataLineString::last() const
{
    return p()->m_vector.last();
}

const GeoDataCoordinates& GeoDataLineString::first() const
{
    return p()->m_vector.first();
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::begin()
{
    GeoDataGeometry::detach();
    return p()->m_vector.begin();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::begin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::end()
{
    GeoDataGeometry::detach();
    return p()->m_vector.end();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::end() const
{
    return p()->m_vector.constEnd();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::constEnd() const
{
    return p()->m_vector.constEnd();
}

void GeoDataLineString::insert( int index, const GeoDataCoordinates& value )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    d->m_vector.insert( index, value );
}

void GeoDataLineString::append ( const GeoDataCoordinates& value )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    d->m_vector.append( value );
}

GeoDataLineString& GeoDataLineString::operator << ( const GeoDataCoordinates& value )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    d->m_vector.append( value );
    return *this;
}

GeoDataLineString& GeoDataLineString::operator << ( const GeoDataLineString& value )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;

    QVector<GeoDataCoordinates>::const_iterator itCoords = value.constBegin();
    QVector<GeoDataCoordinates>::const_iterator itEnd = value.constEnd();

    for( ; itCoords != itEnd; ++itCoords ) {
        d->m_vector.append( *itCoords );
    }

    return *this;
}

bool GeoDataLineString::operator==( const GeoDataLineString &other ) const
{
    if ( !GeoDataGeometry::equals(other) ||
          size() != other.size() ||
          tessellate() != other.tessellate() ) {
        return false;
    }

    const GeoDataLineStringPrivate* d = p();
    const GeoDataLineStringPrivate* other_d = other.p();

    QVector<GeoDataCoordinates>::const_iterator itCoords = d->m_vector.constBegin();
    QVector<GeoDataCoordinates>::const_iterator otherItCoords = other_d->m_vector.constBegin();
    QVector<GeoDataCoordinates>::const_iterator itEnd = d->m_vector.constEnd();
    QVector<GeoDataCoordinates>::const_iterator otherItEnd = other_d->m_vector.constEnd();

    for ( ; itCoords != itEnd && otherItCoords != otherItEnd; ++itCoords, ++otherItCoords ) {
        if ( *itCoords != *otherItCoords ) {
            return false;
        }
    }

    Q_ASSERT ( itCoords == itEnd && otherItCoords == otherItEnd );
    return true;
}

bool GeoDataLineString::operator!=( const GeoDataLineString &other ) const
{
    return !this->operator==(other);
}

void GeoDataLineString::clear()
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;

    d->m_vector.clear();
}

bool GeoDataLineString::isClosed() const
{
    return false;
}

bool GeoDataLineString::tessellate() const
{
    return p()->m_tessellationFlags.testFlag(Tessellate);
}

void GeoDataLineString::setTessellate( bool tessellate )
{
    GeoDataGeometry::detach();
    // According to the KML reference the tesselation of line strings in Google Earth
    // is generally done along great circles. However for subsequent points that share
    // the same latitude the latitude circles are followed. Our Tesselate and RespectLatitude
    // Flags provide this behaviour. For true polygons the latitude circles don't get considered.

    if ( tessellate ) {
        p()->m_tessellationFlags |= Tessellate;
        p()->m_tessellationFlags |= RespectLatitudeCircle;
    } else {
        p()->m_tessellationFlags ^= Tessellate;
        p()->m_tessellationFlags ^= RespectLatitudeCircle;
    }
}

TessellationFlags GeoDataLineString::tessellationFlags() const
{
    return p()->m_tessellationFlags;
}

void GeoDataLineString::setTessellationFlags( TessellationFlags f )
{
    p()->m_tessellationFlags = f;
}

GeoDataLineString GeoDataLineString::toNormalized() const
{
    GeoDataLineString normalizedLineString;

    normalizedLineString.setTessellationFlags( tessellationFlags() );

    qreal lon;
    qreal lat;

    // FIXME: Think about how we can avoid unnecessary copies
    //        if the linestring stays the same.
    QVector<GeoDataCoordinates>::const_iterator end = p()->m_vector.constEnd();
    for( QVector<GeoDataCoordinates>::const_iterator itCoords
          = p()->m_vector.constBegin();
         itCoords != end;
         ++itCoords ) {

        itCoords->geoCoordinates( lon, lat );
        qreal alt = itCoords->altitude();
        GeoDataCoordinates::normalizeLonLat( lon, lat );

        GeoDataCoordinates normalizedCoords( *itCoords );
        normalizedCoords.set( lon, lat, alt );
        normalizedLineString << normalizedCoords;
    }

    return normalizedLineString;
}

GeoDataLineString GeoDataLineString::toRangeCorrected() const
{
    if ( p()->m_dirtyRange ) {

        delete p()->m_rangeCorrected;

        if( isClosed() ) {
            p()->m_rangeCorrected = new GeoDataLinearRing( toPoleCorrected() );
        } else {
            p()->m_rangeCorrected = new GeoDataLineString( toPoleCorrected() );
        }
        p()->m_dirtyRange = false;
    }

    return *p()->m_rangeCorrected;
}

QVector<GeoDataLineString*> GeoDataLineString::toDateLineCorrected() const
{
    QVector<GeoDataLineString*> lineStrings;

    p()->toDateLineCorrected( *this, lineStrings );

    return lineStrings;
}

GeoDataLineString GeoDataLineString::toPoleCorrected() const
{
    if( isClosed() ) {
        GeoDataLinearRing poleCorrected;
        p()->toPoleCorrected( *this, poleCorrected );
        return poleCorrected;
    } else {
        GeoDataLineString poleCorrected;
        p()->toPoleCorrected( *this, poleCorrected );
        return poleCorrected;
    }
}

void GeoDataLineStringPrivate::toPoleCorrected( const GeoDataLineString& q, GeoDataLineString& poleCorrected ) const
{
    poleCorrected.setTessellationFlags( q.tessellationFlags() );

    GeoDataCoordinates previousCoords;
    GeoDataCoordinates currentCoords;

    if ( q.isClosed() ) {
        if ( !( m_vector.first().isPole() ) &&
              ( m_vector.last().isPole() ) ) {
                qreal firstLongitude = ( m_vector.first() ).longitude();
                GeoDataCoordinates modifiedCoords( m_vector.last() );
                modifiedCoords.setLongitude( firstLongitude );
                poleCorrected << modifiedCoords;
        }
    }

    QVector<GeoDataCoordinates>::const_iterator itCoords = m_vector.constBegin();
    QVector<GeoDataCoordinates>::const_iterator itEnd = m_vector.constEnd();

    for( ; itCoords != itEnd; ++itCoords ) {

        currentCoords  = *itCoords;

        if ( itCoords == m_vector.constBegin() ) {
            previousCoords = currentCoords;
        }

        if ( currentCoords.isPole() ) {
            if ( previousCoords.isPole() ) {
                continue;
            }
            else {
                qreal previousLongitude = previousCoords.longitude();
                GeoDataCoordinates currentModifiedCoords( currentCoords );
                currentModifiedCoords.setLongitude( previousLongitude );
                poleCorrected << currentModifiedCoords;
            }
        }
        else {
            if ( previousCoords.isPole() ) {
                qreal currentLongitude = currentCoords.longitude();
                GeoDataCoordinates previousModifiedCoords( previousCoords );
                previousModifiedCoords.setLongitude( currentLongitude );
                poleCorrected << previousModifiedCoords;
                poleCorrected << currentCoords;
            }
            else {
                // No poles at all. Nothing special to handle
                poleCorrected << currentCoords;
            }
        }
        previousCoords = currentCoords;
    }

    if ( q.isClosed() ) {
        if (  ( m_vector.first().isPole() ) &&
             !( m_vector.last().isPole() ) ) {
                qreal lastLongitude = ( m_vector.last() ).longitude();
                GeoDataCoordinates modifiedCoords( m_vector.first() );
                modifiedCoords.setLongitude( lastLongitude );
                poleCorrected << modifiedCoords;
        }
    }
}

void GeoDataLineStringPrivate::toDateLineCorrected(
                           const GeoDataLineString & q,
                           QVector<GeoDataLineString*> & lineStrings
                           ) const
{
    const bool isClosed = q.isClosed();

    const QVector<GeoDataCoordinates>::const_iterator itStartPoint = q.constBegin();
    const QVector<GeoDataCoordinates>::const_iterator itEndPoint = q.constEnd();
    QVector<GeoDataCoordinates>::const_iterator itPoint = itStartPoint;
    QVector<GeoDataCoordinates>::const_iterator itPreviousPoint = itPoint;

    TessellationFlags f = q.tessellationFlags();

    GeoDataLineString * unfinishedLineString = 0;

    GeoDataLineString * dateLineCorrected = isClosed ? new GeoDataLinearRing( f )
                                                     : new GeoDataLineString( f );

    qreal currentLon = 0.0;
    qreal previousLon = 0.0;
    int previousSign = 1;

    bool unfinished = false;

    for (; itPoint != itEndPoint; ++itPoint ) {
        currentLon = itPoint->longitude();

        int currentSign = ( currentLon < 0.0 ) ? -1 : +1 ;

        if( itPoint == q.constBegin() ) {
            previousSign = currentSign;
            previousLon  = currentLon;
        }

        // If we are crossing the date line ...
        if ( previousSign != currentSign && fabs(previousLon) + fabs(currentLon) > M_PI ) {

            unfinished = !unfinished;

            GeoDataCoordinates previousTemp;
            GeoDataCoordinates currentTemp;

            interpolateDateLine( *itPreviousPoint, *itPoint,
                                 previousTemp, currentTemp, q.tessellationFlags() );

            *dateLineCorrected << previousTemp;

            if ( isClosed && unfinished ) {
                // If it's a linear ring and if it crossed the IDL only once then
                // store the current string inside the unfinishedLineString for later use ...
                unfinishedLineString = dateLineCorrected;
                // ... and start a new linear ring for now.
                dateLineCorrected = new GeoDataLinearRing( f );
            }
            else {
                // Now it can only be a (finished) line string or a finished linear ring.
                // Store it in the vector  if the size is not zero.
                if ( dateLineCorrected->size() > 0 ) {
                    lineStrings << dateLineCorrected;
                }
                else {
                    // Or delete it.
                    delete dateLineCorrected;
                }

                // If it's a finished linear ring restore the "remembered" unfinished String
                if ( isClosed && !unfinished && unfinishedLineString ) {
                    dateLineCorrected = unfinishedLineString;
                }
                else {
                    // if it's a line string just create a new line string.
                    dateLineCorrected = new GeoDataLineString( f );
                }
            }

            *dateLineCorrected << currentTemp;
            *dateLineCorrected << *itPoint;

        }
        else {
            *dateLineCorrected << *itPoint;
        }

        previousSign = currentSign;
        previousLon  = currentLon;
        itPreviousPoint = itPoint;
    }

    // If the line string doesn't cross the dateline an even number of times
    // then need to take care of the data stored in the unfinishedLineString
    if ( unfinished && unfinishedLineString && !unfinishedLineString->isEmpty() ) {
        *dateLineCorrected << *unfinishedLineString;
        delete unfinishedLineString;
    }

    lineStrings << dateLineCorrected;
}

const GeoDataLatLonAltBox& GeoDataLineString::latLonAltBox() const
{
    // GeoDataLatLonAltBox::fromLineString is very expensive
    // that's why we recreate it only if the m_dirtyBox
    // is TRUE.
    // DO NOT REMOVE THIS CONSTRUCT OR MARBLE WILL BE SLOW.
    if ( p()->m_dirtyBox ) {
        p()->m_latLonAltBox = GeoDataLatLonAltBox::fromLineString( *this );
    }
    p()->m_dirtyBox = false;

    return p()->m_latLonAltBox;
}

qreal GeoDataLineString::length( qreal planetRadius, int offset ) const
{
    if( offset < 0 || offset >= size() ) {
        return 0;
    }

    qreal length = 0.0;
    QVector<GeoDataCoordinates> const & vector = p()->m_vector;
    int const start = qMax(offset+1, 1);
    int const end = p()->m_vector.size();
    for( int i=start; i<end; ++i )
    {
        length += distanceSphere( vector[i-1], vector[i] );
    }

    return planetRadius * length;
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator pos )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    return d->m_vector.erase( pos );
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator begin,
                                                                 QVector<GeoDataCoordinates>::Iterator end )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    delete d->m_rangeCorrected;
    d->m_rangeCorrected = 0;
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    return d->m_vector.erase( begin, end );
}

void GeoDataLineString::remove ( int i )
{
    GeoDataGeometry::detach();
    GeoDataLineStringPrivate* d = p();
    d->m_dirtyRange = true;
    d->m_dirtyBox = true;
    d->m_vector.remove( i );
}

void GeoDataLineString::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << size();
    stream << (qint32)(p()->m_tessellationFlags);

    for( QVector<GeoDataCoordinates>::const_iterator iterator
          = p()->m_vector.constBegin();
         iterator != p()->m_vector.constEnd();
         ++iterator ) {
        mDebug() << "innerRing: size" << p()->m_vector.size();
        GeoDataCoordinates coord = ( *iterator );
        coord.pack( stream );
    }

}

void GeoDataLineString::unpack( QDataStream& stream )
{
    GeoDataGeometry::detach();
    GeoDataGeometry::unpack( stream );
    qint32 size;
    qint32 tessellationFlags;

    stream >> size;
    stream >> tessellationFlags;

    p()->m_tessellationFlags = (TessellationFlags)(tessellationFlags);

    for(qint32 i = 0; i < size; i++ ) {
        GeoDataCoordinates coord;
        coord.unpack( stream );
        p()->m_vector.append( coord );
    }
}

}
