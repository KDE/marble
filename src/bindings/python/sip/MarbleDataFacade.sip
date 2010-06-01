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

class MarbleDataFacade /Abstract/
{
%TypeHeaderCode
#include <MarbleDataFacade.h>
%End



public:
    explicit                MarbleDataFacade (Marble::MarbleModel* model);
    qreal                   planetRadius () const;
    const Marble::Planet*   planet () const;
    QDateTime               dateTime () const;
    QString                 target () const;
    ~MarbleDataFacade ();
//ig    MarbleGeoDataModel*     geoDataModel ();
//ig    MarbleGeometryModel*    geometryModel ();
//ig    MarblePlacemarkModel*   placemarkModel ();
//ig    FileViewModel*          fileViewModel () const;
    Marble::PluginManager*          pluginManager ();
};
// MarbleDataFacade

};
// Marble


