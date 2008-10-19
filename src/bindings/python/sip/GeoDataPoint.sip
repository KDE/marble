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

class GeoDataPoint : Marble::GeoDataCoordinates, Marble::GeoDataGeometry
{
%TypeHeaderCode
#include <marble/GeoDataPoint.h>
%End

public:
    typedef Marble::GeoDataCoordinates::Notation Notation;

    typedef Marble::GeoDataCoordinates::Unit Unit;

                            GeoDataPoint (const Marble::GeoDataPoint& other);
    explicit                GeoDataPoint (Marble::GeoDataObject* parent = 0);

                            GeoDataPoint (qreal _lon, qreal _lat, qreal alt = 0, Marble::GeoDataPoint::Unit _unit = Marble::GeoDataPoint::Radian, int _detail = 0, Marble::GeoDataObject* parent = 0);

//ig                            GeoDataPoint (qreal _lon, qreal _lat, qreal alt = 0, Marble::GeoDataObject* parent = 0);
    virtual EnumGeometryId  geometryId () const;
    typedef QVector<Marble::GeoDataPoint> Vector;

    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
};   // GeoDataPoint

};   // Marble


