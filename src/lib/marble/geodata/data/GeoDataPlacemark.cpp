//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//


// Own
#include "GeoDataPlacemark.h"

// Private
#include "GeoDataPlacemark_p.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataData.h"
#include "osm/OsmPlacemarkData.h"

// Qt
#include <QDataStream>
#include "MarbleDebug.h"
#include "GeoDataTrack.h"
#include "GeoDataModel.h"
#include <QString>
#include <QXmlStreamWriter>

namespace Marble
{

const OsmPlacemarkData GeoDataPlacemarkPrivate::s_nullOsmPlacemarkData = OsmPlacemarkData();
const GeoDataPlacemarkExtendedData GeoDataPlacemarkPrivate::s_nullPlacemarkExtendedData = GeoDataPlacemarkExtendedData();

GeoDataPlacemark::GeoDataPlacemark()
    : GeoDataFeature( new GeoDataPlacemarkPrivate )
{
    Q_D(GeoDataPlacemark);
    d->m_geometry->setParent(this);
}


GeoDataPlacemark::GeoDataPlacemark( const GeoDataPlacemark& other )
    : GeoDataFeature(other, new GeoDataPlacemarkPrivate(*other.d_func()))
{
    Q_D(GeoDataPlacemark);
    if (d->m_geometry) {
        d->m_geometry->setParent(this);
    }
}

GeoDataPlacemark::GeoDataPlacemark( const QString& name )
    : GeoDataFeature( new GeoDataPlacemarkPrivate )
{
    Q_D(GeoDataPlacemark);
    d->m_name = name;
    d->m_geometry->setParent(this);
}

GeoDataPlacemark::~GeoDataPlacemark()
{
    // nothing to do
}

GeoDataPlacemark &GeoDataPlacemark::operator=( const GeoDataPlacemark &other )
{
    if (this != &other) {
        Q_D(GeoDataPlacemark);
        *d = *other.d_func();
        if (d->m_geometry) {
            d->m_geometry->setParent(this);
        }
    }

    return *this;
}

bool GeoDataPlacemark::operator==( const GeoDataPlacemark& other ) const
{ 
    Q_D(const GeoDataPlacemark);
    const GeoDataPlacemarkPrivate* const other_d = other.d_func();
    if (!equals(other) ||
        d->m_population != other_d->m_population) {
        return false;
    }

    if ((d->m_placemarkExtendedData && !other_d->m_placemarkExtendedData) ||
        (!d->m_placemarkExtendedData && other_d->m_placemarkExtendedData)) {
        return false;
    }
    if (d->m_placemarkExtendedData && other_d->m_placemarkExtendedData &&
            !(*d->m_placemarkExtendedData == *other_d->m_placemarkExtendedData)) {
        return false;
    }

    if (!d->m_geometry && !other_d->m_geometry) {
        return true;
    }
    if ((!d->m_geometry && other_d->m_geometry) ||
        (d->m_geometry && !other_d->m_geometry)) {
        return false;
    }

    if (d->m_geometry->nodeType() != other_d->m_geometry->nodeType()) {
        return false;
    }

    if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataPolygonType) {
        GeoDataPolygon *thisPoly = dynamic_cast<GeoDataPolygon*>(d->m_geometry);
        GeoDataPolygon *otherPoly = dynamic_cast<GeoDataPolygon*>(other_d->m_geometry);
        Q_ASSERT( thisPoly && otherPoly );

        if ( *thisPoly != *otherPoly ) {
            return false;
        }
    } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataLineStringType) {
        GeoDataLineString *thisLine = dynamic_cast<GeoDataLineString*>(d->m_geometry);
        GeoDataLineString *otherLine = dynamic_cast<GeoDataLineString*>(other_d->m_geometry);
        Q_ASSERT( thisLine && otherLine );

        if ( *thisLine != *otherLine ) {
            return false;
        }
    } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataModelType) {
        GeoDataModel *thisModel = dynamic_cast<GeoDataModel*>(d->m_geometry);
        GeoDataModel *otherModel = dynamic_cast<GeoDataModel*>(other_d->m_geometry);
        Q_ASSERT( thisModel && otherModel );

        if ( *thisModel != *otherModel ) {
            return false;
        }
    /*} else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiGeometryType) {
        GeoDataMultiGeometry *thisMG = dynamic_cast<GeoDataMultiGeometry*>(d->m_geometry);
        GeoDataMultiGeometry *otherMG = dynamic_cast<GeoDataMultiGeometry*>(other_d->m_geometry);
        Q_ASSERT( thisMG && otherMG );

        if ( *thisMG != *otherMG ) {
            return false;
        } */ // Does not have equality operators. I guess they need to be implemented soon.
    } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataTrackType) {
        GeoDataTrack *thisTrack = dynamic_cast<GeoDataTrack*>(d->m_geometry);
        GeoDataTrack *otherTrack = dynamic_cast<GeoDataTrack*>(other_d->m_geometry);
        Q_ASSERT( thisTrack && otherTrack );

        if ( *thisTrack != *otherTrack ) {
            return false;
        }
    } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiTrackType) {
        GeoDataMultiTrack *thisMT = dynamic_cast<GeoDataMultiTrack*>(d->m_geometry);
        GeoDataMultiTrack *otherMT = dynamic_cast<GeoDataMultiTrack*>(other_d->m_geometry);
        Q_ASSERT( thisMT && otherMT );

        if ( *thisMT != *otherMT ) {
            return false;
        }
    } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataPointType) {
        GeoDataPoint *thisPoint = dynamic_cast<GeoDataPoint*>(d->m_geometry);
        GeoDataPoint *otherPoint = dynamic_cast<GeoDataPoint*>(other_d->m_geometry);
        Q_ASSERT( thisPoint && otherPoint );

        if ( *thisPoint != *otherPoint ) {
            return false;
        }
    }

    return true;
}

