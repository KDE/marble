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

class GeoDataGeometry : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataGeometry.h>
%End


public:
                            GeoDataGeometry ();
                            GeoDataGeometry (const Marble::GeoDataGeometry& other);
                            GeoDataGeometry (const Marble::GeoDataPolygon& other);
                            GeoDataGeometry (const Marble::GeoDataPoint& other);
                            GeoDataGeometry (const Marble::GeoDataLineString& other);
                            GeoDataGeometry (const Marble::GeoDataMultiGeometry& other);
    bool                    operator == (const Marble::GeoDataGeometry& other) const;
    virtual EnumGeometryId  geometryId () const;
    bool                    extrude () const;
    void                    setExtrude (bool extrude);
    Marble::AltitudeMode    altitudeMode () const;
    void                    setAltitudeMode (const Marble::AltitudeMode altitudeMode);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    void                    detach ();

    virtual ~GeoDataGeometry ();
    virtual QString         nodeType () const;
private:
//ig                            GeoDataGeometry (GeoDataGeometryPrivate* priv);
public:
    virtual Marble::GeoDataLatLonAltBox  latLonAltBox () const;
};
// GeoDataGeometry

};
// Marble


