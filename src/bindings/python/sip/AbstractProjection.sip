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

class AbstractProjection /Abstract/
{
%TypeHeaderCode
#include <AbstractProjection.h>
%End

public:
    explicit                AbstractProjection ();
    virtual qreal           maxLat () const;
    virtual qreal           minLat () const;
    virtual bool            traversableMaxLat () const;
    virtual bool            repeatX () const;
    virtual void            setRepeatX (bool val);
//ig    virtual Marble::AbstractProjectionHelper*  helper ();

//ig    virtual bool            screenCoordinates (const qreal lon, const qreal lat, const Marble::ViewportParams* viewport, int& x /out/, int& y /out/) = 0;
//ig    virtual bool            screenCoordinates (const Marble::GeoDataCoordinates& geopoint, const Marble::ViewportParams* viewport, int& x /out/, int& y /out/, bool& globeHidesPoint /out/) = 0;
//ig    bool                    screenCoordinates (const Marble::GeoDataCoordinates& geopoint, const Marble::ViewportParams* viewport, int& x /out/, int& y /out/);

    virtual bool            screenCoordinates (const Marble::GeoDataCoordinates& coordinates, const Marble::ViewportParams* viewport, int* x /out/, int& y /out/, int& pointRepeatNum /out/, bool& globeHidesPoint /out/) = 0;

//mt undefined symbol: QVector<QPolygonF*>& polygons -- need mapped type?
//mt    virtual bool       screenCoordinates (const Marble::GeoDataLineString& lineString, const Marble::ViewportParams* viewport, QVector<QPolygonF*>& polygons) = 0;

    virtual bool    geoCoordinates (int x, int y, const Marble::ViewportParams* viewport, qreal& lon /out/, qreal& lat /out/, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Degree) = 0;

//FIXME export needed.
//    virtual Marble::GeoDataLatLonAltBox  latLonAltBox (const QRect& screenRect, const Marble::ViewportParams* viewport);

//FIXME    virtual Marble::GeoDataLinearRing  rectOutline (const QRect& screenRect, const Marble::ViewportParams* viewport);
    virtual bool            mapCoversViewport (const Marble::ViewportParams* viewport) const = 0;

protected:
    void                    coordinateExtremes (qreal lon, qreal lat, qreal& westLon /out/, qreal& eastLon /out/, qreal& otherWestLon /out/, qreal& otherEastLon /out/, qreal& northLat /out/, qreal& southLat /out/);

//FIXME    QPolygonF               tessellateLineSegment (const Marble::GeoDataCoordinates& previousCoords, const Marble::GeoDataCoordinates& currentCoords, int count, const Marble::ViewportParams* viewport, Marble::TessellationFlags f = 0);
};   // AbstractProjection

};   // Marble


