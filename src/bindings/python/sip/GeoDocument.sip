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

class GeoDocument
{
%TypeHeaderCode
#include <GeoDocument.h>
%End

protected:
                            GeoDocument ();

public:
    unsigned long s_leakProtector;
    virtual bool            isGeoDataDocument () const;
    virtual bool            isGeoSceneDocument () const;
};   // GeoDocument


class GeoNode
{
%TypeHeaderCode
#include <GeoDocument.h>
%End

protected:
                            GeoNode ();
    virtual                 ~GeoNode ();
};   // GeoNode

};   // Marble


