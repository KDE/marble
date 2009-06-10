//
// This file is part of the Marble Desktop Globe.
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

#include <QtCore/QDebug>


namespace Marble
{
GeoDataLineString::GeoDataLineString( TessellationFlags f )
  : GeoDataGeometry( new GeoDataLineStringPrivate( f ) )
{
//    qDebug() << "1) GeoDataLineString created:" << p();
}

GeoDataLineString::GeoDataLineString( GeoDataLineStringPrivate* priv )
  : GeoDataGeometry( priv )
{
//    qDebug() << "2) GeoDataLineString created:" << p();
}

GeoDataLineString::GeoDataLineString( const GeoDataGeometry & other )
  : GeoDataGeometry( other )
{
//    qDebug() << "3) GeoDataLineString created:" << p();
}

GeoDataLineString::~GeoDataLineString()
{
#ifdef DEBUG_GEODATA
    qDebug() << "delete Linestring";
#endif
}

GeoDataLineStringPrivate* GeoDataLineString::p() const
{
    return static_cast<GeoDataLineStringPrivate*>(d);
}

int GeoDataLineString::size() const
{
    return p()->m_vector.size();
}

GeoDataCoordinates& GeoDataLineString::at( int pos )
{
    GeoDataGeometry::detach();
    return p()->m_vector[ pos ];
}

const GeoDataCoordinates& GeoDataLineString::at( int pos ) const
{
    return p()->m_vector.at( pos );
}

GeoDataCoordinates& GeoDataLineString::operator[]( int pos )
{
    GeoDataGeometry::detach();
    return p()->m_vector[ pos ];
}

const GeoDataCoordinates& GeoDataLineString::operator[]( int pos ) const
{
    return p()->m_vector[ pos ];
}

GeoDataCoordinates& GeoDataLineString::last()
{
    GeoDataGeometry::detach();
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

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::end()
{
    GeoDataGeometry::detach();
    return p()->m_vector.end();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataCoordinates>::ConstIterator GeoDataLineString::constEnd() const
{
    return p()->m_vector.constEnd();
}

void GeoDataLineString::append ( const GeoDataCoordinates& value )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    p()->m_vector.append( value );
}

GeoDataLineString& GeoDataLineString::operator << ( const GeoDataCoordinates& value )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    p()->m_vector.append( value );
    return *this;
}

void GeoDataLineString::clear()
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;

    p()->m_vector.clear();
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

GeoDataLineString GeoDataLineString::toPoleCorrected() const
{
/*
    if ( p()->m_poleCorrected ) {
        return *(p()->m_poleCorrected);
    }
    
    p()->m_poleCorrected = new GeoDataLineString( p()->m_tessellationFlags );
*/
    GeoDataLineString poleCorrected;

    poleCorrected.setTessellationFlags( tessellationFlags() );

    GeoDataCoordinates previousCoords;
    GeoDataCoordinates currentCoords;

    if ( isClosed() ) {
        if ( !( p()->m_vector.first().isPole() ) &&
              ( p()->m_vector.last().isPole() ) ) {
                qreal firstLongitude = ( p()->m_vector.first() ).longitude();
                GeoDataCoordinates modifiedCoords( p()->m_vector.last() );
                modifiedCoords.setLongitude( firstLongitude );
                poleCorrected << modifiedCoords;
//                *(p()->m_poleCorrected) << modifiedCoords;
        }
    }

    for( QVector<GeoDataCoordinates>::const_iterator itCoords
          = p()->m_vector.constBegin();
         itCoords != p()->m_vector.constEnd();
         ++itCoords ) {

        currentCoords  = *itCoords;

        if ( itCoords == p()->m_vector.constBegin() ) {
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
//                *(p()->m_poleCorrected) << currentModifiedCoords;
            }
        }
        else {
            if ( previousCoords.isPole() ) {
                qreal currentLongitude = currentCoords.longitude();
                GeoDataCoordinates previousModifiedCoords( previousCoords );
                previousModifiedCoords.setLongitude( currentLongitude );
                poleCorrected << previousModifiedCoords;
                poleCorrected << currentCoords;
//                *(p()->m_poleCorrected) << previousModifiedCoords;
//                *(p()->m_poleCorrected) << currentCoords;
            }
            else {
                // No poles at all. Nothing special to handle
                poleCorrected << currentCoords;
//                *(p()->m_poleCorrected) << currentCoords;
            }
        }
        previousCoords = currentCoords;
    }

