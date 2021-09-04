// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
//


#ifndef MARBLE_SERIALIZABLE_H
#define MARBLE_SERIALIZABLE_H

class QDataStream;

namespace Marble {

class Serializable
{
 public:
    virtual ~Serializable(){}
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
    GeoDataMultiTrackId,
    GeoDataModelId,
    GeoDataTrackId,
    GeoDataBuildingId
};

}

#endif
