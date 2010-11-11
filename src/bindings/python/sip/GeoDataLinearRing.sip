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

class GeoDataLinearRing : Marble::GeoDataLineString
{
%TypeHeaderCode
#include <GeoDataLinearRing.h>
%End

public:
    explicit                GeoDataLinearRing (Marble::TessellationFlags f = Marble::NoTessellation);
    virtual ~GeoDataLinearRing ();
    virtual bool            isClosed () const;
                            GeoDataLinearRing (const Marble::GeoDataGeometry& other);
    virtual QString         nodeType () const;
    virtual qreal           length (qreal planetRadius, int offset = 0) const;
//ig    virtual QVector<Marble::GeoDataLineString*>  toRangeCorrected () const;
    virtual bool            contains (const Marble::GeoDataCoordinates& coordinates) const;
};
// GeoDataLinearRing

};
// Marble
