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
    Marble::AbstractProjection*  currentProjection () const;
    void                    setProjection (Marble::Projection newProjection);
    int                     polarity () const;
    Marble::GeoDataLatLonAltBox  viewLatLonAltBox () const;
    int                     radius () const;
    void                    setRadius (int newRadius);
    bool                    globeCoversViewport () const;
    Marble::Quaternion      planetAxis () const;
    bool                    setPlanetAxis (const Marble::Quaternion& newAxis);
//FIXME    Marble::matrix*         planetAxisMatrix () const;
    int                     width () const;
    int                     height () const;
    QSize                   size () const;
    void                    setWidth (int newWidth);
    void                    setHeight (int newHeight);
    void                    setSize (QSize newSize);

//mt undefined symbol: BoundingBox None -- need mapped type?
//mt     BoundingBox        boundingBox () const;

//mt undefined symbol: BoundingBox& None -- need mapped type?
//mt     void               setBoundingBox (const BoundingBox&);

    void                    centerCoordinates (qreal& centerLon /out/, qreal& centerLat /out/) const;
    bool                    mapCoversViewport () const;

private:
                            ViewportParams (const Marble::ViewportParams&);
};   // ViewportParams

};   // Marble


