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

class GeoDataLatLonAltBox : Marble::GeoDataLatLonBox
{
%TypeHeaderCode
#include <GeoDataLatLonAltBox.h>
%End


public:
                            GeoDataLatLonAltBox ();
                            GeoDataLatLonAltBox (const Marble::GeoDataLatLonAltBox& other);
                            GeoDataLatLonAltBox (const Marble::GeoDataLatLonBox& other);
    qreal                   minAltitude () const;
    void                    setMinAltitude (const qreal minAltitude);
    qreal                   maxAltitude () const;
    void                    setMaxAltitude (const qreal maxAltitude);
    Marble::AltitudeMode    altitudeMode () const;
    void                    setAltitudeMode (const Marble::AltitudeMode altitudeMode);
    virtual bool            contains (const Marble::GeoDataCoordinates&) const;
    bool                    contains (const Marble::GeoDataLatLonAltBox&) const;
    virtual bool            intersects (const Marble::GeoDataLatLonAltBox&) const;
    static Marble::GeoDataLatLonAltBox  fromLineString (const Marble::GeoDataLineString& lineString);
    virtual QString         toString (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    bool                    isNull () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
                            GeoDataLatLonAltBox (const Marble::GeoDataCoordinates& coordinates);
    virtual ~GeoDataLatLonAltBox ();
    virtual const char*     nodeType () const;
    virtual Marble::GeoDataCoordinates  center () const;
    virtual void            clear ();
};
// GeoDataLatLonAltBox

//ig bool                    operator == (const Marble::GeoDataLatLonAltBox& lhs, const Marble::GeoDataLatLonAltBox& rhs);
};
// Marble