bool GeoDataPlacemark::operator!=( const GeoDataPlacemark& other ) const
{
    return !this->operator==( other );
}

const char* GeoDataPlacemark::nodeType() const
{
    return GeoDataTypes::GeoDataPlacemarkType;
}


GeoDataFeature * GeoDataPlacemark::clone() const
{
    return new GeoDataPlacemark(*this);
}


GeoDataPlacemark::GeoDataVisualCategory GeoDataPlacemark::visualCategory() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_visualCategory;
}

void GeoDataPlacemark::setVisualCategory(GeoDataPlacemark::GeoDataVisualCategory index)
{
    Q_D(GeoDataPlacemark);
    d->m_visualCategory = index;
}

GeoDataGeometry* GeoDataPlacemark::geometry()
{
    Q_D(GeoDataPlacemark);
    return d->m_geometry;
}

const GeoDataGeometry* GeoDataPlacemark::geometry() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_geometry;
}

const OsmPlacemarkData& GeoDataPlacemark::osmData() const
{
    Q_D(const GeoDataPlacemark);
    QVariant &placemarkVariantData = extendedData().valueRef( OsmPlacemarkData::osmHashKey() ).valueRef();
    if ( !placemarkVariantData.canConvert<OsmPlacemarkData>() ) {
        return d->s_nullOsmPlacemarkData;
    }

    OsmPlacemarkData &osmData = *reinterpret_cast<OsmPlacemarkData*>( placemarkVariantData.data() );
    return osmData;
}

void GeoDataPlacemark::setOsmData( const OsmPlacemarkData &osmData )
{
    extendedData().addValue( GeoDataData( OsmPlacemarkData::osmHashKey(), QVariant::fromValue( osmData ) ) );
}

OsmPlacemarkData& GeoDataPlacemark::osmData()
{
    QVariant &placemarkVariantData = extendedData().valueRef( OsmPlacemarkData::osmHashKey() ).valueRef();
    if ( !placemarkVariantData.canConvert<OsmPlacemarkData>() ) {
        extendedData().addValue( GeoDataData( OsmPlacemarkData::osmHashKey(), QVariant::fromValue( OsmPlacemarkData() ) ) );
        placemarkVariantData = extendedData().valueRef( OsmPlacemarkData::osmHashKey() ).valueRef();
    }

    OsmPlacemarkData &osmData = *reinterpret_cast<OsmPlacemarkData*>( placemarkVariantData.data() );
    return osmData;
}

bool GeoDataPlacemark::hasOsmData() const
{
    QVariant &placemarkVariantData = extendedData().valueRef( OsmPlacemarkData::osmHashKey() ).valueRef();
    return placemarkVariantData.canConvert<OsmPlacemarkData>();
}

void GeoDataPlacemark::clearOsmData()
{
    extendedData().removeKey(OsmPlacemarkData::osmHashKey());
}

const GeoDataLookAt *GeoDataPlacemark::lookAt() const
{
    return dynamic_cast<const GeoDataLookAt*>( abstractView() );
}

GeoDataLookAt *GeoDataPlacemark::lookAt()
{
    return dynamic_cast<GeoDataLookAt*>( abstractView() );
}

bool GeoDataPlacemark::placemarkLayoutOrderCompare(const GeoDataPlacemark *left, const GeoDataPlacemark *right)
{
    const GeoDataPlacemarkPrivate * const left_d = left->d_func();
    const GeoDataPlacemarkPrivate * const right_d = right->d_func();

    if (left_d->m_zoomLevel != right_d->m_zoomLevel) {
        return (left_d->m_zoomLevel < right_d->m_zoomLevel); // lower zoom level comes first
    }

    if (left_d->m_popularity != right_d->m_popularity) {
        return (left_d->m_popularity > right_d->m_popularity); // higher popularity comes first
    }

    return left < right; // lower pointer value comes first
}

GeoDataCoordinates GeoDataPlacemark::coordinate( const QDateTime &dateTime, bool *iconAtCoordinates ) const
{
    Q_D(const GeoDataPlacemark);
    bool hasIcon = false;
    GeoDataCoordinates coord;
 
    if (d->m_geometry) {
        // Beware: comparison between pointers, not strings.
        if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataPointType
                || d->m_geometry->nodeType() == GeoDataTypes::GeoDataPolygonType
                || d->m_geometry->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
            hasIcon = true;
            coord = d->m_geometry->latLonAltBox().center();
        } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiGeometryType) {
            const GeoDataMultiGeometry *multiGeometry = static_cast<const GeoDataMultiGeometry *>(d->m_geometry);

            QVector<GeoDataGeometry*>::ConstIterator it = multiGeometry->constBegin();
            QVector<GeoDataGeometry*>::ConstIterator end = multiGeometry->constEnd();
            for ( ; it != end; ++it ) {
                if ((*it)->nodeType() == GeoDataTypes::GeoDataPointType
                        || (*it)->nodeType() == GeoDataTypes::GeoDataPolygonType
                        || (*it)->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                    hasIcon = true;
                    break;
                }
            }

            coord = d->m_geometry->latLonAltBox().center();
        } else if (d->m_geometry->nodeType() == GeoDataTypes::GeoDataTrackType) {
            const GeoDataTrack *track = static_cast<const GeoDataTrack *>(d->m_geometry);
            hasIcon = track->size() != 0 && track->firstWhen() <= dateTime;
            coord = track->coordinatesAt( dateTime );
        } else {
            coord = d->m_geometry->latLonAltBox().center();
        }
    }

    if ( iconAtCoordinates != 0 ) {
        *iconAtCoordinates = hasIcon;
    }
    return coord;
}

