//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class GeoDocument
{
%TypeHeaderCode
#include <GeoDocument.h>
%End

protected:
                            GeoDocument ();

public:
    virtual bool            isGeoDataDocument () const;
    virtual bool            isGeoSceneDocument () const;
    virtual ~GeoDocument ();
};
// GeoDocument


class GeoNode
{
%TypeHeaderCode
#include <GeoDocument.h>
%End

protected:
                            GeoNode ();
    virtual ~GeoNode ();
%ConvertToSubClassCode
    // CTSCC for subclasses of 'GeoNode'
    sipType = NULL;

    if (dynamic_cast<Marble::GeoDataObject*>(sipCpp))
        {
        sipType = sipType_Marble_GeoDataObject;
        if (dynamic_cast<Marble::GeoDataAbstractView*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataAbstractView;
            if (dynamic_cast<Marble::GeoDataLookAt*>(sipCpp))
                sipType = sipType_Marble_GeoDataLookAt;
            }
        else if (dynamic_cast<Marble::GeoDataColorStyle*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataColorStyle;
            if (dynamic_cast<Marble::GeoDataIconStyle*>(sipCpp))
                sipType = sipType_Marble_GeoDataIconStyle;
            else if (dynamic_cast<Marble::GeoDataLabelStyle*>(sipCpp))
                sipType = sipType_Marble_GeoDataLabelStyle;
            else if (dynamic_cast<Marble::GeoDataLineStyle*>(sipCpp))
                sipType = sipType_Marble_GeoDataLineStyle;
            else if (dynamic_cast<Marble::GeoDataPolyStyle*>(sipCpp))
                sipType = sipType_Marble_GeoDataPolyStyle;
            }
        else if (dynamic_cast<Marble::GeoDataFeature*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataFeature;
            if (dynamic_cast<Marble::GeoDataContainer*>(sipCpp))
                {
                sipType = sipType_Marble_GeoDataContainer;
                if (dynamic_cast<Marble::GeoDataDocument*>(sipCpp))
                    sipType = sipType_Marble_GeoDataDocument;
                else if (dynamic_cast<Marble::GeoDataFolder*>(sipCpp))
                    sipType = sipType_Marble_GeoDataFolder;
                }
            else if (dynamic_cast<Marble::GeoDataOverlay*>(sipCpp))
                sipType = sipType_Marble_GeoDataOverlay;
            else if (dynamic_cast<Marble::GeoDataPlacemark*>(sipCpp))
                sipType = sipType_Marble_GeoDataPlacemark;
            }
        else if (dynamic_cast<Marble::GeoDataGeometry*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataGeometry;
            if (dynamic_cast<Marble::GeoDataLineString*>(sipCpp))
                {
                sipType = sipType_Marble_GeoDataLineString;
                if (dynamic_cast<Marble::GeoDataLinearRing*>(sipCpp))
                    sipType = sipType_Marble_GeoDataLinearRing;
                }
            else if (dynamic_cast<Marble::GeoDataMultiGeometry*>(sipCpp))
                sipType = sipType_Marble_GeoDataMultiGeometry;
            else if (dynamic_cast<Marble::GeoDataPoint*>(sipCpp))
                sipType = sipType_Marble_GeoDataPoint;
            else if (dynamic_cast<Marble::GeoDataPolygon*>(sipCpp))
                {
                sipType = sipType_Marble_GeoDataPolygon;
                if (dynamic_cast<Marble::GeoDataInnerBoundary*>(sipCpp))
                    sipType = sipType_Marble_GeoDataInnerBoundary;
                else if (dynamic_cast<Marble::GeoDataOuterBoundary*>(sipCpp))
                    sipType = sipType_Marble_GeoDataOuterBoundary;
                }
            else if (dynamic_cast<Marble::GeoDataTrack*>(sipCpp))
                sipType = sipType_Marble_GeoDataTrack;
            }
        else if (dynamic_cast<Marble::GeoDataHotSpot*>(sipCpp))
            sipType = sipType_Marble_GeoDataHotSpot;
        else if (dynamic_cast<Marble::GeoDataLatLonBox*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataLatLonBox;
            if (dynamic_cast<Marble::GeoDataLatLonAltBox*>(sipCpp))
                sipType = sipType_Marble_GeoDataLatLonAltBox;
            }
        else if (dynamic_cast<Marble::GeoDataLod*>(sipCpp))
            sipType = sipType_Marble_GeoDataLod;
        else if (dynamic_cast<Marble::GeoDataRegion*>(sipCpp))
            sipType = sipType_Marble_GeoDataRegion;
        else if (dynamic_cast<Marble::GeoDataStyleSelector*>(sipCpp))
            {
            sipType = sipType_Marble_GeoDataStyleSelector;
            if (dynamic_cast<Marble::GeoDataStyle*>(sipCpp))
                sipType = sipType_Marble_GeoDataStyle;
            else if (dynamic_cast<Marble::GeoDataStyleMap*>(sipCpp))
                sipType = sipType_Marble_GeoDataStyleMap;
            }
        }
%End
public:
    virtual const char*     nodeType () const;
};
// GeoNode

//ig namespace GeoDataTypes;
};
// Marble


%ModuleHeaderCode
//ctscc
#include <GeoDataAbstractView.h>
#include <GeoDataColorStyle.h>
#include <GeoDataContainer.h>
#include <GeoDataDocument.h>
#include <GeoDataFeature.h>
#include <GeoDataFolder.h>
#include <GeoDataGeometry.h>
#include <GeoDataHotSpot.h>
#include <GeoDataIconStyle.h>
#include <GeoDataLabelStyle.h>
#include <GeoDataLatLonAltBox.h>
#include <GeoDataLatLonBox.h>
#include <GeoDataLineString.h>
#include <GeoDataLineStyle.h>
#include <GeoDataLinearRing.h>
#include <GeoDataLod.h>
#include <GeoDataLookAt.h>
#include <GeoDataMultiGeometry.h>
#include <GeoDataObject.h>
#include <GeoDataOverlay.h>
#include <GeoDataPlacemark.h>
#include <GeoDataPoint.h>
#include <GeoDataPolyStyle.h>
#include <GeoDataPolygon.h>
#include <GeoDataRegion.h>
#include <GeoDataStyle.h>
#include <GeoDataStyleMap.h>
#include <GeoDataStyleSelector.h>
#include <GeoDataTrack.h>
#include <GeoDocument.h>
%End
