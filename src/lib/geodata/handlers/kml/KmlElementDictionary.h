/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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

#ifndef MARBLE_KML_KMLELEMENTDICTIONARY_H
#define MARBLE_KML_KMLELEMENTDICTIONARY_H

#include "geodata_export.h"

namespace Marble
{

// Lists all known KML 2.1 tags (http://code.google.com/apis/kml/documentation/kmlTag_tags_21.html)
namespace kml
{
    GEODATA_EXPORT extern const char* kmlTag_nameSpace20;
    GEODATA_EXPORT extern const char* kmlTag_nameSpace21;
    GEODATA_EXPORT extern const char* kmlTag_nameSpace22;
    GEODATA_EXPORT extern const char* kmlTag_nameSpaceOgc22;

    GEODATA_EXPORT extern const char* kmlTag_address;
    GEODATA_EXPORT extern const char* kmlTag_AddressDetails;
    GEODATA_EXPORT extern const char* kmlTag_altitude;
    GEODATA_EXPORT extern const char* kmlTag_altitudeMode;
    GEODATA_EXPORT extern const char* kmlTag_BalloonStyle;
    GEODATA_EXPORT extern const char* kmlTag_begin;
    GEODATA_EXPORT extern const char* kmlTag_bgColor;
    GEODATA_EXPORT extern const char* kmlTag_Change;
    GEODATA_EXPORT extern const char* kmlTag_code;
    GEODATA_EXPORT extern const char* kmlTag_color;
    GEODATA_EXPORT extern const char* kmlTag_colorMode;
    GEODATA_EXPORT extern const char* kmlTag_ColorStyle;
    GEODATA_EXPORT extern const char* kmlTag_Container;
    GEODATA_EXPORT extern const char* kmlTag_cookie;
    GEODATA_EXPORT extern const char* kmlTag_coordinates;
    GEODATA_EXPORT extern const char* kmlTag_Create;
    GEODATA_EXPORT extern const char* kmlTag_Data;
    GEODATA_EXPORT extern const char* kmlTag_Delete;
    GEODATA_EXPORT extern const char* kmlTag_description;
    GEODATA_EXPORT extern const char* kmlTag_displayName;
    GEODATA_EXPORT extern const char* kmlTag_Document;
    GEODATA_EXPORT extern const char* kmlTag_drawOrder;
    GEODATA_EXPORT extern const char* kmlTag_east;
    GEODATA_EXPORT extern const char* kmlTag_end;
    GEODATA_EXPORT extern const char* kmlTag_expires;
    GEODATA_EXPORT extern const char* kmlTag_ExtendedData;
    GEODATA_EXPORT extern const char* kmlTag_extrude;
    GEODATA_EXPORT extern const char* kmlTag_Feature;
    GEODATA_EXPORT extern const char* kmlTag_fill;
    GEODATA_EXPORT extern const char* kmlTag_flyToView;
    GEODATA_EXPORT extern const char* kmlTag_Folder;
    GEODATA_EXPORT extern const char* kmlTag_geomColor;
    GEODATA_EXPORT extern const char* kmlTag_Geometry;
    GEODATA_EXPORT extern const char* kmlTag_GeometryCollection;
    GEODATA_EXPORT extern const char* kmlTag_geomScale;
    GEODATA_EXPORT extern const char* kmlTag_GroundOverlay;
    GEODATA_EXPORT extern const char* kmlTag_h;
    GEODATA_EXPORT extern const char* kmlTag_heading;
    GEODATA_EXPORT extern const char* kmlTag_href;
    GEODATA_EXPORT extern const char* kmlTag_hotSpot;
    GEODATA_EXPORT extern const char* kmlTag_httpQuery;
    GEODATA_EXPORT extern const char* kmlTag_Icon;
    GEODATA_EXPORT extern const char* kmlTag_IconStyle;
    GEODATA_EXPORT extern const char* kmlTag_innerBoundaryIs;
    GEODATA_EXPORT extern const char* kmlTag_ItemIcon;
    GEODATA_EXPORT extern const char* kmlTag_key;
    GEODATA_EXPORT extern const char* kmlTag_kml;
    GEODATA_EXPORT extern const char* kmlTag_labelColor;
    GEODATA_EXPORT extern const char* kmlTag_LabelStyle;
    GEODATA_EXPORT extern const char* kmlTag_latitude;
    GEODATA_EXPORT extern const char* kmlTag_LatLonAltBox;
    GEODATA_EXPORT extern const char* kmlTag_LatLonBox;
    GEODATA_EXPORT extern const char* kmlTag_LinearRing;
    GEODATA_EXPORT extern const char* kmlTag_LineString;
    GEODATA_EXPORT extern const char* kmlTag_LineStyle;
    GEODATA_EXPORT extern const char* kmlTag_Link;
    GEODATA_EXPORT extern const char* kmlTag_linkDescription;
    GEODATA_EXPORT extern const char* kmlTag_linkName;
    GEODATA_EXPORT extern const char* kmlTag_linkSnippet;
    GEODATA_EXPORT extern const char* kmlTag_listItemType;
    GEODATA_EXPORT extern const char* kmlTag_ListStyle;
    GEODATA_EXPORT extern const char* kmlTag_Location;
    GEODATA_EXPORT extern const char* kmlTag_Lod;
    GEODATA_EXPORT extern const char* kmlTag_longitude;
    GEODATA_EXPORT extern const char* kmlTag_LookAt;
    GEODATA_EXPORT extern const char* kmlTag_maxAltitude;
    GEODATA_EXPORT extern const char* kmlTag_maxFadeExtent;
    GEODATA_EXPORT extern const char* kmlTag_maxLodPixels;
    GEODATA_EXPORT extern const char* kmlTag_message;
    GEODATA_EXPORT extern const char* kmlTag_Metadata;
    GEODATA_EXPORT extern const char* kmlTag_minAltitude;
    GEODATA_EXPORT extern const char* kmlTag_minFadeExtent;
    GEODATA_EXPORT extern const char* kmlTag_minLodPixels;
    GEODATA_EXPORT extern const char* kmlTag_minRefreshPeriod;
    GEODATA_EXPORT extern const char* kmlTag_Model;
    GEODATA_EXPORT extern const char* kmlTag_MultiGeometry;
    GEODATA_EXPORT extern const char* kmlTag_name;
    GEODATA_EXPORT extern const char* kmlTag_NetworkLink;
    GEODATA_EXPORT extern const char* kmlTag_NetworkLinkControl;
    GEODATA_EXPORT extern const char* kmlTag_north;
    GEODATA_EXPORT extern const char* kmlTag_Object;
    GEODATA_EXPORT extern const char* kmlTag_ObjArrayField;
    GEODATA_EXPORT extern const char* kmlTag_ObjField;
    GEODATA_EXPORT extern const char* kmlTag_open;
    GEODATA_EXPORT extern const char* kmlTag_Orientation;
    GEODATA_EXPORT extern const char* kmlTag_outerBoundaryIs;
    GEODATA_EXPORT extern const char* kmlTag_outline;
    GEODATA_EXPORT extern const char* kmlTag_Overlay;
    GEODATA_EXPORT extern const char* kmlTag_overlayXY;
    GEODATA_EXPORT extern const char* kmlTag_Pair;
    GEODATA_EXPORT extern const char* kmlTag_phoneNumber;
    GEODATA_EXPORT extern const char* kmlTag_Placemark;
    GEODATA_EXPORT extern const char* kmlTag_Point;
    GEODATA_EXPORT extern const char* kmlTag_Polygon;
    GEODATA_EXPORT extern const char* kmlTag_PolyStyle;
    GEODATA_EXPORT extern const char* kmlTag_range;
    GEODATA_EXPORT extern const char* kmlTag_refreshInterval;
    GEODATA_EXPORT extern const char* kmlTag_refreshMode;
    GEODATA_EXPORT extern const char* kmlTag_refreshVisibility;
    GEODATA_EXPORT extern const char* kmlTag_Region;
    GEODATA_EXPORT extern const char* kmlTag_request;
    GEODATA_EXPORT extern const char* kmlTag_Response;
    GEODATA_EXPORT extern const char* kmlTag_roll;
    GEODATA_EXPORT extern const char* kmlTag_rotation;
    GEODATA_EXPORT extern const char* kmlTag_rotationXY;
    GEODATA_EXPORT extern const char* kmlTag_Scale;
    GEODATA_EXPORT extern const char* kmlTag_scale;
    GEODATA_EXPORT extern const char* kmlTag_Schema;
    GEODATA_EXPORT extern const char* kmlTag_SchemaField;
    GEODATA_EXPORT extern const char* kmlTag_ScreenOverlay;
    GEODATA_EXPORT extern const char* kmlTag_screenXY;
    GEODATA_EXPORT extern const char* kmlTag_SimpleArrayField;
    GEODATA_EXPORT extern const char* kmlTag_SimpleField;
    GEODATA_EXPORT extern const char* kmlTag_size;
    GEODATA_EXPORT extern const char* kmlTag_Snippet;
    GEODATA_EXPORT extern const char* kmlTag_south;
    GEODATA_EXPORT extern const char* kmlTag_state;
    GEODATA_EXPORT extern const char* kmlTag_Status;
    GEODATA_EXPORT extern const char* kmlTag_Style;
    GEODATA_EXPORT extern const char* kmlTag_StyleMap;
    GEODATA_EXPORT extern const char* kmlTag_StyleSelector;
    GEODATA_EXPORT extern const char* kmlTag_styleUrl;
    GEODATA_EXPORT extern const char* kmlTag_targetHref;
    GEODATA_EXPORT extern const char* kmlTag_tessellate;
    GEODATA_EXPORT extern const char* kmlTag_text;
    GEODATA_EXPORT extern const char* kmlTag_textcolor;
    GEODATA_EXPORT extern const char* kmlTag_tilt;
    GEODATA_EXPORT extern const char* kmlTag_TimeSpan;
    GEODATA_EXPORT extern const char* kmlTag_TimeStamp;
    GEODATA_EXPORT extern const char* kmlTag_Update;
    GEODATA_EXPORT extern const char* kmlTag_Url;
    GEODATA_EXPORT extern const char* kmlTag_value;
    GEODATA_EXPORT extern const char* kmlTag_viewBoundScale;
    GEODATA_EXPORT extern const char* kmlTag_viewFormat;
    GEODATA_EXPORT extern const char* kmlTag_viewRefreshMode;
    GEODATA_EXPORT extern const char* kmlTag_viewRefreshTime;
    GEODATA_EXPORT extern const char* kmlTag_visibility;
    GEODATA_EXPORT extern const char* kmlTag_w;
    GEODATA_EXPORT extern const char* kmlTag_west;
    GEODATA_EXPORT extern const char* kmlTag_when;
    GEODATA_EXPORT extern const char* kmlTag_width;
    GEODATA_EXPORT extern const char* kmlTag_x;
    GEODATA_EXPORT extern const char* kmlTag_y;
    
#ifdef KML_LAZY_IMP
// preliminary implementation for the extra defined Tags
    GEODATA_EXPORT extern const char* kmlTag_MarblePlacemark;
    GEODATA_EXPORT extern const char* kmlTag_area;
    GEODATA_EXPORT extern const char* kmlTag_pop;
    GEODATA_EXPORT extern const char* kmlTag_role;
    GEODATA_EXPORT extern const char* kmlTag_CountryNameCode;
    GEODATA_EXPORT extern const char* kmlTag_countrycode;
#endif // KML_LAZY_IMP
}

// Helper macros
#define KML_DEFINE_TAG_HANDLER_20(Name) GEODATA_DEFINE_TAG_HANDLER(kml, Kml, Name, kmlTag_nameSpace20)
#define KML_DEFINE_TAG_HANDLER_21(Name) GEODATA_DEFINE_TAG_HANDLER(kml, Kml, Name, kmlTag_nameSpace21)
#define KML_DEFINE_TAG_HANDLER_22(Name) GEODATA_DEFINE_TAG_HANDLER(kml, Kml, Name, kmlTag_nameSpace22)
#define KML_DEFINE_TAG_HANDLER_OGC22(Name) GEODATA_DEFINE_TAG_HANDLER(kml, Kml, Name, kmlTag_nameSpaceOgc22)

#define KML_DEFINE_TAG_HANDLER(Name) \
	KML_DEFINE_TAG_HANDLER_20(Name) \
	KML_DEFINE_TAG_HANDLER_21(Name) \
    KML_DEFINE_TAG_HANDLER_22(Name) \
    KML_DEFINE_TAG_HANDLER_OGC22(Name)

}

#endif
