#!/usr/bin/env python
#    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>
#
#    This file is part of the KDE project
#
#    This library is free software you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation either
#    version 2 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.
#
#    You should have received a copy of the GNU Library General Public License
#    aint with this library see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#    Boston, MA 02110-1301, USA.
#
# this script can be used to easily add new kml files
# use it in the form: add_kml_tag.py Tagname
# where Tagname is the name of the tag - be aware that the case does matter!!!

maintainer=True
import sys
import os

# the taglist lists all the tags of KML spec 2.1 with their respective parent tags
# please keep in mind that tags are case sensitive in XML

TAGLIST = """
address,Feature
altitude,Location,LookAt
altitudeMode,Geometry,LookAt
BalloonStyle,Style
begin,TimeSpan
bgColor,BalloonStyle,ListStyle
Change,Update
code,Status
color,ColorStyle
colorMode,ColorStyle
cookie,NetworkLinkControl
coordinates,Point
Create,Update
Delete,Update
description,Feature
Document,kml
drawOrder,Overlay
east,LatLonBox
end,TimeSpan
expires,NetworkLinkControl
extrude,Geometry
fill,PolyStyle
flyToView,NetworkLink
Folder,kml
GroundOverlay,Folder,Document
heading,Orientation,LookAt
href,Icon
hotSpot,IconStyle
httpQuery,Link
Icon,IconStyle,Overlay
IconStyle,Style
innerBoundaryIs,Polygon
ItemIcon,ListStyle
key,Pair
LabelStyle,Style
latitude,Location,LookAt
LatLonAltBox,Region
LatLonBox,GroundOverlay
LinearRing,Placemark,innerBoundaryIs,outerBoundaryIs,MultiGeometry
LineString,Placemark,MultiGeometry
LineStyle,Style
Link,Model,NetworkLink
linkDescription,NetworkLinkControl
linkName,NetworkLinkControl
linkSnippet,NetworkLinkControl
listItemType,ListStyle
ListStyle,Style
Location,Model
Lod,Region
longitude,Location,LookAt
LookAt,Feature
maxAltitude,LatLonAltBox
maxFadeExtent,Lod
maxLodPixels,Lod
message,NetworkLinkControl
Metadata,Feature
minAltitude,LatLonAltBox
minFadeExtent,Lod
minLodPixels,Lod
minRefreshPeriod,NetworkLinkControl
Model,Placemark,MultiGeometry
MultiGeometry,Placemark,MultiGeometry
name,Feature
NetworkLink,Container
NetworkLinkControl,kml
north,LatLonBox
ObjArrayField,Schema
ObjField,Schema
open,Feature
Orientation,Model
outerBoundaryIs,Polygon
outline,PolyStyle
overlayXY,ScreenOverlay
Pair,Stylemap
phoneNumber,Feature
Placemark,Container
Point,Placemark,MultiGeometry
Polygon,Placemark,MultiGeometry
PolyStyle,Style
range,LookAt
refreshInterval,Link
refreshMode,Link
refreshVisibility,NetworkLink
Region,Feature
request,Status
roll,Orientation
rotation,ScreenOverlay
rotationXY,ScreenOverlay
Scale,Scale
scale,LabelStyle
Schema,Document
ScreenOverlay,Container
screenXY,ScreenOverlay
SimpleArrayField,Schema
SimpleField,Schema
size,ScreenOverlay
Snippet,Feature
south,LatLonBox
state,ItemIcon
Style,Feature
StyleMap,Feature
styleUrl,Feature,Pair
targetHref,Update
tessellate,Geometry
text,BalloonStyle
textcolor,BalloonStyle
tilt,Orientation,LookAt
TimeSpan,Feature
TimeStamp,Feature
Update,NetworkLinkControl
viewBoundScale,Link
viewFormat,Link
viewRefreshMode,Link
viewRefreshTime,Link
visibility,Feature
west,LatLonBox
when,TimeStamp
width,LineStyle
x,Scale
y,Scale
z,Scale
"""

# the template header file
HEADER="""/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#ifndef KmltemplateTagHandler_h
#define KmltemplateTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{

class KmltemplateTagHandler : public GeoTagHandler {
public:
    virtual GeoNode* parse(GeoParser&) const;
};

}   // Marble

#endif // KmltemplateTagHandler_h
"""

