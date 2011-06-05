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

class GeoDataLatLonBox : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataLatLonBox.h>
%End


public:
                            GeoDataLatLonBox ();
                            GeoDataLatLonBox (qreal north, qreal south, qreal east, qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
                            GeoDataLatLonBox (const Marble::GeoDataLatLonBox&);
    qreal                   north (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setNorth (const qreal north, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   south (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setSouth (const qreal south, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   east (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setEast (const qreal east, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   west (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setWest (const qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   rotation (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setRotation (const qreal rotation, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    void                    setBoundaries (qreal north, qreal south, qreal east, qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   width (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    qreal                   height (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    bool                    crossesDateLine () const;
    virtual bool            contains (const Marble::GeoDataPoint&) const;
    virtual bool            contains (const Marble::GeoDataCoordinates&) const;
    bool                    contains (const Marble::GeoDataLatLonBox&) const;
    virtual bool            intersects (const Marble::GeoDataLatLonBox&) const;

//mt undefined symbol: QVector<Marble::GeoDataCoordinates*>& coordinates -- need mapped type?
//mt     static Marble::GeoDataLatLonBox  fromCoordinates (const QVector<Marble::GeoDataCoordinates*>& coordinates);

    static Marble::GeoDataLatLonBox  fromLineString (const Marble::GeoDataLineString& lineString);
    virtual QString         toString (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    virtual bool            isNull () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual ~GeoDataLatLonBox ();
    virtual const char*     nodeType () const;
    virtual Marble::GeoDataCoordinates  center () const;
    bool                    containsPole (Marble::Pole pole = Marble::AnyPole) const;
    Marble::GeoDataLatLonBox  united (const Marble::GeoDataLatLonBox& other) const;
    virtual bool            isEmpty () const;
    Marble::GeoDataLatLonBox  operator | (const Marble::GeoDataLatLonBox& other) const;
    Marble::GeoDataLatLonBox&  operator |= (const Marble::GeoDataLatLonBox& other);
    void                    boundaries (qreal& north, qreal& south, qreal& east, qreal& west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
};
// GeoDataLatLonBox

};
// Marble


