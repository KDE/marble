//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLineString.h"

#include <QtCore/QDebug>

class GeoDataLineStringPrivate
{
 public:
    GeoDataLineStringPrivate()
         : m_dirtyBox( true )
    {
    }
    bool         m_dirtyBox; // tells whether there have been changes to the
                             // GeoDataPoints since the LatLonAltBox has 
                             // been calculated. Saves performance. 
};

GeoDataLineString::GeoDataLineString()
  : QVector<GeoDataCoordinates*>(), GeoDataGeometry(),
    d( new GeoDataLineStringPrivate )
{
}

GeoDataLineString::GeoDataLineString( const GeoDataLineString & other )
  : QVector<GeoDataCoordinates*>( other ), GeoDataGeometry( other ),
    d( new GeoDataLineStringPrivate( *other.d ) )
{
}

GeoDataLineString& GeoDataLineString::operator=( const GeoDataLineString & rhs )
{
    // FIXME: check for self assignment is not needed imho, discuss if we
    // check nevertheless.
    *d = *rhs.d;
    return *this;
}

GeoDataLineString::~GeoDataLineString()
{
#if DEBUG_GEODATA
    qDebug() << "delete Linestring";
#endif
    delete d;
    qDeleteAll(*this);
}

GeoDataLatLonAltBox GeoDataLineString::latLonAltBox() const
{
    if (d->m_dirtyBox) {
    // calulate LatLonAltBox
    }
    d->m_dirtyBox = false;

    return GeoDataLatLonAltBox();
}


void GeoDataLineString::append ( GeoDataCoordinates* value )
{
    d->m_dirtyBox = true;
    QVector<GeoDataCoordinates*>::append( value );
}

void GeoDataLineString::clear()
{
    d->m_dirtyBox = true;
// possible leakage
    QVector<GeoDataCoordinates*>::clear();
}

QVector<GeoDataCoordinates*>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates*>::Iterator pos )
{
    d->m_dirtyBox = true;
    return QVector<GeoDataCoordinates*>::erase( pos );
}

QVector<GeoDataCoordinates*>::Iterator GeoDataLineString::erase ( QVector<GeoDataCoordinates*>::Iterator begin, 
                                                                 QVector<GeoDataCoordinates*>::Iterator end )
{
    d->m_dirtyBox = true;
    return QVector<GeoDataCoordinates*>::erase( begin, end );
}

void GeoDataLineString::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << size();
    
    for( QVector<GeoDataCoordinates*>::const_iterator iterator 
          = constBegin(); 
         iterator != constEnd();
         ++iterator ) {
        qDebug() << "innerRing: size" << size();
        GeoDataCoordinates* coord = ( *iterator );
        coord->pack( stream );
    }
    
}

void GeoDataLineString::unpack( QDataStream& stream )
{
    GeoDataGeometry::unpack( stream );
    int size;
    
    stream >> size;
    for(int i = 0; i < size; i++ ) {
        GeoDataCoordinates* coord = new GeoDataCoordinates();
        coord->unpack( stream );
        append( coord );
    }
}
