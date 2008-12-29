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

class GeoDataPlacemark : Marble::GeoDataFeature
{
%TypeHeaderCode
#include <GeoDataPlacemark.h>
%End

public:
    explicit                GeoDataPlacemark (Marble::GeoDataObject* parent = 0);
    explicit                GeoDataPlacemark (const QString& name, Marble::GeoDataObject* parent = 0);
    Marble::GeoDataCoordinates  coordinate () const;
    Marble::GeoDataGeometry*  geometry ();

//ig    void                    coordinate (qreal& longitude /out/, qreal& latitude /out/, qreal& altitude /out/);
    void                    setCoordinate (qreal longitude, qreal latitude, qreal altitude = 0);
    void                    setCoordinate (const Marble::GeoDataPoint& point);
    void                    setGeometry (Marble::GeoDataPoint* entry);
    void                    setGeometry (Marble::GeoDataLineString* entry);
    void                    setGeometry (Marble::GeoDataLinearRing* entry);
    void                    setGeometry (Marble::GeoDataMultiGeometry* entry);
    void                    setGeometry (Marble::GeoDataPolygon* entry);
    qreal                   area () const;
    void                    setArea (qreal area);
    qint64                  population () const;
    void                    setPopulation (qint64 population);
    const QString           countryCode () const;
    void                    setCountryCode (const QString& code);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual bool            isPlacemark () const;
    virtual EnumFeatureId   featureId () const;

private:
                            GeoDataPlacemark (const Marble::GeoDataPlacemark&);
};   // GeoDataPlacemark

};   // Marble


