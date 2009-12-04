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
                            GeoDataPlacemark ();
                            GeoDataPlacemark (const Marble::GeoDataPlacemark& placemark);
                            GeoDataPlacemark (const Marble::GeoDataFeature& feature);
                            GeoDataPlacemark (const QString& name);
    bool                    operator == (const Marble::GeoDataPlacemark& other) const;
    Marble::GeoDataCoordinates  coordinate () const;
    Marble::GeoDataGeometry*  geometry () const;
//ig    void                    coordinate (qreal& longitude, qreal& latitude, qreal& altitude) const;
    void                    setCoordinate (qreal longitude, qreal latitude, qreal altitude = 0);
    void                    setCoordinate (const Marble::GeoDataPoint& point);
    void                    setGeometry (const Marble::GeoDataPoint& entry);
    void                    setGeometry (const Marble::GeoDataLineString& entry);
    void                    setGeometry (const Marble::GeoDataLinearRing& entry);
    void                    setGeometry (const Marble::GeoDataMultiGeometry& entry);
    void                    setGeometry (const Marble::GeoDataPolygon& entry);
    qreal                   area () const;
    void                    setArea (qreal area);
    qint64                  population () const;
    void                    setPopulation (qint64 population);
    const QString           countryCode () const;
    void                    setCountryCode (const QString& code);
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    virtual bool            isPlacemark () const;

    ~GeoDataPlacemark ();
    virtual QString         nodeType () const;
    virtual QXmlStreamWriter&  pack (QXmlStreamWriter& stream) const;
    virtual QXmlStreamWriter&  operator << (QXmlStreamWriter& stream) const;
};
// GeoDataPlacemark

};
// Marble


