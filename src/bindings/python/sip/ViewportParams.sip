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

class ViewportParams
{
%TypeHeaderCode
#include <ViewportParams.h>
%End

public:
                            ViewportParams ();
    Marble::Projection      projection () const;
//ig    Marble::AbstractProjection*  currentProjection () const;
    void                    setProjection (Marble::Projection newProjection);
    int                     polarity () const;
    Marble::GeoDataLatLonAltBox  viewLatLonAltBox () const;
    qreal                   angularResolution () const;
    bool                    resolves (const Marble::GeoDataLatLonBox& latLonBox) const;
    bool                    resolves (const Marble::GeoDataCoordinates& coord1, const Marble::GeoDataCoordinates& coord2) const;
    int                     radius () const;
    void                    setRadius (int radius);
    bool                    globeCoversViewport () const;
    Marble::Quaternion      planetAxis () const;
    int                     width () const;
    int                     height () const;
    QSize                   size () const;
    void                    setWidth (int newWidth);
    void                    setHeight (int newHeight);
    void                    setSize (QSize newSize);
    bool                    mapCoversViewport () const;

    ~ViewportParams ();
    bool                    resolves (const Marble::GeoDataLatLonAltBox& latLonAltBox) const;
//ig    Marble::matrix*         planetAxisMatrix () const;
private:
//force
                            ViewportParams (const Marble::ViewportParams&);
//end
public:
    Marble::GeoDataCoordinates  focusPoint () const;
    void                    setFocusPoint (const Marble::GeoDataCoordinates& focusPoint);
    void                    resetFocusPoint ();
    void                    centerOn (qreal lon, qreal lat);
    qreal                   centerLongitude () const;
    qreal                   centerLatitude () const;
    Marble::GeoDataLatLonAltBox  latLonAltBox (const QRect& screenRect) const;
    void                    centerCoordinates (qreal& centerLon, qreal& centerLat) const;
    bool                    screenCoordinates (const qreal lon, const qreal lat, qreal& x, qreal& y) const;
    bool                    screenCoordinates (const Marble::GeoDataCoordinates& geopoint, qreal& x, qreal& y, bool& globeHidesPoint) const;
//    bool                    screenCoordinates (const Marble::GeoDataCoordinates& geopoint, qreal& x, qreal& y) const;
    bool                    screenCoordinates (const Marble::GeoDataCoordinates& geopoint, QPointF& screenpoint) const;
//    bool                    screenCoordinates (const Marble::GeoDataCoordinates& coordinates, qreal* x, qreal& y, int& pointRepeatNum, bool& globeHidesPoint) const;
//    bool                    screenCoordinates (const Marble::GeoDataCoordinates& coordinates, qreal* x, qreal& y, int& pointRepeatNum, const QSizeF& size, bool& globeHidesPoint) const;
//    bool                    screenCoordinates (const Marble::GeoDataLineString& lineString, QVector<QPolygonF*>& polygons) const;
    bool                    geoCoordinates (const int x, const int y, qreal& lon, qreal& lat, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Degree) const;
    QPainterPath            mapShape () const;
    QRegion                 mapRegion () const;

//ig    bool                    screenCoordinates (const Marble::GeoDataCoordinates& geopoint, qreal& x, qreal& y) const;
//ig    bool                    screenCoordinates (const Marble::GeoDataCoordinates& coordinates, qreal* x, qreal& y, int& pointRepeatNum, bool& globeHidesPoint) const;
    bool                    screenCoordinates (const Marble::GeoDataCoordinates& coordinates, qreal* x, qreal& y, int& pointRepeatNum, const QSizeF& size, bool& globeHidesPoint) const;
//ig    bool                    screenCoordinates (const Marble::GeoDataLineString& lineString, QVector<QPolygonF*>& polygons) const;
};
// ViewportParams

};
// Marble


