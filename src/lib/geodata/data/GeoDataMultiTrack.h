//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef GEODATAMULTITRACK_H
#define GEODATAMULTITRACK_H

#include "geodata_export.h"

#include "GeoDataGeometry.h"

#include <QtCore/QVector>

namespace Marble
{

class GeoDataMultiTrackPrivate;
class GeoDataTrack;

class GEODATA_EXPORT GeoDataMultiTrack : public GeoDataGeometry
{
public:
    GeoDataMultiTrack();
    GeoDataMultiTrack( const GeoDataGeometry& other );

    virtual ~GeoDataMultiTrack();

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    int size() const;
    GeoDataTrack& at( int pos );
    const GeoDataTrack& at( int pos ) const;
    GeoDataTrack& operator[]( int pos );
    const GeoDataTrack& operator[]( int pos ) const;

    GeoDataTrack& first();
    const GeoDataTrack& first() const;
    GeoDataTrack& last();
    const GeoDataTrack& last() const;

    /**
     * @brief  returns the requested child item
     */
    GeoDataTrack* child( int );

    /**
     * @brief  returns the requested child item
     */
    const GeoDataTrack* child( int ) const;

    /**
     * @brief returns the position of an item in the list
     */
    int childPosition( GeoDataTrack *child);

    /**
    * @brief add an element
    */
    void append( GeoDataTrack *other );

    GeoDataMultiTrack& operator << ( const GeoDataTrack& value );

    QVector<GeoDataTrack*>::Iterator begin();
    QVector<GeoDataTrack*>::Iterator end();
    QVector<GeoDataTrack*>::ConstIterator constBegin() const;
    QVector<GeoDataTrack*>::ConstIterator constEnd() const;
    void clear();
    QVector<GeoDataTrack> vector() const;

    QVector<GeoDataTrack*>::Iterator erase ( QVector<GeoDataTrack*>::Iterator pos );
    QVector<GeoDataTrack*>::Iterator erase ( QVector<GeoDataTrack*>::Iterator begin,
                                                  QVector<GeoDataTrack*>::Iterator end );

    // Serialize the Placemark to @p stream
    virtual void pack( QDataStream& stream ) const;
    // Unserialize the Placemark from @p stream
    virtual void unpack( QDataStream& stream );
 private:
    GeoDataMultiTrackPrivate *p() const;
};

}

#endif // GEODATAMULTITRACK_H
