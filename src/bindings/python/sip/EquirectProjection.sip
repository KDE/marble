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

class EquirectProjection : Marble::AbstractProjection
{
%TypeHeaderCode
#include <marble/EquirectProjection.h>
%End

public:
    explicit                EquirectProjection ();
    Marble::AbstractProjectionHelper*  helper ();


//io ambiguous symbol: int& x -- specify In, Out or both
//io ambiguous symbol: int& y -- specify In, Out or both
//io     bool               screenCoordinates (const qreal lon, const qreal lat, const Marble::ViewportParams* params, int& x, int& y);




//io ambiguous symbol: int& x -- specify In, Out or both
//io ambiguous symbol: int& y -- specify In, Out or both
//io ambiguous symbol: bool& globeHidesPoint -- specify In, Out or both
//io     bool               screenCoordinates (const Marble::GeoDataCoordinates& geopoint, const Marble::ViewportParams* params, int& x, int& y, bool& globeHidesPoint);




//io ambiguous symbol: int* x -- specify In, Out or both
//io ambiguous symbol: int& y -- specify In, Out or both
//io ambiguous symbol: int& pointRepeatNum -- specify In, Out or both
//io ambiguous symbol: bool& globeHidesPoint -- specify In, Out or both
//io     bool               screenCoordinates (const Marble::GeoDataCoordinates& coordinates, const Marble::ViewportParams* viewport, int* x, int& y, int& pointRepeatNum, bool& globeHidesPoint);




//mt undefined symbol: QVector<QPolygonF*>& polygons -- need mapped type?
//mt     bool               screenCoordinates (const Marble::GeoDataLineString& lineString, const Marble::ViewportParams* viewport, QVector<QPolygonF*>& polygons);




//mt undefined symbol: Unit unit -- need mapped type?
//io ambiguous symbol: qreal& lon -- specify In, Out or both
//io ambiguous symbol: qreal& lat -- specify In, Out or both
//mt io     bool            geoCoordinates (int x, int y, const Marble::ViewportParams* params, qreal& lon, qreal& lat, Marble::GeoDataCoordinates::Unit unit = GeoDataCoordinates::Degree);


    Marble::GeoDataLatLonAltBox  latLonAltBox (const QRect& screenRect, const Marble::ViewportParams* viewport);
    bool                    mapCoversViewport (const Marble::ViewportParams* viewport) const;
};   // EquirectProjection

};   // Marble


