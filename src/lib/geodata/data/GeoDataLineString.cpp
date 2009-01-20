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

#include <QtCore/QDebug>

#if QT_VERSION < 0x040400
# include <qatomic.h>
#else
# include <QtCore/QAtomicInt>
#endif

namespace Marble
{
#if QT_VERSION < 0x040400
    typedef QAtomic QAtomicInt;
#endif

class GeoDataLineStringPrivate
{
 public:
    GeoDataLineStringPrivate( TessellationFlags f )
         : m_dirtyBox( true ),
           m_tessellationFlags( f ),
           ref( 1 )
    {
    }

    bool         m_dirtyBox; // tells whether there have been changes to the
                             // GeoDataPoints since the LatLonAltBox has 
                             // been calculated. Saves performance. 
    TessellationFlags m_tessellationFlags;
    QAtomicInt ref;
};

GeoDataLineString::GeoDataLineString( GeoDataObject *parent, TessellationFlags f )
  : QVector<GeoDataCoordinates>(),
    GeoDataGeometry( parent ), 
    d( new GeoDataLineStringPrivate( f ) )
{
}

GeoDataLineString::GeoDataLineString( const GeoDataLineString & other )
  : QVector<GeoDataCoordinates>( other ), GeoDataGeometry( other ),
    d( other.d )
{
    d->ref.ref();
}

GeoDataLineString& GeoDataLineString::operator=( const GeoDataLineString & other )
{
    qAtomicAssign(d, other.d);
    return *this;
}

GeoDataLineString::~GeoDataLineString()
{
#ifdef DEBUG_GEODATA
    qDebug() << "delete Linestring";
#endif
    if (!d->ref.deref())
        delete d;
}

void GeoDataLineString::detach()
{
    if(d->ref == 1)
        return;

    GeoDataLineStringPrivate *new_d = new GeoDataLineStringPrivate( *d );

    if (!d->ref.deref())
        delete d;

    d = new_d;
}

bool GeoDataLineString::isClosed() const
{
    return false;
}

bool GeoDataLineString::tessellate() const
{
    return d->m_tessellationFlags.testFlag(Tessellate);
}

void GeoDataLineString::setTessellate( bool tessellate )
{
    detach();
    // According to the KML reference the tesselation of line strings in Google Earth 
    // is generally done along great circles. However for subsequent points that share 
    // the same latitude the latitude circles are followed. Our Tesselate and RespectLatitude
    // Flags provide this behaviour. For true polygons the latitude circles don't get considered. 

    if ( tessellate ) {
        d->m_tessellationFlags |= Tessellate; 
        d->m_tessellationFlags |= RespectLatitudeCircle; 
    } else {
        d->m_tessellationFlags ^= Tessellate; 
        d->m_tessellationFlags ^= RespectLatitudeCircle; 
    }
}

TessellationFlags GeoDataLineString::tessellationFlags() const
{
    return d->m_tessellationFlags;
}

void GeoDataLineString::setTessellationFlags( TessellationFlags f )
{
    detach();
    d->m_tessellationFlags = f;
}

GeoDataLatLonAltBox GeoDataLineString::latLonAltBox() const
{
    if (d->m_dirtyBox) {
        return GeoDataLatLonAltBox::fromLineString( *this );
    }
    d->m_dirtyBox = false;

    return GeoDataLatLonAltBox();
}


void GeoDataLineString::append ( const GeoDataCoordinates& value )
{
    detach();
    d->m_dirtyBox = true;
    QVector<GeoDataCoordinates>::append( value );
}

GeoDataLineString& GeoDataLineString::operator << ( const GeoDataCoordinates& value )
{
    detach();
    d->m_dirtyBox = true;
    QVector<GeoDataCoordinates>::append( value );
    return *this;
}

void GeoDataLineString::clear()
{
    detach();
    d->m_dirtyBox = true;

    QVector<GeoDataCoordinates>::clear();
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator pos )
{
    detach();
    d->m_dirtyBox = true;
    return QVector<GeoDataCoordinates>::erase( pos );
}

QVector<GeoDataCoordinates>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates>::Iterator begin, 
                                                                 QVector<GeoDataCoordinates>::Iterator end )
{
    detach();
    d->m_dirtyBox = true;
    return QVector<GeoDataCoordinates>::erase( begin, end );
}

void GeoDataLineString::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << size();
    stream << (qint32)(d->m_tessellationFlags);
    
    for( QVector<GeoDataCoordinates>::const_iterator iterator 
          = constBegin(); 
         iterator != constEnd();
         ++iterator ) {
        qDebug() << "innerRing: size" << size();
        GeoDataCoordinates coord = ( *iterator );
        coord.pack( stream );
    }
    
}

void GeoDataLineString::unpack( QDataStream& stream )
{
    detach();
    GeoDataGeometry::unpack( stream );
    qint32 size;
    qint32 tessellationFlags;

    stream >> size;
    stream >> tessellationFlags;

    d->m_tessellationFlags = (TessellationFlags)(tessellationFlags);

    for(qint32 i = 0; i < size; i++ ) {
        GeoDataCoordinates coord;
        coord.unpack( stream );
        append( coord );
    }
}

}
