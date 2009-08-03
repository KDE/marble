/*
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

#include "GeoDocument.h"
#include <cstdio>

namespace Marble
{
#if DUMP_GEONODE_LEAKS > 0
     // Initialize here, as there is no GeoDocument.cpp file     
     unsigned long GeoDocument::s_leakProtector = 0;     
#endif      

namespace geonode
{
const char* GeoDataColorStyleType = "GeoDataColorStyle";
const char* GeoDataContainerType = "GeoDataContainer";
const char* GeoDataDocumentType = "GeoDataDocument";
const char* GeoDataFeatureType = "GeoDataFeature";
const char* GeoDataFolderType = "GeoDataFolder";
const char* GeoDataGeometryType = "GeoDataGeometry";
const char* GeoDataHotspotType = "GeoDataHotspot";
const char* GeoDataIconStyleType = "GeoDataIconStyle";
const char* GeoDataInnerBoundaryType = "GeoDataInnerBoundary";
const char* GeoDataLabelStyleType = "GeoDataLabelStyle";
const char* GeoDataLatLonAltBoxType = "GeoDataLatLonAlt";
const char* GeoDataLatLonBoxType = "GeoDataLatLonBox";
const char* GeoDataLinearRingType = "GeoDataLinearRing";
const char* GeoDataLineStringType = "GeoDataLineString";
const char* GeoDataLineStyleType = "GeoDataLineStyle";
const char* GeoDataMultiGeometryType = "GeoDataMultyGeometry";
const char* GeoDataObjectType = "GeoDataObject";
const char* GeoDataOuterBoundaryType = "GeoDataOuterBoundary";
const char* GeoDataPlacemarkType = "GeoDataPlacemark";
const char* GeoDataPointType = "GeoDataPoint";
const char* GeoDataPolygonType = "GeoDataPolygon";
const char* GeoDataPolyStyleType = "GeoDataPolyStyle";
const char* GeoDataStyleSelectorType = "GeoDataStyleSelector";
const char* GeoDataStyleType = "GeoDataStyle";
const char* GeoDataSyleMapType = "GeoDataStyleMap";
const char* GeoNodeType = "GeoNode";
const char* GeoSceneAbstractDatasetType = "GeoSceneAbstractDataset";
const char* GeoSceneDocumentType = "GeoSceneDocument";
const char* GeoSceneFilterType = "GeoSceneFilter";
const char* GeoSceneGroupType = "GeoSceneGroup";
const char* GeoSceneHeadType = "GeoSceneHead";
const char* GeoSceneIconType = "GeoSceneIcon";
const char* GeoSceneItemType = "GeoSceneItem";
const char* GeoSceneLayerType = "GeoSceneLayer";
const char* GeoSceneLegendType = "GeoSceneLegend";
const char* GeoSceneMapType = "GeoSceneMap";
const char* GeoScenePaletteType = "GeoScenePalette";
const char* GeoScenePropertyType = "GeoSceneProperty";
const char* GeoSceneSectionType = "GeoSceneSection";
const char* GeoSceneSettingsType = "GeoSceneSettings";
const char* GeoSceneTextureType = "GeoSceneTexture";
const char* GeoSceneVectorType = "GeoSceneVector";
const char* GeoSceneXmlDataSourceType = "GeoSceneXmlDataSource";
const char* GeoSceneZoomType = "GeoSceneZoom";
}


GeoDocument::GeoDocument()
{
}

GeoDocument::~GeoDocument()
{
#if DUMP_GEONODE_LEAKS > 0
    if (s_leakProtector != 0) {
        fprintf(stderr, "Found %li GeoNode object LEAKS!\n", s_leakProtector);
        s_leakProtector = 0;
    } else {
        fprintf(stderr, "No GeoNode object leak!\n");
    }
#endif
}

bool GeoDocument::isGeoDataDocument() const
{
    return false;
}

bool GeoDocument::isGeoSceneDocument() const
{
    return false;
}


GeoNode::GeoNode()
{
#if DUMP_GEONODE_LEAKS > 0
    GeoDocument::s_leakProtector++;
#endif
}

GeoNode::~GeoNode()
{
#if DUMP_GEONODE_LEAKS > 0
    --GeoDocument::s_leakProtector;
#endif
}

}
