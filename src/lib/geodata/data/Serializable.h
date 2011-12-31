//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class QDataStream;

class Serializable
{
 public:
    virtual ~Serializable(){};
    virtual void pack( QDataStream& stream ) const = 0;
    virtual void unpack( QDataStream& stream ) = 0;
};

/* the next two id's are needed to get unpacking working - this cannot be
 * achieved without a special Id
 */
enum EnumFeatureId {
    InvalidFeatureId = -1,
    GeoDataDocumentId = 1,
    GeoDataFolderId,
    GeoDataPlacemarkId,
    GeoDataNetworkLinkId,
    GeoDataScreenOverlayId,
    GeoDataGroundOverlayId
};

enum EnumGeometryId {
    InvalidGeometryId = -1,
    GeoDataPointId = 1,
    GeoDataLineStringId,
    GeoDataLinearRingId,
    GeoDataPolygonId,
    GeoDataMultiGeometryId,
    GeoDataModelId,
    GeoDataTrackId
};
#endif