void GeoDataPlacemark::coordinate( qreal& lon, qreal& lat, qreal& alt ) const
{
    coordinate().geoCoordinates( lon, lat, alt );
}

void GeoDataPlacemark::setCoordinate( qreal lon, qreal lat, qreal alt, GeoDataPoint::Unit _unit)
{
    setGeometry( new GeoDataPoint(lon, lat, alt, _unit ) );
}

void GeoDataPlacemark::setCoordinate( const GeoDataCoordinates &point )
{
    setGeometry ( new GeoDataPoint( point ) );
}

void GeoDataPlacemark::setGeometry( GeoDataGeometry *entry )
{
    Q_D(GeoDataPlacemark);
    delete d->m_geometry;
    d->m_geometry = entry;
    d->m_geometry->setParent(this);
}


QString GeoDataPlacemark::displayName() const
{
    if (hasOsmData()) {
        OsmPlacemarkData const &data = osmData();
        QStringList const uiLanguages = QLocale::system().uiLanguages();
        foreach (const QString &uiLanguage, uiLanguages) {
            for (auto tagIter = data.tagsBegin(), end = data.tagsEnd(); tagIter != end; ++tagIter) {
                if (tagIter.key().startsWith(QLatin1String("name:"))) {
                    QStringRef const tagLanguage = tagIter.key().midRef(5);
                    if (tagLanguage == uiLanguage) {
                        return tagIter.value();
                    }
                }
            }
        }
    }

    return name();
}

