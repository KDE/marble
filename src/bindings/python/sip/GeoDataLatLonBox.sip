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
#include <marble/GeoDataLatLonBox.h>
%End

public:
                            GeoDataLatLonBox ();

                            GeoDataLatLonBox (qreal north, qreal south, qreal east, qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

                            GeoDataLatLonBox (const Marble::GeoDataLatLonBox&);
    virtual bool            isFolder () const;
    qreal                   north (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setNorth (const qreal north, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

    qreal                   south (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;

    void                    setSouth (const qreal south, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

    qreal                   east (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;

    void                    setEast (const qreal east, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

    qreal                   west (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;

     void                   setWest (const qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

     qreal                  rotation (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;

     void                   setRotation (const qreal rotation, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

     void                   boundaries (qreal& north /out/, qreal& south /out/, qreal& east /out/, qreal& west /out/, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

     void                   setBoundaries (qreal north, qreal south, qreal east, qreal west, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);

    bool                    crossesDateLine () const;
    virtual bool            contains (const Marble::GeoDataPoint&);
    virtual bool            contains (const Marble::GeoDataCoordinates&);
    bool                    contains (const Marble::GeoDataLatLonBox&);
    virtual bool            intersects (const Marble::GeoDataLatLonBox&);


//mt undefined symbol: QVector<Marble::GeoDataCoordinates*>& coordinates -- need mapped type?
//mt     static Marble::GeoDataLatLonBox  fromCoordinates (const QVector<Marble::GeoDataCoordinates*>& coordinates);


//FIXME    static Marble::GeoDataLatLonBox  fromLineString (const Marble::GeoDataLineString& lineString);

    virtual QString         toString (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;

    virtual bool            isNull () const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
};   // GeoDataLatLonBox

};   // Marble


