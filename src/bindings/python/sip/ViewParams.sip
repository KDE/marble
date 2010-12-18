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

class ViewParams
{
%TypeHeaderCode
#include <ViewParams.h>
%End

public:
                            ViewParams ();
    Marble::ViewportParams*  viewport ();
    void                    setMapThemeId (const QString&);
    Marble::Projection      projection () const;
//ig    Marble::AbstractProjection*  currentProjection () const;
    void                    setProjection (Marble::Projection newProjection);
    void                    setMapQuality (Marble::MapQuality);
    void                    setPropertyValue (const QString& name, bool value);
    void                    propertyAvailable (const QString& name, bool& available /out/);
    void                    propertyValue (const QString& name, bool& value /out/);
    int                     radius () const;
    void                    setRadius (int newRadius);
    Marble::Quaternion      planetAxis () const;
    void                    setPlanetAxis (const Marble::Quaternion& newAxis);
    void                    centerCoordinates (qreal& centerLon /out/, qreal& centerLat /out/);
    QImage*                 canvasImage () const;
    QImage*                 coastImage () const;

// FIXME const here causes trouble
//    void                    setCoastImage (QImage *const);

    bool                    showGps () const;
    void                    setShowGps (bool);
    bool                    showElevationModel () const;
    void                    setShowElevationModel (bool);
    bool                    showAtmosphere () const;
    void                    setShowAtmosphere (bool);

    ~ViewParams ();
//FIXME implementation is missing.
//ig    Marble::GeoSceneDocument*  mapTheme ();
private:
//force
                            ViewParams (const Marble::ViewParams&);
//end
public:
    Marble::MapQuality      mapQuality () const;
    int                     width () const;
    int                     height () const;
    bool                    showClouds () const;
    void                    setShowClouds (const bool);
    void                    setSize (int width, int height);
};
// ViewParams

};
// Marble


