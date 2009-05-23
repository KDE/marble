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
class GeoDataLineString : /* QVector<Marble::GeoDataCoordinates*>, */ Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <GeoDataLineString.h>
%End

public:
                            GeoDataLineString (const Marble::GeoDataLineString&);
    virtual bool            isClosed () const;
    bool                    tessellate () const;
    void                    setTessellate (bool tessellate);
    Marble::TessellationFlags  tessellationFlags () const;
    void                    setTessellationFlags (Marble::TessellationFlags f);
    void                    clear ();

    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual EnumGeometryId  geometryId () const;

};   // GeoDataLineString

};   // Marble
