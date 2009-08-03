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

#ifndef GeoDocument_h
#define GeoDocument_h

#include <QtCore/QString>

#include "geodata_export.h"

#ifdef _WIN32
#define DUMP_GEONODE_LEAKS 1
#else
#define DUMP_GEONODE_LEAKS 0
#endif

namespace Marble
{

namespace geonode
{
/**
 * the following const char* XXType are used to provide RTTI to the GeoNode for
 * downcasting to the good object
 * please keep alphabetic order to prevent mess
 */
extern const char* GeoDataColorStyleType;
extern const char* GeoDataContainerType;
extern const char* GeoDataDocumentType;
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
extern const char* GeoDataMultiGeometryType;
extern const char* GeoDataObjectType;
extern const char* GeoDataOuterBoundaryType;
extern const char* GeoDataPlacemarkType;
extern const char* GeoDataPointType;
extern const char* GeoDataPolygonType;
extern const char* GeoDataPolyStyleType;
extern const char* GeoDataStyleSelectorType;
extern const char* GeoDataStyleType;
extern const char* GeoDataSyleMapType;
extern const char* GeoNodeType;
extern const char* GeoSceneAbstractDatasetType;
extern const char* GeoSceneDocumentType;
extern const char* GeoSceneFilterType;
extern const char* GeoSceneGroupType;
extern const char* GeoSceneHeadType;
extern const char* GeoSceneIconType;
extern const char* GeoSceneItemType;
extern const char* GeoSceneLayerType;
extern const char* GeoSceneLegendType;
extern const char* GeoSceneMapType;
extern const char* GeoScenePaletteType;
extern const char* GeoScenePropertyType;
extern const char* GeoSceneSectionType;
extern const char* GeoSceneSettingsType;
extern const char* GeoSceneTextureType;
extern const char* GeoSceneVectorType;
extern const char* GeoSceneXmlDataSourceType;
extern const char* GeoSceneZoomType;
}

/**
 * @short A shared base class between GeoDataDocument/GeoSourceDocument
 */
class GEODATA_EXPORT GeoDocument {
protected:
    GeoDocument();

public:
    virtual ~GeoDocument();

    virtual bool isGeoDataDocument() const;
    virtual bool isGeoSceneDocument() const;
#if DUMP_GEONODE_LEAKS > 0
    static unsigned long s_leakProtector;
#endif
};

/**
 * @short A shared base class for all classes that are mapped to a specifig tag (ie. GeoFolder)
 */
class GEODATA_EXPORT GeoNode {
public:
    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() { return geonode::GeoNodeType; }

protected:
    GeoNode();
    virtual ~GeoNode();
};

}

#endif // GeoDocument_h
