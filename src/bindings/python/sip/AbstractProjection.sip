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

    enum SurfaceType
    {
        Cylindrical,
        Pseudocylindrical,
        Hybrid,
        Conical,
        Pseudoconical,
        Azimuthal
    };


    enum PreservationType
    {
        NoPreservation,
        Conformal,
        EqualArea
    };

                            AbstractProjection ();
    virtual qreal           maxLat () const;
    virtual qreal           minLat () const;
    virtual bool            repeatX () const;
    virtual void            setRepeatX (bool val);
    virtual bool            traversablePoles () const;
    virtual bool            traversableDateLine () const;
    virtual Marble::AbstractProjection::SurfaceType  surfaceType () const = 0;
    virtual Marble::AbstractProjection::PreservationType  preservationType () const;
    virtual bool            isOrientedNormal () const;

//ig    virtual bool            screenCoordinates (const qreal lon, const qreal lat, const Marble::ViewportParams* viewport, qreal& x /out/, qreal& y /out/) = 0;
//ig    virtual bool            screenCoordinates (const Marble::GeoDataCoordinates& geopoint, const Marble::ViewportParams* viewport, qreal& x /out/, qreal& y /out/, bool& globeHidesPoint /out/) = 0;
//ig    bool            screenCoordinates (const Marble::GeoDataCoordinates& geopoint, const Marble::ViewportParams* viewport, qreal& x /out/, qreal& y /out/);

//io ambiguous symbol: qreal* x -- specify In, Out or both
//io ambiguous symbol: qreal& y -- specify In, Out or both
//io ambiguous symbol: int& pointRepeatNum -- specify In, Out or both
//io ambiguous symbol: bool& globeHidesPoint -- specify In, Out or both
//io     bool               screenCoordinates (const Marble::GeoDataCoordinates& coordinates, const Marble::ViewportParams* viewport, qreal* x, qreal& y, int& pointRepeatNum, bool& globeHidesPoint);

//io ambiguous symbol: qreal* x -- specify In, Out or both
//io ambiguous symbol: qreal& y -- specify In, Out or both
//io ambiguous symbol: int& pointRepeatNum -- specify In, Out or both
//io ambiguous symbol: bool& globeHidesPoint -- specify In, Out or both
//io     virtual bool       screenCoordinates (const Marble::GeoDataCoordinates& coordinates, const Marble::ViewportParams* viewport, qreal* x, qreal& y, int& pointRepeatNum, const QSizeF& size, bool& globeHidesPoint) = 0;



//io ambiguous symbol: int& x -- specify In, Out or both
//io ambiguous symbol: int& y -- specify In, Out or both
//io     bool               screenCoordinates (qreal lon, qreal lat, const Marble::ViewportParams* viewport, int& x, int& y);

//mt undefined symbol: QVector<QPolygonF*>& polygons -- need mapped type?
//mt    virtual bool       screenCoordinates (const Marble::GeoDataLineString& lineString, const Marble::ViewportParams* viewport, QVector<QPolygonF*>& polygons) = 0;

    virtual bool    geoCoordinates (int x, int y, const Marble::ViewportParams* viewport, qreal& lon /out/, qreal& lat /out/, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Degree) = 0;

// FIXME missing implementation?
//ig    virtual Marble::GeoDataLatLonAltBox  latLonAltBox (const QRect& screenRect, const Marble::ViewportParams* viewport);
//ig    bool                    exceedsLatitudeRange (const Marble::GeoDataCoordinates& coords) const;
//ig    bool                    exceedsLatitudeRange (const Marble::GeoDataLineString& lineString) const;

    virtual bool            mapCoversViewport (const Marble::ViewportParams* viewport) const = 0;
    virtual QPainterPath    mapShape (const Marble::ViewportParams* viewport) const = 0;

// FIXME missing implementation?
//ig    QRegion                 mapRegion (const Marble::ViewportParams* viewport) const;

protected:
    virtual void            tessellateHorizon (const Marble::GeoDataCoordinates& previousCoords, const Marble::GeoDataCoordinates& currentCoords, const Marble::ViewportParams* viewport);

//mt undefined symbol: QVector<QPolygonF*>& polygons -- need mapped type?
//mt     bool               lineStringToPolygon (const Marble::GeoDataLineString& lineString, const Marble::ViewportParams* viewport, QVector<QPolygonF*>& polygons);

    QPolygonF               tessellateLineSegment (const Marble::GeoDataCoordinates& previousCoords, const Marble::GeoDataCoordinates& currentCoords, int count, const Marble::ViewportParams* viewport, Marble::TessellationFlags f = 0);
};   // AbstractProjection

};   // Marble