# the template source file
SOURCE="""/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#include "KmltemplateTagHandler.h"

#include <QtCore/QDebug>

#include "KmlElementDictionary.h"

TEMPLATE_PARENT_HEADERS
#include "GeoDataParser.h"

namespace Marble
{

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( template )

GeoNode* KmltemplateTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_template ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.represents( kmlTag_Parent ) ) {
        QString content = parser.readElementText().trimmed();
        
        parentItem.nodeAs<GeoDataParent>()->doSomething( content );
    }

    return 0;
}

} // Marble
"""

path = os.getcwd() + os.sep

# all known abstract parents - those need a different handling
abstractParents=['Feature', 'Container', 'Geometry', 'ColorStyle', 'StyleSelector', 'SchemaField', 'Overlay', 'TimePrimitive']

def add_new_copy( tagname, parents ):
    """ add new source files for Tag tagname """
# set pretend to True to not actually generate the source files
    pretend = False
    if not ( os.path.exists( path + "Kml" + tagname.capitalize() + "TagHandler.h" ) or os.path.exists( path + "Kml" + tagname.capitalize() + "TagHandler.cpp" ) ):
        if not pretend:
            file( path + "Kml" + capitalize( tagname ) + "TagHandler.h", "wb" ).write( HEADER )
            file( path + "Kml" + capitalize( tagname ) + "TagHandler.cpp", "wb" ).write( SOURCE )
        else:
            print "writing " + path + "Kml" + capitalize( tagname ) + "TagHandler.h"
            print "writing " + path + "Kml" + capitalize( tagname ) + "TagHandler.cpp"

        HEADERS = ""
# generate a list of the include headers according to the parent tags (which are normally equivalent to a GeoDataObject header)
        if tagname in parents.keys():
            for i in parents[ tagname ]:
                if i in abstractParents:
                    print "abstract:", i
                else:
                    HEADERS += "#include \\\"GeoData" + i + "\\.h\\\"\\n"
            firstParent = parents[ tagname ][ 0 ]
        else:
            firstParent = "Parent"
                
        sedHeaderCommand = "sed -i -e \"s/KmltemplateTagHandler/Kml" + tagname + "TagHandler/g\" " \
                           + path + "Kml" + capitalize( tagname ) + "TagHandler.h"

        sedCPPCommand = "sed -i -e \"s/KmltemplateTagHandler/Kml" + tagname + "TagHandler/g\" " \
                        + "-e \"s/kmlTag_template/kmlTag_" + tagname +"/g\" " \
                        + "-e \"s/kmlTag_Parent/kmlTag_" + firstParent +"/g\" " \
                        + "-e \"s/KML_DEFINE_TAG_HANDLER( template )/KML_DEFINE_TAG_HANDLER( " + tagname +" )/g\" " \
                        + "-e \"s/TEMPLATE_PARENT_HEADERS/" + HEADERS + "/g\" " \
                        + "-e \"s/KmltemplateTagHandler.h/Kml" + capitalize( tagname ) +"TagHandler.h/g\" " \
                        + path + "Kml" + capitalize( tagname ) + "TagHandler.cpp"
        #print sedHeaderCommand
        print sedCPPCommand
        if not pretend:
            os.system( sedHeaderCommand )
            os.system( sedCPPCommand )
    else:
        print "file already existing: ", tagname

def capitalize( tagname ):
    """ because the capitalize function does not work correctly for CamelCase """
    return tagname[0].capitalize() + tagname[1:]

def usage( name ):
    print
    print name + " [Tag1 [...]]"
    print
    print "generate new source file for any kml tag"
    print "source files will be added in the directory where you execute"
    print "this script"
    

parents = dict()
for line in TAGLIST.split():
    """make up a dict of all tags parents"""
    tmp = line.split(',')
    tagname = tmp[ 0 ]
    parents[ tagname ] = tmp[ 1: ]
    
if len(sys.argv) > 1:
    if sys.argv[ 1 ] in ['-h', '--help']:
        usage( sys.argv[ 0 ] )
        sys.exit( 0 )
        
    newTags = sys.argv[ 1: ]
    
    for tag in newTags:
        """ check if parents of to-be-generated are available """
        if maintainer==False:
            try:
                p = parents[ tag ]
            except:
                print "error: no such tag " + tag + "!"
                continue
        add_new_copy( tag, parents )
