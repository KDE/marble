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

class GeoDataCoordinates
{
%TypeHeaderCode
#include <GeoDataCoordinates.h>
%End



public:
    enum Unit
    {
        Radian,
        Degree
    };


    enum Notation
    {
        Decimal,
        DMS
    };

    typedef QVector<Marble::GeoDataCoordinates> Vector;
    typedef QVector<Marble::GeoDataCoordinates*> PtrVector;

                            GeoDataCoordinates (const Marble::GeoDataCoordinates& other);
                            GeoDataCoordinates ();
                            GeoDataCoordinates (qreal lon, qreal lat, qreal alt = 0, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian, int detail = 0);
    void                    set (qreal lon, qreal lat, qreal alt = 0, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    void                    geoCoordinates (qreal& lon /out/, qreal& lat /out/, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setLongitude (qreal lon, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   longitude (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    qreal                   latitude (Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian) const;
    void                    setLatitude (qreal lat, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian);
    qreal                   altitude () const;
    void                    setAltitude (const qreal altitude);
    int                     detail () const;
    void                    setDetail (const int det);
    const Marble::Quaternion&  quaternion () const;
    static Marble::GeoDataCoordinates::Notation  defaultNotation ();
    static void             setDefaultNotation (Marble::GeoDataCoordinates::Notation notation);
    static qreal            normalizeLon (qreal lon, Marble::GeoDataCoordinates::Unit = Marble::GeoDataCoordinates::Radian);
    static qreal            normalizeLat (qreal lat, Marble::GeoDataCoordinates::Unit = Marble::GeoDataCoordinates::Radian);
    static void             normalizeLonLat (qreal& lon /in,out/, qreal& lat /in,out/, Marble::GeoDataCoordinates::Unit = Marble::GeoDataCoordinates::Radian);
    static Marble::GeoDataCoordinates  fromString (const QString& string, bool& successful /out/);
    QString                 toString () const;
    QString                 toString (Marble::GeoDataCoordinates::Notation notation, int precision = -1) const;
    static QString          lonToString (qreal lon, Marble::GeoDataCoordinates::Notation notation, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian, int precision = -1, char format = 'f');
    QString                 lonToString () const;
    static QString          latToString (qreal lat, Marble::GeoDataCoordinates::Notation notation, Marble::GeoDataCoordinates::Unit unit = Marble::GeoDataCoordinates::Radian, int precision = -1, char format = 'f');
    QString                 latToString () const;
    bool                    operator == (const Marble::GeoDataCoordinates&) const;
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual void            detach ();
    virtual ~GeoDataCoordinates ();
    bool                    isPole (Marble::Pole = Marble::AnyPole) const;
    virtual bool            operator != (const Marble::GeoDataCoordinates&) const;
};
// GeoDataCoordinates

};
// Marble