    if ( isClosed() ) {
        if (  ( p()->m_vector.first().isPole() ) &&
             !( p()->m_vector.last().isPole() ) ) {
                qreal lastLongitude = ( p()->m_vector.last() ).longitude();
                GeoDataCoordinates modifiedCoords( p()->m_vector.first() );
                modifiedCoords.setLongitude( lastLongitude );
                poleCorrected << modifiedCoords;
//                *(p()->m_poleCorrected) << modifiedCoords;
        }
    }
    
    return poleCorrected;
//    return *(p()->m_poleCorrected);
}

QVector<GeoDataLineString> GeoDataLineString::toRangeCorrected() const
{
    if ( p()->m_dirtyRange ) {

        p()->m_rangeCorrected.clear();

        // Normalization
        // TODO

        // PoleCorrection
        GeoDataLineString poleCorrectedLineString;
        poleCorrectedLineString = toPoleCorrected();

        // DateLine Correction
        p()->m_rangeCorrected = poleCorrectedLineString.toDateLineCorrected();
    }

    return p()->m_rangeCorrected;
}


QVector<GeoDataLineString> GeoDataLineString::toDateLineCorrected() const
{
//    qDebug() << Q_FUNC_INFO;
    QVector<GeoDataLineString> lineStrings;

    const QVector<GeoDataCoordinates>::const_iterator itStartPoint = constBegin();
    const QVector<GeoDataCoordinates>::const_iterator itEndPoint = constEnd();
    QVector<GeoDataCoordinates>::const_iterator itPoint = itStartPoint;
    QVector<GeoDataCoordinates>::const_iterator itPreviousPoint = itPoint;

    GeoDataLineString dateLineCorrected;
    dateLineCorrected.setTessellationFlags( tessellationFlags() );

    qreal currentLon = 0.0;
    qreal previousLon = 0.0;
    int previousSign = 1;

    for (; itPoint != itEndPoint; ++itPoint ) {
        currentLon = itPoint->longitude();

        int currentSign = ( currentLon > 0.0 ) ? 1 : -1 ;

        if( itPoint == constBegin() ) {
            previousSign = currentSign;
            previousLon  = currentLon;
        }

        if ( previousSign != currentSign && fabs(previousLon) + fabs(currentLon) > M_PI ) {
            qDebug() << "DateLineCorrected";
            // FIXME: Interpolate temporary points
            GeoDataCoordinates previousTemp( *itPreviousPoint );
            previousTemp.setLongitude( previousSign * M_PI );
            dateLineCorrected << previousTemp;
            lineStrings << dateLineCorrected;
            dateLineCorrected.clear();
            GeoDataCoordinates currentTemp( *itPoint );
            currentTemp.setLongitude( currentSign * M_PI );
            dateLineCorrected << currentTemp;
            dateLineCorrected << *itPoint;
            
        }
        else {
            dateLineCorrected << *itPoint;
        }

        previousSign = currentSign;
        previousLon  = currentLon;
        itPreviousPoint = itPoint;
    }

    lineStrings << dateLineCorrected;
    return lineStrings;
}

GeoDataLatLonAltBox GeoDataLineString::latLonAltBox() const
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

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator pos )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    return p()->m_vector.erase( pos );
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator begin, 
                                                                 QVector<GeoDataCoordinates>::Iterator end )
{
    GeoDataGeometry::detach();
    p()->m_dirtyRange = true;
    p()->m_dirtyBox = true;
    return p()->m_vector.erase( begin, end );
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
        qDebug() << "innerRing: size" << p()->m_vector.size();
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