QString GeoDataPlacemark::categoryName() const
{
    Q_D(const GeoDataPlacemark);
    switch (d->m_visualCategory) {
    case Valley: return GeoDataPlacemarkPrivate::tr("Valley");
    case OtherTerrain: return GeoDataPlacemarkPrivate::tr("Terrain");
    case Crater: return GeoDataPlacemarkPrivate::tr("Crater");
    case Mare: return GeoDataPlacemarkPrivate::tr("Sea");
    case MannedLandingSite: return GeoDataPlacemarkPrivate::tr("Manned Landing Site");
    case RoboticRover: return GeoDataPlacemarkPrivate::tr("Robotic Rover");
    case UnmannedSoftLandingSite: return GeoDataPlacemarkPrivate::tr("Unmanned Soft Landing Site");
    case UnmannedHardLandingSite: return GeoDataPlacemarkPrivate::tr("Unmanned Hard Landing Site");
    case Mons: return GeoDataPlacemarkPrivate::tr("Mountain");
    case SmallCity: return GeoDataPlacemarkPrivate::tr("City");
    case SmallCountyCapital: return GeoDataPlacemarkPrivate::tr("County Capital");
    case SmallStateCapital: return GeoDataPlacemarkPrivate::tr("State Capital");
    case SmallNationCapital: return GeoDataPlacemarkPrivate::tr("Nation Capital");
    case MediumCity: return GeoDataPlacemarkPrivate::tr("City");
    case MediumCountyCapital: return GeoDataPlacemarkPrivate::tr("County Capital");
    case MediumStateCapital: return GeoDataPlacemarkPrivate::tr("State Capital");
    case MediumNationCapital: return GeoDataPlacemarkPrivate::tr("Nation Capital");
    case BigCity: return GeoDataPlacemarkPrivate::tr("City");
    case BigCountyCapital: return GeoDataPlacemarkPrivate::tr("County Capital");
    case BigStateCapital: return GeoDataPlacemarkPrivate::tr("State Capital");
    case BigNationCapital: return GeoDataPlacemarkPrivate::tr("Nation Capital");
    case LargeCity: return GeoDataPlacemarkPrivate::tr("City");
    case LargeCountyCapital: return GeoDataPlacemarkPrivate::tr("County Capital");
    case LargeStateCapital: return GeoDataPlacemarkPrivate::tr("State Capital");
    case LargeNationCapital: return GeoDataPlacemarkPrivate::tr("Nation Capital");
    case Nation: return GeoDataPlacemarkPrivate::tr("Nation");
    case Mountain: return GeoDataPlacemarkPrivate::tr("Mountain");
    case Volcano: return GeoDataPlacemarkPrivate::tr("Volcano");
    case Continent: return GeoDataPlacemarkPrivate::tr("Continent");
    case Ocean: return GeoDataPlacemarkPrivate::tr("Ocean");
    case GeographicPole: return GeoDataPlacemarkPrivate::tr("Geographic Pole");
    case MagneticPole: return GeoDataPlacemarkPrivate::tr("Magnetic Pole");
    case ShipWreck: return GeoDataPlacemarkPrivate::tr("Ship Wreck");
    case AirPort: return GeoDataPlacemarkPrivate::tr("Air Port");
    case Observatory: return GeoDataPlacemarkPrivate::tr("Observatory");
    case MilitaryDangerArea: return GeoDataPlacemarkPrivate::tr("Military Danger Area");
    case OsmSite: return GeoDataPlacemarkPrivate::tr("OSM Site");
    case Coordinate: return GeoDataPlacemarkPrivate::tr("Coordinate");
    case Bookmark: return GeoDataPlacemarkPrivate::tr("Bookmark");
    case Satellite: return GeoDataPlacemarkPrivate::tr("Satellite");

    // OpenStreetMap categories
    case PlaceCity: return GeoDataPlacemarkPrivate::tr("City");
    case PlaceCityCapital: return GeoDataPlacemarkPrivate::tr("City Capital");
    case PlaceSuburb: return GeoDataPlacemarkPrivate::tr("Suburb");
    case PlaceHamlet: return GeoDataPlacemarkPrivate::tr("Hamlet");
    case PlaceLocality: return GeoDataPlacemarkPrivate::tr("Locality");
    case PlaceTown: return GeoDataPlacemarkPrivate::tr("Town");
    case PlaceTownCapital: return GeoDataPlacemarkPrivate::tr("Town Capital");
    case PlaceVillage: return GeoDataPlacemarkPrivate::tr("Village");
    case PlaceVillageCapital: return GeoDataPlacemarkPrivate::tr("Village Capital");
    case NaturalWater: return GeoDataPlacemarkPrivate::tr("Water");
    case NaturalReef: return GeoDataPlacemarkPrivate::tr("Reef");
    case NaturalWood: return GeoDataPlacemarkPrivate::tr("Wood");
    case NaturalBeach: return GeoDataPlacemarkPrivate::tr("Beach");
    case NaturalWetland: return GeoDataPlacemarkPrivate::tr("Wetland");
    case NaturalGlacier: return GeoDataPlacemarkPrivate::tr("Glacier");
    case NaturalIceShelf: return GeoDataPlacemarkPrivate::tr("Ice Shelf");
    case NaturalScrub: return GeoDataPlacemarkPrivate::tr("Scrub");
    case NaturalCliff: return GeoDataPlacemarkPrivate::tr("Cliff");
    case NaturalHeath: return GeoDataPlacemarkPrivate::tr("Heath");
    case HighwayTrafficSignals: return GeoDataPlacemarkPrivate::tr("Traffic Signals");
    case HighwaySteps: return GeoDataPlacemarkPrivate::tr("Steps");
    case HighwayUnknown: return GeoDataPlacemarkPrivate::tr("Unknown Road");
    case HighwayPath: return GeoDataPlacemarkPrivate::tr("Path");
    case HighwayFootway: return GeoDataPlacemarkPrivate::tr("Footway");
    case HighwayTrack: return GeoDataPlacemarkPrivate::tr("Track");
    case HighwayPedestrian: return GeoDataPlacemarkPrivate::tr("Footway");
    case HighwayCycleway: return GeoDataPlacemarkPrivate::tr("Cycleway");
    case HighwayService: return GeoDataPlacemarkPrivate::tr("Service Road");
    case HighwayRoad: return GeoDataPlacemarkPrivate::tr("Road");
    case HighwayResidential: return GeoDataPlacemarkPrivate::tr("Residential Road");
    case HighwayLivingStreet: return GeoDataPlacemarkPrivate::tr("Living Street");
    case HighwayUnclassified: return GeoDataPlacemarkPrivate::tr("Unclassified Road");
    case HighwayTertiaryLink: return GeoDataPlacemarkPrivate::tr("Tertiary Link Road");
    case HighwayTertiary: return GeoDataPlacemarkPrivate::tr("Tertiary Road");
    case HighwaySecondaryLink: return GeoDataPlacemarkPrivate::tr("Secondary Link Road");
    case HighwaySecondary: return GeoDataPlacemarkPrivate::tr("Secondary Road");
    case HighwayPrimaryLink: return GeoDataPlacemarkPrivate::tr("Primary Link Road");
    case HighwayPrimary: return GeoDataPlacemarkPrivate::tr("Primary Road");
    case HighwayTrunkLink: return GeoDataPlacemarkPrivate::tr("Trunk Link Road");
    case HighwayTrunk: return GeoDataPlacemarkPrivate::tr("Trunk Road");
    case HighwayMotorwayLink: return GeoDataPlacemarkPrivate::tr("Motorway Link Road");
    case HighwayMotorway: return GeoDataPlacemarkPrivate::tr("Motorway");
    case Building: return GeoDataPlacemarkPrivate::tr("Building");
    case AccomodationCamping: return GeoDataPlacemarkPrivate::tr("Camping");
    case AccomodationHostel: return GeoDataPlacemarkPrivate::tr("Hostel");
    case AccomodationHotel: return GeoDataPlacemarkPrivate::tr("Hotel");
    case AccomodationMotel: return GeoDataPlacemarkPrivate::tr("Motel");
    case AccomodationYouthHostel: return GeoDataPlacemarkPrivate::tr("Youth Hostel");
    case AccomodationGuestHouse: return GeoDataPlacemarkPrivate::tr("Guest House");
    case AmenityLibrary: return GeoDataPlacemarkPrivate::tr("Library");
    case AmenityKindergarten: return GeoDataPlacemarkPrivate::tr("Kindergarten");
    case EducationCollege: return GeoDataPlacemarkPrivate::tr("College");
    case EducationSchool: return GeoDataPlacemarkPrivate::tr("School");
    case EducationUniversity: return GeoDataPlacemarkPrivate::tr("University");
    case FoodBar: return GeoDataPlacemarkPrivate::tr("Bar");
    case FoodBiergarten: return GeoDataPlacemarkPrivate::tr("Biergarten");
    case FoodCafe: return GeoDataPlacemarkPrivate::tr("Cafe");
    case FoodFastFood: return GeoDataPlacemarkPrivate::tr("Fast Food");
    case FoodPub: return GeoDataPlacemarkPrivate::tr("Pub");
    case FoodRestaurant: return GeoDataPlacemarkPrivate::tr("Restaurant");
    case HealthDentist: return GeoDataPlacemarkPrivate::tr("Dentist");
    case HealthDoctors: return GeoDataPlacemarkPrivate::tr("Doctors");
    case HealthHospital: return GeoDataPlacemarkPrivate::tr("Hospital");
    case HealthPharmacy: return GeoDataPlacemarkPrivate::tr("Pharmacy");
    case HealthVeterinary: return GeoDataPlacemarkPrivate::tr("Veterinary");
    case MoneyAtm: return GeoDataPlacemarkPrivate::tr("ATM");
    case MoneyBank: return GeoDataPlacemarkPrivate::tr("Bank");
    case AmenityArchaeologicalSite: return GeoDataPlacemarkPrivate::tr("Archaeological Site");
    case AmenityEmbassy: return GeoDataPlacemarkPrivate::tr("Embassy");
    case AmenityEmergencyPhone: return GeoDataPlacemarkPrivate::tr("Emergency Phone");
    case AmenityWaterPark: return GeoDataPlacemarkPrivate::tr("Water Park");
    case AmenityCommunityCentre: return GeoDataPlacemarkPrivate::tr("Community Centre");
    case AmenityFountain: return GeoDataPlacemarkPrivate::tr("Fountain");
    case AmenityNightClub: return GeoDataPlacemarkPrivate::tr("Night Club");
    case AmenityBench: return GeoDataPlacemarkPrivate::tr("Bench");
    case AmenityCourtHouse: return GeoDataPlacemarkPrivate::tr("Court House");
    case AmenityFireStation: return GeoDataPlacemarkPrivate::tr("Fire Station");
    case AmenityHuntingStand: return GeoDataPlacemarkPrivate::tr("Hunting Stand");
    case AmenityPolice: return GeoDataPlacemarkPrivate::tr("Police");
    case AmenityPostBox: return GeoDataPlacemarkPrivate::tr("Post Box");
    case AmenityPostOffice: return GeoDataPlacemarkPrivate::tr("Post Office");
    case AmenityPrison: return GeoDataPlacemarkPrivate::tr("Prison");
    case AmenityRecycling: return GeoDataPlacemarkPrivate::tr("Recycling");
    case AmenityShelter: return GeoDataPlacemarkPrivate::tr("Shelter");
    case AmenityTelephone: return GeoDataPlacemarkPrivate::tr("Telephone");
    case AmenityToilets: return GeoDataPlacemarkPrivate::tr("Toilets");
    case AmenityTownHall: return GeoDataPlacemarkPrivate::tr("Town Hall");
    case AmenityWasteBasket: return GeoDataPlacemarkPrivate::tr("Waste Basket");
    case AmenityDrinkingWater: return GeoDataPlacemarkPrivate::tr("Drinking Water");
    case AmenityGraveyard: return GeoDataPlacemarkPrivate::tr("Graveyard");
    case BarrierCityWall: return GeoDataPlacemarkPrivate::tr("City Wall");
    case BarrierGate: return GeoDataPlacemarkPrivate::tr("Gate");
    case BarrierLiftGate: return GeoDataPlacemarkPrivate::tr("Lift Gate");
    case BarrierWall: return GeoDataPlacemarkPrivate::tr("Wall");
    case NaturalPeak: return GeoDataPlacemarkPrivate::tr("Peak");
    case NaturalTree: return GeoDataPlacemarkPrivate::tr("Tree");
    case ShopBeverages: return GeoDataPlacemarkPrivate::tr("Beverages");
    case ShopHifi: return GeoDataPlacemarkPrivate::tr("Hifi");
    case ShopSupermarket: return GeoDataPlacemarkPrivate::tr("Supermarket");
    case ShopAlcohol: return GeoDataPlacemarkPrivate::tr("Alcohol");
    case ShopBakery: return GeoDataPlacemarkPrivate::tr("Bakery");
    case ShopButcher: return GeoDataPlacemarkPrivate::tr("Butcher");
    case ShopConfectionery: return GeoDataPlacemarkPrivate::tr("Confectionery");
    case ShopConvenience: return GeoDataPlacemarkPrivate::tr("Convenience Shop");
    case ShopGreengrocer: return GeoDataPlacemarkPrivate::tr("Greengrocer");
    case ShopSeafood: return GeoDataPlacemarkPrivate::tr("Seafood");
    case ShopDepartmentStore: return GeoDataPlacemarkPrivate::tr("Department Store");
    case ShopKiosk: return GeoDataPlacemarkPrivate::tr("Kiosk");
    case ShopBag: return GeoDataPlacemarkPrivate::tr("Bag");
    case ShopClothes: return GeoDataPlacemarkPrivate::tr("Clothes");
    case ShopFashion: return GeoDataPlacemarkPrivate::tr("Fashion");
    case ShopJewelry: return GeoDataPlacemarkPrivate::tr("Jewelry");
    case ShopShoes: return GeoDataPlacemarkPrivate::tr("Shoes");
    case ShopVarietyStore: return GeoDataPlacemarkPrivate::tr("Variety Store");
    case ShopBeauty: return GeoDataPlacemarkPrivate::tr("Beauty");
    case ShopChemist: return GeoDataPlacemarkPrivate::tr("Chemist");
    case ShopCosmetics: return GeoDataPlacemarkPrivate::tr("Cosmetics");
    case ShopHairdresser: return GeoDataPlacemarkPrivate::tr("Hairdresser");
    case ShopOptician: return GeoDataPlacemarkPrivate::tr("Optician");
    case ShopPerfumery: return GeoDataPlacemarkPrivate::tr("Perfumery");
    case ShopDoitYourself: return GeoDataPlacemarkPrivate::tr("Doit Yourself");
    case ShopFlorist: return GeoDataPlacemarkPrivate::tr("Florist");
    case ShopHardware: return GeoDataPlacemarkPrivate::tr("Hardware");
    case ShopFurniture: return GeoDataPlacemarkPrivate::tr("Furniture");
    case ShopElectronics: return GeoDataPlacemarkPrivate::tr("Electronics");
    case ShopMobilePhone: return GeoDataPlacemarkPrivate::tr("Mobile Phone");
    case ShopBicycle: return GeoDataPlacemarkPrivate::tr("Bicycle");
    case ShopCar: return GeoDataPlacemarkPrivate::tr("Car");
    case ShopCarRepair: return GeoDataPlacemarkPrivate::tr("Car Repair");
    case ShopCarParts: return GeoDataPlacemarkPrivate::tr("Car Parts");
    case ShopMotorcycle: return GeoDataPlacemarkPrivate::tr("Motorcycle");
    case ShopOutdoor: return GeoDataPlacemarkPrivate::tr("Outdoor");
    case ShopMusicalInstrument: return GeoDataPlacemarkPrivate::tr("Musical Instrument");
    case ShopPhoto: return GeoDataPlacemarkPrivate::tr("Photo");
    case ShopBook: return GeoDataPlacemarkPrivate::tr("Book");
    case ShopGift: return GeoDataPlacemarkPrivate::tr("Gift");
    case ShopStationery: return GeoDataPlacemarkPrivate::tr("Stationery");
    case ShopLaundry: return GeoDataPlacemarkPrivate::tr("Laundry");
    case ShopPet: return GeoDataPlacemarkPrivate::tr("Pet");
    case ShopToys: return GeoDataPlacemarkPrivate::tr("Toys");
    case ShopTravelAgency: return GeoDataPlacemarkPrivate::tr("Travel Agency");
    case Shop: return GeoDataPlacemarkPrivate::tr("Shop");
    case ManmadeBridge: return GeoDataPlacemarkPrivate::tr("Bridge");
    case ManmadeLighthouse: return GeoDataPlacemarkPrivate::tr("Lighthouse");
    case ManmadePier: return GeoDataPlacemarkPrivate::tr("Pier");
    case ManmadeWaterTower: return GeoDataPlacemarkPrivate::tr("Water Tower");
    case ManmadeWindMill: return GeoDataPlacemarkPrivate::tr("Wind Mill");
    case TouristAttraction: return GeoDataPlacemarkPrivate::tr("Tourist Attraction");
    case TouristCastle: return GeoDataPlacemarkPrivate::tr("Castle");
    case TouristCinema: return GeoDataPlacemarkPrivate::tr("Cinema");
    case TouristInformation: return GeoDataPlacemarkPrivate::tr("Information");
    case TouristMonument: return GeoDataPlacemarkPrivate::tr("Monument");
    case TouristMuseum: return GeoDataPlacemarkPrivate::tr("Museum");
    case TouristRuin: return GeoDataPlacemarkPrivate::tr("Ruin");
    case TouristTheatre: return GeoDataPlacemarkPrivate::tr("Theatre");
    case TouristThemePark: return GeoDataPlacemarkPrivate::tr("Theme Park");
    case TouristViewPoint: return GeoDataPlacemarkPrivate::tr("View Point");
    case TouristZoo: return GeoDataPlacemarkPrivate::tr("Zoo");
    case TouristAlpineHut: return GeoDataPlacemarkPrivate::tr("Alpine Hut");
    case TransportAerodrome: return GeoDataPlacemarkPrivate::tr("Aerodrome");
    case TransportHelipad: return GeoDataPlacemarkPrivate::tr("Helipad");
    case TransportAirportGate: return GeoDataPlacemarkPrivate::tr("Airport Gate");
    case TransportAirportRunway: return GeoDataPlacemarkPrivate::tr("Airport Runway");
    case TransportAirportApron: return GeoDataPlacemarkPrivate::tr("Airport Apron");
    case TransportAirportTaxiway: return GeoDataPlacemarkPrivate::tr("Airport Taxiway");
    case TransportAirportTerminal: return GeoDataPlacemarkPrivate::tr("Airport Terminal");
    case TransportBusStation: return GeoDataPlacemarkPrivate::tr("Bus Station");
    case TransportBusStop: return GeoDataPlacemarkPrivate::tr("Bus Stop");
    case TransportCarShare: return GeoDataPlacemarkPrivate::tr("Car Sharing");
    case TransportFuel: return GeoDataPlacemarkPrivate::tr("Gas Station");
    case TransportParking: return GeoDataPlacemarkPrivate::tr("Parking");
    case TransportParkingSpace: return GeoDataPlacemarkPrivate::tr("Parking Space");
    case TransportPlatform: return GeoDataPlacemarkPrivate::tr("Platform");
    case TransportRentalBicycle: return GeoDataPlacemarkPrivate::tr("Rental Bicycle");
    case TransportRentalCar: return GeoDataPlacemarkPrivate::tr("Rental Car");
    case TransportTaxiRank: return GeoDataPlacemarkPrivate::tr("Taxi Rank");
    case TransportTrainStation: return GeoDataPlacemarkPrivate::tr("Train Station");
    case TransportTramStop: return GeoDataPlacemarkPrivate::tr("Tram Stop");
    case TransportBicycleParking: return GeoDataPlacemarkPrivate::tr("Bicycle Parking");
    case TransportMotorcycleParking: return GeoDataPlacemarkPrivate::tr("Motorcycle Parking");
    case TransportSubwayEntrance: return GeoDataPlacemarkPrivate::tr("Subway Entrance");
    case ReligionPlaceOfWorship: return GeoDataPlacemarkPrivate::tr("Place Of Worship");
    case ReligionBahai: return GeoDataPlacemarkPrivate::tr("Bahai");
    case ReligionBuddhist: return GeoDataPlacemarkPrivate::tr("Buddhist");
    case ReligionChristian: return GeoDataPlacemarkPrivate::tr("Christian");
    case ReligionMuslim: return GeoDataPlacemarkPrivate::tr("Muslim");
    case ReligionHindu: return GeoDataPlacemarkPrivate::tr("Hindu");
    case ReligionJain: return GeoDataPlacemarkPrivate::tr("Jain");
    case ReligionJewish: return GeoDataPlacemarkPrivate::tr("Jewish");
    case ReligionShinto: return GeoDataPlacemarkPrivate::tr("Shinto");
    case ReligionSikh: return GeoDataPlacemarkPrivate::tr("Sikh");
    case LeisureGolfCourse: return GeoDataPlacemarkPrivate::tr("Golf Course");
    case LeisureMarina: return GeoDataPlacemarkPrivate::tr("Marina");
    case LeisurePark: return GeoDataPlacemarkPrivate::tr("Park");
    case LeisurePlayground: return GeoDataPlacemarkPrivate::tr("Playground");
    case LeisurePitch: return GeoDataPlacemarkPrivate::tr("Pitch");
    case LeisureSportsCentre: return GeoDataPlacemarkPrivate::tr("Sports Centre");
    case LeisureStadium: return GeoDataPlacemarkPrivate::tr("Stadium");
    case LeisureTrack: return GeoDataPlacemarkPrivate::tr("Track");
    case LeisureSwimmingPool: return GeoDataPlacemarkPrivate::tr("Swimming Pool");
    case LanduseAllotments: return GeoDataPlacemarkPrivate::tr("Allotments");
    case LanduseBasin: return GeoDataPlacemarkPrivate::tr("Basin");
    case LanduseCemetery: return GeoDataPlacemarkPrivate::tr("Cemetery");
    case LanduseCommercial: return GeoDataPlacemarkPrivate::tr("Commercial");
    case LanduseConstruction: return GeoDataPlacemarkPrivate::tr("Construction");
    case LanduseFarmland: return GeoDataPlacemarkPrivate::tr("Farmland");
    case LanduseFarmyard: return GeoDataPlacemarkPrivate::tr("Farmyard");
    case LanduseGarages: return GeoDataPlacemarkPrivate::tr("Garages");
    case LanduseGrass: return GeoDataPlacemarkPrivate::tr("Grass");
    case LanduseIndustrial: return GeoDataPlacemarkPrivate::tr("Industrial");
    case LanduseLandfill: return GeoDataPlacemarkPrivate::tr("Landfill");
    case LanduseMeadow: return GeoDataPlacemarkPrivate::tr("Meadow");
    case LanduseMilitary: return GeoDataPlacemarkPrivate::tr("Military");
    case LanduseQuarry: return GeoDataPlacemarkPrivate::tr("Quarry");
    case LanduseRailway: return GeoDataPlacemarkPrivate::tr("Railway");
    case LanduseReservoir: return GeoDataPlacemarkPrivate::tr("Reservoir");
    case LanduseResidential: return GeoDataPlacemarkPrivate::tr("Residential");
    case LanduseRetail: return GeoDataPlacemarkPrivate::tr("Retail");
    case LanduseOrchard: return GeoDataPlacemarkPrivate::tr("Orchard");
    case LanduseVineyard: return GeoDataPlacemarkPrivate::tr("Vineyard");
    case RailwayRail: return GeoDataPlacemarkPrivate::tr("Rail");
    case RailwayNarrowGauge: return GeoDataPlacemarkPrivate::tr("Narrow Gauge");
    case RailwayTram: return GeoDataPlacemarkPrivate::tr("Tram");
    case RailwayLightRail: return GeoDataPlacemarkPrivate::tr("Light Rail");
    case RailwayAbandoned: return GeoDataPlacemarkPrivate::tr("Abandoned Railway");
    case RailwaySubway: return GeoDataPlacemarkPrivate::tr("Subway");
    case RailwayPreserved: return GeoDataPlacemarkPrivate::tr("Preserved Railway");
    case RailwayMiniature: return GeoDataPlacemarkPrivate::tr("Miniature Railway");
    case RailwayConstruction: return GeoDataPlacemarkPrivate::tr("Railway Construction");
    case RailwayMonorail: return GeoDataPlacemarkPrivate::tr("Monorail");
    case RailwayFunicular: return GeoDataPlacemarkPrivate::tr("Funicular Railway");
    case PowerTower: return GeoDataPlacemarkPrivate::tr("Power Tower");
    case AdminLevel1: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 1)");
    case AdminLevel2: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 2)");
    case AdminLevel3: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 3)");
    case AdminLevel4: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 4)");
    case AdminLevel5: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 5)");
    case AdminLevel6: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 6)");
    case AdminLevel7: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 7)");
    case AdminLevel8: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 8)");
    case AdminLevel9: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 9)");
    case AdminLevel10: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 10)");
    case AdminLevel11: return GeoDataPlacemarkPrivate::tr("Admin Boundary (Level 11)");
    case BoundaryMaritime: return GeoDataPlacemarkPrivate::tr("Boundary (Maritime)");
    case Landmass: return GeoDataPlacemarkPrivate::tr("Land Mass");
    case UrbanArea: return GeoDataPlacemarkPrivate::tr("Urban Area");
    case InternationalDateLine: return GeoDataPlacemarkPrivate::tr("International Date Line");
    case Bathymetry: return GeoDataPlacemarkPrivate::tr("Bathymetry");

    case Default:
    case Unknown:
    case None:
    case LastIndex: return QString();
    }

    return QString();
}

