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

namespace Marble
{

// Lists all known KML 2.1 tags (http://code.google.com/apis/kml/documentation/kmlTag_tags_21.html)
namespace kml
{
    extern const char* kmlTag_nameSpace20;
    extern const char* kmlTag_nameSpace21;
    extern const char* kmlTag_nameSpace22;
    extern const char* kmlTag_nameSpaceOgc22;

    extern const char* kmlTag_address;
    extern const char* kmlTag_AddressDetails;
    extern const char* kmlTag_altitude;
    extern const char* kmlTag_altitudeMode;
    extern const char* kmlTag_BalloonStyle;
    extern const char* kmlTag_begin;
    extern const char* kmlTag_bgColor;
    extern const char* kmlTag_Change;
    extern const char* kmlTag_code;
    extern const char* kmlTag_color;
    extern const char* kmlTag_colorMode;
    extern const char* kmlTag_ColorStyle;
    extern const char* kmlTag_Container;
    extern const char* kmlTag_cookie;
    extern const char* kmlTag_coordinates;
    extern const char* kmlTag_Create;
    extern const char* kmlTag_Data;
    extern const char* kmlTag_Delete;
    extern const char* kmlTag_description;
    extern const char* kmlTag_displayName;
    extern const char* kmlTag_Document;
    extern const char* kmlTag_drawOrder;
    extern const char* kmlTag_east;
    extern const char* kmlTag_end;
    extern const char* kmlTag_expires;
    extern const char* kmlTag_ExtendedData;
    extern const char* kmlTag_extrude;
    extern const char* kmlTag_Feature;
    extern const char* kmlTag_fill;
    extern const char* kmlTag_flyToView;
    extern const char* kmlTag_Folder;
    extern const char* kmlTag_geomColor;
    extern const char* kmlTag_Geometry;
    extern const char* kmlTag_GeometryCollection;
    extern const char* kmlTag_geomScale;
    extern const char* kmlTag_GroundOverlay;
    extern const char* kmlTag_h;
    extern const char* kmlTag_heading;
    extern const char* kmlTag_href;
    extern const char* kmlTag_hotSpot;
    extern const char* kmlTag_httpQuery;
    extern const char* kmlTag_Icon;
    extern const char* kmlTag_IconStyle;
    extern const char* kmlTag_innerBoundaryIs;
    extern const char* kmlTag_ItemIcon;
    extern const char* kmlTag_key;
    extern const char* kmlTag_kml;
    extern const char* kmlTag_labelColor;
    extern const char* kmlTag_LabelStyle;
    extern const char* kmlTag_latitude;
    extern const char* kmlTag_LatLonAltBox;
    extern const char* kmlTag_LatLonBox;
    extern const char* kmlTag_LinearRing;
    extern const char* kmlTag_LineString;
    extern const char* kmlTag_LineStyle;
    extern const char* kmlTag_Link;
    extern const char* kmlTag_linkDescription;
    extern const char* kmlTag_linkName;
    extern const char* kmlTag_linkSnippet;
    extern const char* kmlTag_listItemType;
    extern const char* kmlTag_ListStyle;
    extern const char* kmlTag_Location;
    extern const char* kmlTag_Lod;
    extern const char* kmlTag_longitude;
    extern const char* kmlTag_LookAt;
    extern const char* kmlTag_maxAltitude;
    extern const char* kmlTag_maxFadeExtent;
    extern const char* kmlTag_maxLodPixels;
    extern const char* kmlTag_message;
    extern const char* kmlTag_Metadata;
    extern const char* kmlTag_minAltitude;
    extern const char* kmlTag_minFadeExtent;
    extern const char* kmlTag_minLodPixels;
    extern const char* kmlTag_minRefreshPeriod;
    extern const char* kmlTag_Model;
    extern const char* kmlTag_MultiGeometry;
    extern const char* kmlTag_name;
    extern const char* kmlTag_NetworkLink;
    extern const char* kmlTag_NetworkLinkControl;
    extern const char* kmlTag_north;
    extern const char* kmlTag_Object;
    extern const char* kmlTag_ObjArrayField;
    extern const char* kmlTag_ObjField;
    extern const char* kmlTag_open;
    extern const char* kmlTag_Orientation;
    extern const char* kmlTag_outerBoundaryIs;
    extern const char* kmlTag_outline;
    extern const char* kmlTag_Overlay;
    extern const char* kmlTag_overlayXY;
    extern const char* kmlTag_Pair;
    extern const char* kmlTag_phoneNumber;
    extern const char* kmlTag_Placemark;
    extern const char* kmlTag_Point;
    extern const char* kmlTag_Polygon;
    extern const char* kmlTag_PolyStyle;
    extern const char* kmlTag_range;
    extern const char* kmlTag_refreshInterval;
    extern const char* kmlTag_refreshMode;
    extern const char* kmlTag_refreshVisibility;
    extern const char* kmlTag_Region;
    extern const char* kmlTag_request;
    extern const char* kmlTag_Response;
    extern const char* kmlTag_roll;
    extern const char* kmlTag_rotation;
    extern const char* kmlTag_rotationXY;
    extern const char* kmlTag_Scale;
    extern const char* kmlTag_scale;
    extern const char* kmlTag_Schema;
    extern const char* kmlTag_SchemaField;
    extern const char* kmlTag_ScreenOverlay;
    extern const char* kmlTag_screenXY;
    extern const char* kmlTag_SimpleArrayField;
    extern const char* kmlTag_SimpleField;
    extern const char* kmlTag_size;
    extern const char* kmlTag_Snippet;
    extern const char* kmlTag_south;
    extern const char* kmlTag_state;
    extern const char* kmlTag_Status;
    extern const char* kmlTag_Style;
    extern const char* kmlTag_StyleMap;
    extern const char* kmlTag_StyleSelector;
    extern const char* kmlTag_styleUrl;
    extern const char* kmlTag_targetHref;
    extern const char* kmlTag_tessellate;
    extern const char* kmlTag_text;
    extern const char* kmlTag_textcolor;
    extern const char* kmlTag_tilt;
    extern const char* kmlTag_TimeSpan;
    extern const char* kmlTag_TimeStamp;
    extern const char* kmlTag_Update;
    extern const char* kmlTag_Url;
    extern const char* kmlTag_value;
    extern const char* kmlTag_viewBoundScale;
    extern const char* kmlTag_viewFormat;
    extern const char* kmlTag_viewRefreshMode;
    extern const char* kmlTag_viewRefreshTime;
    extern const char* kmlTag_visibility;
    extern const char* kmlTag_w;
    extern const char* kmlTag_west;
    extern const char* kmlTag_when;
    extern const char* kmlTag_width;
    extern const char* kmlTag_x;
    extern const char* kmlTag_y;
    
#ifdef KML_LAZY_IMP
// preliminary implementation for the extra defined Tags
    extern const char* kmlTag_MarblePlacemark;
    extern const char* kmlTag_area;
    extern const char* kmlTag_pop;
    extern const char* kmlTag_role;
    extern const char* kmlTag_CountryNameCode;
    extern const char* kmlTag_countrycode;
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
