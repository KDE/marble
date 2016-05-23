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

#include "KmlElementDictionary.h"

namespace Marble
{

// One static list of all tag names, to avoid string comparisons all-over-the-place
namespace kml
{

const char kmlTag_nameSpace20[] = "http://earth.google.com/kml/2.0";
const char kmlTag_nameSpace21[] = "http://earth.google.com/kml/2.1";
const char kmlTag_nameSpace22[] = "http://earth.google.com/kml/2.2";
const char kmlTag_nameSpaceOgc22[] = "http://www.opengis.net/kml/2.2";
const char kmlTag_nameSpaceGx22[] = "http://www.google.com/kml/ext/2.2";
const char kmlTag_nameSpaceMx[] = "http://marble.kde.org";

const char kmlTag_address[] = "address";
const char kmlTag_AddressDetails[] = "AddressDetails";
const char kmlTag_Alias[] = "Alias";
const char kmlTag_altitude[] = "altitude";
const char kmlTag_altitudeMode[] = "altitudeMode";
const char kmlTag_BalloonStyle[] = "BalloonStyle";
const char kmlTag_begin[] = "begin";
const char kmlTag_bgColor[] = "bgColor";
const char kmlTag_bottomFov[] = "bottomFov";
const char kmlTag_Camera[] = "Camera";
const char kmlTag_Change[] = "Change";
const char kmlTag_code[] = "code";
const char kmlTag_color[] = "color";
const char kmlTag_colorMode[] = "colorMode";
const char kmlTag_ColorStyle[] = "ColorStyle";
const char kmlTag_Container[] = "Container";
const char kmlTag_cookie[] = "cookie";
const char kmlTag_coordinates[] = "coordinates";
const char kmlTag_Create[] = "Create";
const char kmlTag_Data[] = "Data";
const char kmlTag_Delete[] = "Delete";
const char kmlTag_description[] = "description";
const char kmlTag_displayMode[] = "displayMode";
const char kmlTag_displayName[] = "displayName";
const char kmlTag_Document[] = "Document";
const char kmlTag_drawOrder[] = "drawOrder";
const char kmlTag_duration[] = "duration";
const char kmlTag_east[] = "east";
const char kmlTag_end[] = "end";
const char kmlTag_expires[] = "expires";
const char kmlTag_ExtendedData[] = "ExtendedData";
const char kmlTag_extrude[] = "extrude";
const char kmlTag_Feature[] = "Feature";
const char kmlTag_fill[] = "fill";
const char kmlTag_flyToMode[] = "flyToMode";
const char kmlTag_flyToView[] = "flyToView";
const char kmlTag_Folder[] = "Folder";
const char kmlTag_geomColor[] = "geomColor";
const char kmlTag_Geometry[] = "Geometry";
const char kmlTag_GeometryCollection[] = "GeometryCollection";
const char kmlTag_geomScale[] = "geomScale";
const char kmlTag_gridOrigin[] = "gridOrigin";
const char kmlTag_GroundOverlay[] = "GroundOverlay";
const char kmlTag_h[] = "h";
const char kmlTag_heading[] = "heading";
const char kmlTag_href[] = "href";
const char kmlTag_hotSpot[] = "hotSpot";
const char kmlTag_httpQuery[] = "httpQuery";
const char kmlTag_Icon[] = "Icon";
const char kmlTag_IconStyle[] = "IconStyle";
const char kmlTag_ImagePyramid[] = "ImagePyramid";
const char kmlTag_innerBoundaryIs[] = "innerBoundaryIs";
const char kmlTag_ItemIcon[] = "ItemIcon";
const char kmlTag_key[] = "key";
const char kmlTag_kml[] = "kml";
const char kmlTag_labelColor[] = "labelColor";
const char kmlTag_LabelStyle[] = "LabelStyle";
const char kmlTag_latitude[] = "latitude";
const char kmlTag_LatLonAltBox[] = "LatLonAltBox";
const char kmlTag_LatLonBox[] = "LatLonBox";
const char kmlTag_LatLonQuad[] = "LatLonQuad";
const char kmlTag_leftFov[] = "leftFov";
const char kmlTag_LinearRing[] = "LinearRing";
const char kmlTag_LineString[] = "LineString";
const char kmlTag_LineStyle[] = "LineStyle";
const char kmlTag_Link[] = "Link";
const char kmlTag_linkDescription[] = "linkDescription";
const char kmlTag_linkName[] = "linkName";
const char kmlTag_linkSnippet[] = "linkSnippet";
const char kmlTag_listItemType[] = "listItemType";
const char kmlTag_ListStyle[] = "ListStyle";
const char kmlTag_Location[] = "Location";
const char kmlTag_Lod[] = "Lod";
const char kmlTag_longitude[] = "longitude";
const char kmlTag_LookAt[] = "LookAt";
const char kmlTag_maxAltitude[] = "maxAltitude";
const char kmlTag_maxFadeExtent[] = "maxFadeExtent";
const char kmlTag_maxHeight[] = "maxHeight";
const char kmlTag_maxLodPixels[] = "maxLodPixels";
const char kmlTag_maxSessionLength[] = "maxSessionLength";
const char kmlTag_maxWidth[] = "maxWidth";
const char kmlTag_message[] = "message";
const char kmlTag_Metadata[] = "Metadata";
const char kmlTag_minAltitude[] = "minAltitude";
const char kmlTag_minFadeExtent[] = "minFadeExtent";
const char kmlTag_minLodPixels[] = "minLodPixels";
const char kmlTag_minRefreshPeriod[] = "minRefreshPeriod";
const char kmlTag_Model[] = "Model";
const char kmlTag_MultiGeometry[] = "MultiGeometry";
const char kmlTag_name[] = "name";
const char kmlTag_near[] = "near";
const char kmlTag_NetworkLink[] = "NetworkLink";
const char kmlTag_NetworkLinkControl[] = "NetworkLinkControl";
const char kmlTag_north[] = "north";
const char kmlTag_Object[] = "Object";
const char kmlTag_ObjArrayField[] = "ObjArrayField";
const char kmlTag_ObjField[] = "ObjField";
const char kmlTag_open[] = "open";
const char kmlTag_Orientation[] = "Orientation";
const char kmlTag_OsmPlacemarkData[] = "OsmPlacemarkData";
const char kmlTag_member[] = "member";
const char kmlTag_nd[] = "nd";
const char kmlTag_tag[] = "tag";
const char kmlTag_outerBoundaryIs[] = "outerBoundaryIs";
const char kmlTag_outline[] = "outline";
const char kmlTag_Overlay[] = "Overlay";
const char kmlTag_overlayXY[] = "overlayXY";
const char kmlTag_Pair[] = "Pair";
const char kmlTag_phoneNumber[] = "phoneNumber";
const char kmlTag_PhotoOverlay[] = "PhotoOverlay";
const char kmlTag_Placemark[] = "Placemark";
const char kmlTag_Point[] = "Point";
const char kmlTag_Polygon[] = "Polygon";
const char kmlTag_PolyStyle[] = "PolyStyle";
const char kmlTag_range[] = "range";
const char kmlTag_refreshInterval[] = "refreshInterval";
const char kmlTag_refreshMode[] = "refreshMode";
const char kmlTag_refreshVisibility[] = "refreshVisibility";
const char kmlTag_Region[] = "Region";
const char kmlTag_request[] = "request";
const char kmlTag_ResourceMap[] = "ResourceMap";
const char kmlTag_Response[] = "Response";
const char kmlTag_rightFov[] = "rightFov";
const char kmlTag_roll[] = "roll";
const char kmlTag_rotation[] = "rotation";
const char kmlTag_rotationXY[] = "rotationXY";
const char kmlTag_Scale[] = "Scale";
const char kmlTag_scale[] = "scale";
const char kmlTag_Schema[] = "Schema";
const char kmlTag_SchemaData[] = "SchemaData";
const char kmlTag_SchemaField[] = "SchemaField";
const char kmlTag_ScreenOverlay[] = "ScreenOverlay";
const char kmlTag_screenXY[] = "screenXY";
const char kmlTag_shape[] = "shape";
const char kmlTag_SimpleArrayData[] = "SimpleArrayData";
const char kmlTag_SimpleArrayField[] = "SimpleArrayField";
const char kmlTag_SimpleData[] = "SimpleData";
const char kmlTag_SimpleField[] = "SimpleField";
const char kmlTag_size[] = "size";
const char kmlTag_Snippet[] = "Snippet";
const char kmlTag_sourceHref[] = "sourceHref";
const char kmlTag_south[] = "south";
const char kmlTag_state[] = "state";
const char kmlTag_Status[] = "Status";
const char kmlTag_Style[] = "Style";
const char kmlTag_StyleMap[] = "StyleMap";
const char kmlTag_StyleSelector[] = "StyleSelector";
const char kmlTag_styleUrl[] = "styleUrl";
const char kmlTag_targetHref[] = "targetHref";
const char kmlTag_tessellate[] = "tessellate";
const char kmlTag_text[] = "text";
const char kmlTag_textColor[] = "textColor";
const char kmlTag_tileSize[] = "tileSize";
const char kmlTag_tilt[] = "tilt";
const char kmlTag_TimeSpan[] = "TimeSpan";
const char kmlTag_TimeStamp[] = "TimeStamp";
const char kmlTag_topFov[] = "topFov";
const char kmlTag_Update[] = "Update";
const char kmlTag_Url[] = "Url";
const char kmlTag_value[] = "value";
const char kmlTag_viewBoundScale[] = "viewBoundScale";
const char kmlTag_viewFormat[] = "viewFormat";
const char kmlTag_viewRefreshMode[] = "viewRefreshMode";
const char kmlTag_viewRefreshTime[] = "viewRefreshTime";
const char kmlTag_ViewVolume[] = "ViewVolume";
const char kmlTag_visibility[] = "visibility";
const char kmlTag_w[] = "w";
const char kmlTag_west[] = "west";
const char kmlTag_when[] = "when";
const char kmlTag_width[] = "width";
const char kmlTag_x[] = "x";
const char kmlTag_y[] = "y";
const char kmlTag_z[] = "z";

const char kmlTag_AnimatedUpdate[] = "AnimatedUpdate";
const char kmlTag_FlyTo[] = "FlyTo";
const char kmlTag_MultiTrack[] = "MultiTrack";
const char kmlTag_Track[] = "Track";
const char kmlTag_coord[] = "coord";
const char kmlTag_delayedStart[] = "delayedStart";
const char kmlTag_SoundCue[] = "SoundCue";
const char kmlTag_Tour [] = "Tour";
const char kmlTag_TourControl[] = "TourControl";
const char kmlTag_Wait[] = "Wait";
const char kmlTag_Playlist[] = "Playlist";
const char kmlTag_playMode[] = "playMode";
const char kmlTag_balloonVisibility[] = "balloonVisibility";

#ifdef KML_LAZY_IMP
 const char kmlTag_MarblePlacemark[] = "MarblePlacemark";
 const char kmlTag_area[] = "area";
 const char kmlTag_pop[] = "pop";
 const char kmlTag_role[] = "role";
 const char kmlTag_CountryNameCode[] = "CountryNameCode";
 const char kmlTag_countrycode[] = "countrycode";
#endif // KML_LAZY_IMP
}

}