qreal GeoDataPlacemark::area() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_placemarkExtendedData ? d->m_placemarkExtendedData->m_area : -1.0;
}

void GeoDataPlacemark::setArea( qreal area )
{
    if (area == -1.0 && !d_func()->m_placemarkExtendedData) {
        return; // nothing to do
    }

    Q_D(GeoDataPlacemark);
    d->placemarkExtendedData().m_area = area;
}

qint64 GeoDataPlacemark::population() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_population;
}

void GeoDataPlacemark::setPopulation( qint64 population )
{
    Q_D(GeoDataPlacemark);
    d->m_population = population;
}

const QString GeoDataPlacemark::state() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_placemarkExtendedData ? d->m_placemarkExtendedData->m_state : QString();
}

void GeoDataPlacemark::setState( const QString &state )
{
    if (state.isEmpty() && !d_func()->m_placemarkExtendedData) {
        return; // nothing to do
    }

    Q_D(GeoDataPlacemark);
    d->placemarkExtendedData().m_state = state;
}

const QString GeoDataPlacemark::countryCode() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_placemarkExtendedData ? d->m_placemarkExtendedData->m_countrycode : QString();
}

void GeoDataPlacemark::setCountryCode( const QString &countrycode )
{
    if (countrycode.isEmpty() && !d_func()->m_placemarkExtendedData) {
        return; // nothing to do
    }

    Q_D(GeoDataPlacemark);
    d->placemarkExtendedData().m_countrycode = countrycode;
}

