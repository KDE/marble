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

class GeoDataFeature : Marble::GeoDataObject
{
%TypeHeaderCode
#include <GeoDataFeature.h>
%End

public:
                            GeoDataFeature ();
                            GeoDataFeature (const QString& name);
                            GeoDataFeature (const Marble::GeoDataFeature& other);
    bool                    operator == (const Marble::GeoDataFeature&) const;
    EnumFeatureId           featureId () const;

    enum GeoDataVisualCategory
    {
        None,
        Default,
        Unknown,
        SmallCity,
        SmallCountyCapital,
        SmallStateCapital,
        SmallNationCapital,
        MediumCity,
        MediumCountyCapital,
        MediumStateCapital,
        MediumNationCapital,
        BigCity,
        BigCountyCapital,
        BigStateCapital,
        BigNationCapital,
        LargeCity,
        LargeCountyCapital,
        LargeStateCapital,
        LargeNationCapital,
        Nation,
        Mountain,
        Volcano,
        Mons,
        Valley,
        Continent,
        Ocean,
        OtherTerrain,
        Crater,
        Mare,
        GeographicPole,
        MagneticPole,
        ShipWreck,
        AirPort,
        Observatory,
        Wikipedia,
        OsmSite,
        Coordinate,
        MannedLandingSite,
        RoboticRover,
        UnmannedSoftLandingSite,
        UnmannedHardLandingSite,
        LastIndex
    };

    QString                 name () const;
    void                    setName (const QString& value);
    QString                 address () const;
    void                    setAddress (const QString& value);
    QString                 phoneNumber () const;
    void                    setPhoneNumber (const QString& value);
    QString                 description () const;
    void                    setDescription (const QString& value);
    QString                 styleUrl () const;
    void                    setStyleUrl (const QString& value);
    bool                    isVisible () const;
    void                    setVisible (bool value);
    Marble::GeoDataStyle*   style () const;
    void                    setStyle (Marble::GeoDataStyle* style);
    Marble::GeoDataFeature::GeoDataVisualCategory  visualCategory () const;
    void                    setVisualCategory (Marble::GeoDataFeature::GeoDataVisualCategory category);
//ig    const QChar             role () const;
    int                     popularityIndex () const;
    void                    setPopularityIndex (int index);
    qint64                  popularity () const;
    void                    setPopularity (qint64 popularity);
    Marble::GeoDataStyleMap*  styleMap () const;
    void                    setStyleMap (Marble::GeoDataStyleMap* map);
    const QSize             symbolSize () const;
    const QPixmap           symbolPixmap () const;
    static void             resetDefaultStyles ();
    virtual void            pack (QDataStream& stream) const;
    virtual void            unpack (QDataStream& stream);
    static QFont            defaultFont ();
    static void             setDefaultFont (const QFont& font);
    virtual void            detach ();

    virtual ~GeoDataFeature ();
    virtual QString         nodeType () const;
    bool                    descriptionIsCDATA () const;
    void                    setDescriptionCDATA (bool cdata);
    Marble::GeoDataRegion&  region () const;
    void                    setRegion (const Marble::GeoDataRegion& region);
protected:
//ig                            GeoDataFeature (GeoDataFeaturePrivate* priv);
public:
    Marble::GeoDataAbstractView  abstractView () const;
    void                    setAbstractView (const Marble::GeoDataAbstractView& abstractView);
//ig    Marble::GeoDataTimeSpan&  timeSpan () const;
//ig    GeoDataTimeStamp&       timeStamp () const;
//ig    void                    setTimeStamp (GeoDataTimeStamp timeStamp);
//ig    GeoDataExtendedData&    extendedData () const;
//ig    void                    setExtendedData (const GeoDataExtendedData& extendedData);
    void                    setRole (const QString& role);
//ig    void                    setTimeSpan (GeoDataTimeSpan timeSpan);
};
// GeoDataFeature

};
// Marble


