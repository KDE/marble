/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEODATATYPES_H
#define MARBLE_GEODATATYPES_H

#include "geodata_export.h"

namespace Marble
{

namespace GeoDataTypes
{
/**
 * the following const char* XXType are used to provide RTTI to the GeoData for
 * downcasting to the good object
 * please keep alphabetic order to prevent mess
 */
GEODATA_EXPORT extern const char* GeoDataCameraType;
GEODATA_EXPORT extern const char* GeoDataColorStyleType;
GEODATA_EXPORT extern const char* GeoDataContainerType;
GEODATA_EXPORT extern const char* GeoDataDataType;
GEODATA_EXPORT extern const char* GeoDataDocumentType;
GEODATA_EXPORT extern const char* GeoDataExtendedDataType;
GEODATA_EXPORT extern const char* GeoDataFeatureType;
GEODATA_EXPORT extern const char* GeoDataFolderType;
GEODATA_EXPORT extern const char* GeoDataGeometryType;
GEODATA_EXPORT extern const char* GeoDataGroundOverlayType;
GEODATA_EXPORT extern const char* GeoDataHotspotType;
GEODATA_EXPORT extern const char* GeoDataIconStyleType;
GEODATA_EXPORT extern const char* GeoDataInnerBoundaryType;
GEODATA_EXPORT extern const char* GeoDataLabelStyleType;
GEODATA_EXPORT extern const char* GeoDataLatLonAltBoxType;
GEODATA_EXPORT extern const char* GeoDataLatLonBoxType;
GEODATA_EXPORT extern const char* GeoDataLinearRingType;
GEODATA_EXPORT extern const char* GeoDataLineStringType;
GEODATA_EXPORT extern const char* GeoDataLineStyleType;
GEODATA_EXPORT extern const char* GeoDataLodType;
GEODATA_EXPORT extern const char* GeoDataLookAtType;
GEODATA_EXPORT extern const char* GeoDataMultiGeometryType;
GEODATA_EXPORT extern const char* GeoDataMultiTrackType;
GEODATA_EXPORT extern const char* GeoDataObjectType;
GEODATA_EXPORT extern const char* GeoDataOuterBoundaryType;
GEODATA_EXPORT extern const char* GeoDataPhotoOverlayType;
GEODATA_EXPORT extern const char* GeoDataPlacemarkType;
GEODATA_EXPORT extern const char* GeoDataPointType;
GEODATA_EXPORT extern const char* GeoDataPolygonType;
GEODATA_EXPORT extern const char* GeoDataPolyStyleType;
GEODATA_EXPORT extern const char* GeoDataRegionType;
GEODATA_EXPORT extern const char* GeoDataSimpleArrayDataType;
GEODATA_EXPORT extern const char* GeoDataStyleSelectorType;
GEODATA_EXPORT extern const char* GeoDataStyleType;
GEODATA_EXPORT extern const char* GeoDataStyleMapType;
GEODATA_EXPORT extern const char* GeoDataTimePrimitiveType;
GEODATA_EXPORT extern const char* GeoDataTimeStampType;
GEODATA_EXPORT extern const char* GeoDataTimeSpanType;
GEODATA_EXPORT extern const char* GeoDataTrackType;
GEODATA_EXPORT extern const char* GeoDataScreenOverlayType;
GEODATA_EXPORT extern const char* GeoDataBalloonStyleType;
GEODATA_EXPORT extern const char* GeoDataListStyleType;
GEODATA_EXPORT extern const char* GeoDataItemIconType;
GEODATA_EXPORT extern const char* GeoDataImagePyramidType;
GEODATA_EXPORT extern const char* GeoDataViewVolumeType;
}

}

#endif