bool GeoDataPlacemark::isBalloonVisible() const
{
    Q_D(const GeoDataPlacemark);
    return d->m_placemarkExtendedData ? d->m_placemarkExtendedData->m_isBalloonVisible : false;
}

void GeoDataPlacemark::setBalloonVisible( bool visible )
{
    if (!visible && !d_func()->m_placemarkExtendedData) {
        return; // nothing to do
    }

    Q_D(GeoDataPlacemark);
    d->placemarkExtendedData().m_isBalloonVisible = visible;
}

void GeoDataPlacemark::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataPlacemark);
    GeoDataFeature::pack( stream );

    stream << d->placemarkExtendedData().m_countrycode;
    stream << d->placemarkExtendedData().m_area;
    stream << d->m_population;
    if (d->m_geometry) {
        stream << d->m_geometry->geometryId();
        d->m_geometry->pack( stream );
    }
    else
    {
        stream << InvalidGeometryId;
    }
}

QXmlStreamWriter& GeoDataPlacemark::pack( QXmlStreamWriter& stream ) const
{
    stream.writeStartElement( "placemark" );

    stream.writeEndElement();
    return stream;
}

QXmlStreamWriter& GeoDataPlacemark::operator <<( QXmlStreamWriter& stream ) const
{
    pack( stream );
    return stream;
}

