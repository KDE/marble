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

namespace Marble
{


namespace GeoDataTypes
{
/**
 * the following const char* XXType are used to provide RTTI to the GeoData for
 * downcasting to the good object
 * please keep alphabetic order to prevent mess
 */
extern const char* GeoDataColorStyleType;
extern const char* GeoDataContainerType;
extern const char* GeoDataDataType;
extern const char* GeoDataDocumentType;
extern const char* GeoDataExtendedDataType;
extern const char* GeoDataFeatureType;
extern const char* GeoDataFolderType;
extern const char* GeoDataGeometryType;
extern const char* GeoDataHotspotType;
extern const char* GeoDataIconStyleType;
extern const char* GeoDataInnerBoundaryType;
extern const char* GeoDataLabelStyleType;
extern const char* GeoDataLatLonAltBoxType;
extern const char* GeoDataLatLonBoxType;
extern const char* GeoDataLinearRingType;
extern const char* GeoDataLineStringType;
extern const char* GeoDataLineStyleType;
extern const char* GeoDataLodType;
extern const char* GeoDataLookAtType;
extern const char* GeoDataMultiGeometryType;
extern const char* GeoDataObjectType;
extern const char* GeoDataOuterBoundaryType;
extern const char* GeoDataPlacemarkType;
extern const char* GeoDataPointType;
extern const char* GeoDataPolygonType;
extern const char* GeoDataPolyStyleType;
extern const char* GeoDataRegionType;
extern const char* GeoDataSimpleArrayDataType;
extern const char* GeoDataStyleSelectorType;
extern const char* GeoDataStyleType;
extern const char* GeoDataStyleMapType;
extern const char* GeoDataTimePrimitiveType;
extern const char* GeoDataTimeStampType;
extern const char* GeoDataTimeSpanType;
extern const char* GeoDataTrackType;
}

}

#endif
