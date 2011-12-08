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
    void                    setRadius (int newRadius);
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
};
// ViewportParams

};
// Marble