void GeoDataPlacemark::unpack( QDataStream& stream )
{
    Q_D(GeoDataPlacemark);
    GeoDataFeature::unpack( stream );

    stream >> d->placemarkExtendedData().m_countrycode;
    stream >> d->placemarkExtendedData().m_area;
    stream >> d->m_population;
    int geometryId;
    stream >> geometryId;
    GeoDataGeometry *geometry = nullptr;
    switch( geometryId ) {
        case InvalidGeometryId:
            break;
        case GeoDataPointId:
            {
            GeoDataPoint* point = new GeoDataPoint;
            point->unpack( stream );
            geometry = point;
            }
            break;
        case GeoDataLineStringId:
            {
            GeoDataLineString* lineString = new GeoDataLineString;
            lineString->unpack( stream );
            geometry = lineString;
            }
            break;
        case GeoDataLinearRingId:
            {
            GeoDataLinearRing* linearRing = new GeoDataLinearRing;
            linearRing->unpack( stream );
            geometry = linearRing;
            }
            break;
        case GeoDataPolygonId:
            {
            GeoDataPolygon* polygon = new GeoDataPolygon;
            polygon->unpack( stream );
            geometry = polygon;
            }
            break;
        case GeoDataMultiGeometryId:
            {
            GeoDataMultiGeometry* multiGeometry = new GeoDataMultiGeometry;
            multiGeometry->unpack( stream );
            geometry = multiGeometry;
            }
            break;
        case GeoDataModelId:
            break;
        default: break;
    };
    if (geometry) {
       delete d->m_geometry;
       d->m_geometry = geometry;
       d->m_geometry->setParent(this);
    }
}

}
