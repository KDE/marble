//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef GEODATAMULTIGEOMETRY_H
#define GEODATAMULTIGEOMETRY_H


#include "geodata_export.h"

#include "GeoDataGeometry.h"
#include <QtCore/QVector>

namespace Marble
{

/**
 * @short A class that can contain other GeoDataGeometry objects
 *
 * GeoDataMultiGeometry is a collection of other GeoDataGeometry objects.
 * As one can add GeoDataMultiGeometry to itself, you can make up a collection
 * of different objects to form one Placemark.
 */

class GEODATA_EXPORT GeoDataMultiGeometry : public QVector<GeoDataGeometry*>,
                                            public GeoDataGeometry {
 public:
    GeoDataMultiGeometry();
    GeoDataMultiGeometry( const GeoDataMultiGeometry& );

    virtual ~GeoDataMultiGeometry();

    // Serialize the Placemark to @p stream
    virtual void pack( QDataStream& stream ) const;
    // Unserialize the Placemark from @p stream
    virtual void unpack( QDataStream& stream );

    virtual EnumGeometryId geometryId() const { return GeoDataMultiGeometryId; };
};

}

#endif // GEODATAMULTIGEOMETRY_H
