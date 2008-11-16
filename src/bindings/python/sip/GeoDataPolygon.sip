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

class GeoDataPolygon : Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataPolygon.h>
%End

public:
    explicit                GeoDataPolygon (Marble::GeoDataObject* parent = 0, Marble::TessellationFlags f = Marble::NoTessellation);
                            GeoDataPolygon (const Marble::GeoDataPolygon&);
    virtual bool            isClosed () const;
    bool                    tessellate () const;
    void                    setTessellate (bool tessellate);
    Marble::TessellationFlags  tessellationFlags () const;
    void                    setTessellationFlags (Marble::TessellationFlags f);
//FIXME    Marble::GeoDataLatLonAltBox  latLonAltBox () const;
//FIXME    Marble::GeoDataLinearRing&  outerBoundary () const;
//FIXME    void                    setOuterBoundary (Marble::GeoDataLinearRing* boundary);


//mt undefined symbol: QVector<Marble::GeoDataLinearRing*> None -- need mapped type?
//mt     QVector<Marble::GeoDataLinearRing*>  innerBoundaries () const;


//FIXME    void                    appendInnerBoundary (Marble::GeoDataLinearRing* boundary);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual EnumGeometryId  geometryId () const;
};   // GeoDataPolygon


class GeoDataOuterBoundary : Marble::GeoDataPolygon
{
%TypeHeaderCode
#include <GeoDataPolygon.h>
%End
};   // GeoDataOuterBoundary


class GeoDataInnerBoundary : Marble::GeoDataPolygon
{
%TypeHeaderCode
#include <GeoDataPolygon.h>
%End
};   // GeoDataInnerBoundary

};   // Marble


