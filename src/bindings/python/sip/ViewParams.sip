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
//ig                            ViewParams ();
//ig    Marble::AbstractProjection*  currentProjection () const;

//FIXME implementation is missing.
//ig    bool                    showAtmosphere () const;
//ig    void                    setShowAtmosphere (bool);

//ig    ~ViewParams ();
//FIXME implementation is missing.
//ig    Marble::GeoSceneDocument*  mapTheme ();
private:
//force
                            ViewParams (const Marble::ViewParams&);
//end
public:
//ig    Marble::MapQuality      mapQuality () const;
//ig    bool                    showClouds () const;
//ig    void                    setShowClouds (const bool);
//ig    Marble::MapQuality      mapQuality (Marble::ViewContext viewContext) const;
//ig    void                    setMapQualityForViewContext (Marble::MapQuality quality, Marble::ViewContext viewContext);
//ig    Marble::ViewContext     viewContext () const;
//ig    void                    setViewContext (Marble::ViewContext viewContext);
//FIXME
//ig    QSharedPointer<QImage>  canvasImagePtr () const;
//ig    QSharedPointer<QImage>  coastImagePtr () const;
};
// ViewParams

};
// Marble


