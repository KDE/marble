//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataFeature.h"
#include "GeoDataFeature_p.h"

#include <QDataStream>
#include <QSize>

#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRegion.h"
#include "GeoDataCamera.h"

namespace Marble
{

const QSharedPointer<const GeoDataStyle> GeoDataFeaturePrivate::s_defaultStyle(new GeoDataStyle);

GeoDataFeature::GeoDataFeature()
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject(),
      d( other.d )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const QString& name )
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
    d->m_name = name;
}

GeoDataFeature::GeoDataFeature( GeoDataFeaturePrivate *priv )
    : d( priv )
{
    d->ref.ref();
}

GeoDataFeature::~GeoDataFeature()
{
    if (!d->ref.deref()) {
        delete d;
    }
}

GeoDataFeaturePrivate* GeoDataFeature::p() const
{
    return static_cast<GeoDataFeaturePrivate*>(d);
}

GeoDataFeature& GeoDataFeature::operator=( const GeoDataFeature& other )
{
    if (!d->ref.deref()) {
        delete d;
    }

    d = other.d;
    d->ref.ref();

    return *this;
}

bool GeoDataFeature::equals( const GeoDataFeature &other ) const
{
    if ( !GeoDataObject::equals(other) ||
         d->m_name != other.d->m_name ||
         d->m_styleUrl != other.d->m_styleUrl ||
         d->m_popularity != other.d->m_popularity ||
         d->m_zoomLevel != other.d->m_zoomLevel ||
         d->m_visible != other.d->m_visible ||
         d->m_role != other.d->m_role ||
         d->m_extendedData != other.d->m_extendedData ||
         *style() != *other.style() ) {
        return false;
    }

    if ( (!d->m_styleMap && other.d->m_styleMap) ||
         (d->m_styleMap && !other.d->m_styleMap) ) {
        return false;
    }

    if ( (d->m_styleMap && other.d->m_styleMap) &&
         (*d->m_styleMap != *other.d->m_styleMap) ) {
        return false;
    }

    if ( (!d->m_featureExtendedData && other.d->m_featureExtendedData && other.d->m_featureExtendedData->m_abstractView) ||
         (d->m_featureExtendedData && d->m_featureExtendedData->m_abstractView && !other.d->m_featureExtendedData) ) {
        return false;
    }

    if ( (d->m_featureExtendedData && other.d->m_featureExtendedData) &&
         (*d->m_featureExtendedData != *other.d->m_featureExtendedData) ) {
        return false;
    }

    return true;
}

const char* GeoDataFeature::nodeType() const
{
    return p()->nodeType();
}

EnumFeatureId GeoDataFeature::featureId() const
{
    return d->featureId();
}

QString GeoDataFeature::name() const
{
    return d->m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    detach();
    d->m_name = value;
}

GeoDataSnippet GeoDataFeature::snippet() const
{
    return d->featureExtendedData().m_snippet;
}

void GeoDataFeature::setSnippet( const GeoDataSnippet &snippet )
{
    detach();
    d->featureExtendedData().m_snippet = snippet;
}

QString GeoDataFeature::address() const
{
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    detach();
    d->featureExtendedData().m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    detach();
    d->featureExtendedData().m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    detach();
    d->featureExtendedData().m_description = value;
}

bool GeoDataFeature::descriptionIsCDATA() const
{
    if (!d->m_featureExtendedData) {
        return false;
    }

    return d->featureExtendedData().m_descriptionCDATA;
}

void GeoDataFeature::setDescriptionCDATA( bool cdata )
{
    detach();
    d->featureExtendedData().m_descriptionCDATA = cdata;
}

const GeoDataAbstractView* GeoDataFeature::abstractView() const
{
    if (!d->m_featureExtendedData) {
        return nullptr;
    }

    return d->featureExtendedData().m_abstractView;
}

GeoDataAbstractView *GeoDataFeature::abstractView()
{
    // FIXME: Calling detach() doesn't help at all because the m_abstractView
    // object isn't actually copied in the Private class as well.
    // detach();
    return d->featureExtendedData().m_abstractView;
}

void GeoDataFeature::setAbstractView( GeoDataAbstractView *abstractView )
{
    if (abstractView == nullptr && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    detach();
    d->featureExtendedData().m_abstractView = abstractView;
}

QString GeoDataFeature::styleUrl() const
{
    return d->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value )
{
    detach();

    d->m_styleUrl = value;

    if ( value.isEmpty() ) {
        d->m_style = GeoDataStyle::Ptr();
        return;
    }

    QString styleUrl = value;
    styleUrl.remove(QLatin1Char('#'));
    GeoDataObject *object = parent();
    bool found = false;
    while ( object && !found ) {
        if( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *doc = static_cast<GeoDataDocument*> ( object );
            GeoDataStyleMap &styleMap = doc->styleMap( styleUrl );
            const QString normalStyleUrl = styleMap.value(QStringLiteral("normal"));
            if (!normalStyleUrl.isEmpty()) {
                styleUrl = normalStyleUrl;
                styleUrl.remove(QLatin1Char('#'));
            }
            // Not calling setStyle here because we don't want
            // re-parenting of the style
            d->m_style = doc->style( styleUrl );
            found = true;
        }
        object = object->parent();
    }
}

bool GeoDataFeature::isVisible() const
{
    return d->m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    detach();
    d->m_visible = value;
}

bool GeoDataFeature::isGloballyVisible() const
{
    if ( parent() == 0 ) {
        return d->m_visible;
    }
    GeoDataContainer *container = static_cast<GeoDataContainer*>( parent() );
    return d->m_visible && container->isGloballyVisible();
}


const GeoDataTimeSpan &GeoDataFeature::timeSpan() const
{
    return d->featureExtendedData().m_timeSpan;
}

GeoDataTimeSpan &GeoDataFeature::timeSpan()
{
    detach();
    return d->featureExtendedData().m_timeSpan;
}

void GeoDataFeature::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    detach();
    d->featureExtendedData().m_timeSpan = timeSpan;
}

const GeoDataTimeStamp &GeoDataFeature::timeStamp() const
{
    return d->featureExtendedData().m_timeStamp;
}

GeoDataTimeStamp &GeoDataFeature::timeStamp()
{
    detach();
    return d->featureExtendedData().m_timeStamp;
}

void GeoDataFeature::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    detach();
    d->featureExtendedData().m_timeStamp = timeStamp;
}

const GeoDataExtendedData &GeoDataFeature::extendedData() const
{
    return d->m_extendedData;
}

GeoDataStyle::ConstPtr GeoDataFeature::style() const
{
    if (d->m_style) {
        return d->m_style;
    }

    return GeoDataFeaturePrivate::s_defaultStyle;
}

GeoDataStyle::ConstPtr GeoDataFeature::customStyle() const
{
    return d->m_style;
}

void GeoDataFeature::setStyle( const GeoDataStyle::Ptr &style )
{
    detach();
    if (style)
        style->setParent( this );
    d->m_style = style;
}

GeoDataExtendedData& GeoDataFeature::extendedData()
{
    detach();
    return d->m_extendedData;
}

void GeoDataFeature::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();
    d->m_extendedData = extendedData;
}

GeoDataRegion& GeoDataFeature::region() const
{
    // FIXME: Should call detach(). Maybe don't return reference.
    return d->featureExtendedData().m_region;
}

void GeoDataFeature::setRegion( const GeoDataRegion& region )
{
    detach();
    d->featureExtendedData().m_region = region;
}

GeoDataFeature::GeoDataVisualCategory GeoDataFeature::visualCategory() const
{
    return d->m_visualCategory;
}

void GeoDataFeature::setVisualCategory( GeoDataFeature::GeoDataVisualCategory index )
{
    detach();
    d->m_visualCategory = index;
}

const QString GeoDataFeature::role() const
{
    return d->m_role;
}

void GeoDataFeature::setRole( const QString &role )
{
    detach();
    d->m_role = role;
}

const GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    return d->m_styleMap;
}

void GeoDataFeature::setStyleMap( const GeoDataStyleMap* styleMap )
{
    d->m_styleMap = styleMap;
}

int GeoDataFeature::zoomLevel() const
{
    return d->m_zoomLevel;
}

void GeoDataFeature::setZoomLevel( int zoomLevel )
{
    detach();
    d->m_zoomLevel = zoomLevel;
}

qint64 GeoDataFeature::popularity() const
{
    return d->m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    detach();
    d->m_popularity = popularity;
}

QString GeoDataFeature::categoryName() const
{
    switch (d->m_visualCategory) {
    case Valley: return GeoDataFeaturePrivate::tr("Valley");
    case OtherTerrain: return GeoDataFeaturePrivate::tr("Terrain");
    case Crater: return GeoDataFeaturePrivate::tr("Crater");
    case Mare: return GeoDataFeaturePrivate::tr("Sea");
    case MannedLandingSite: return GeoDataFeaturePrivate::tr("Manned Landing Site");
    case RoboticRover: return GeoDataFeaturePrivate::tr("Robotic Rover");
    case UnmannedSoftLandingSite: return GeoDataFeaturePrivate::tr("Unmanned Soft Landing Site");
    case UnmannedHardLandingSite: return GeoDataFeaturePrivate::tr("Unmanned Hard Landing Site");
    case Mons: return GeoDataFeaturePrivate::tr("Mountain");
    case SmallCity: return GeoDataFeaturePrivate::tr("City");
    case SmallCountyCapital: return GeoDataFeaturePrivate::tr("County Capital");
    case SmallStateCapital: return GeoDataFeaturePrivate::tr("State Capital");
    case SmallNationCapital: return GeoDataFeaturePrivate::tr("Nation Capital");
    case MediumCity: return GeoDataFeaturePrivate::tr("City");
    case MediumCountyCapital: return GeoDataFeaturePrivate::tr("County Capital");
    case MediumStateCapital: return GeoDataFeaturePrivate::tr("State Capital");
    case MediumNationCapital: return GeoDataFeaturePrivate::tr("Nation Capital");
    case BigCity: return GeoDataFeaturePrivate::tr("City");
    case BigCountyCapital: return GeoDataFeaturePrivate::tr("County Capital");
    case BigStateCapital: return GeoDataFeaturePrivate::tr("State Capital");
    case BigNationCapital: return GeoDataFeaturePrivate::tr("Nation Capital");
    case LargeCity: return GeoDataFeaturePrivate::tr("City");
    case LargeCountyCapital: return GeoDataFeaturePrivate::tr("County Capital");
    case LargeStateCapital: return GeoDataFeaturePrivate::tr("State Capital");
    case LargeNationCapital: return GeoDataFeaturePrivate::tr("Nation Capital");
    case Nation: return GeoDataFeaturePrivate::tr("Nation");
    case Mountain: return GeoDataFeaturePrivate::tr("Mountain");
    case Volcano: return GeoDataFeaturePrivate::tr("Volcano");
    case Continent: return GeoDataFeaturePrivate::tr("Continent");
    case Ocean: return GeoDataFeaturePrivate::tr("Ocean");
    case GeographicPole: return GeoDataFeaturePrivate::tr("Geographic Pole");
    case MagneticPole: return GeoDataFeaturePrivate::tr("Magnetic Pole");
    case ShipWreck: return GeoDataFeaturePrivate::tr("Ship Wreck");
    case AirPort: return GeoDataFeaturePrivate::tr("Air Port");
    case Observatory: return GeoDataFeaturePrivate::tr("Observatory");
    case MilitaryDangerArea: return GeoDataFeaturePrivate::tr("Military Danger Area");
    case OsmSite: return GeoDataFeaturePrivate::tr("OSM Site");
    case Coordinate: return GeoDataFeaturePrivate::tr("Coordinate");
    case Folder: return GeoDataFeaturePrivate::tr("Folder");
    case Bookmark: return GeoDataFeaturePrivate::tr("Bookmark");
    case Satellite: return GeoDataFeaturePrivate::tr("Satellite");

    // OpenStreetMap categories
    case PlaceCity: return GeoDataFeaturePrivate::tr("City");
    case PlaceCityCapital: return GeoDataFeaturePrivate::tr("City Capital");
    case PlaceSuburb: return GeoDataFeaturePrivate::tr("Suburb");
    case PlaceHamlet: return GeoDataFeaturePrivate::tr("Hamlet");
    case PlaceLocality: return GeoDataFeaturePrivate::tr("Locality");
    case PlaceTown: return GeoDataFeaturePrivate::tr("Town");
    case PlaceTownCapital: return GeoDataFeaturePrivate::tr("Town Capital");
    case PlaceVillage: return GeoDataFeaturePrivate::tr("Village");
    case PlaceVillageCapital: return GeoDataFeaturePrivate::tr("Village Capital");
    case NaturalWater: return GeoDataFeaturePrivate::tr("Water");
    case NaturalReef: return GeoDataFeaturePrivate::tr("Reef");
    case NaturalWood: return GeoDataFeaturePrivate::tr("Wood");
    case NaturalBeach: return GeoDataFeaturePrivate::tr("Beach");
    case NaturalWetland: return GeoDataFeaturePrivate::tr("Wetland");
    case NaturalGlacier: return GeoDataFeaturePrivate::tr("Glacier");
    case NaturalIceShelf: return GeoDataFeaturePrivate::tr("Ice Shelf");
    case NaturalScrub: return GeoDataFeaturePrivate::tr("Scrub");
    case NaturalCliff: return GeoDataFeaturePrivate::tr("Cliff");
    case NaturalHeath: return GeoDataFeaturePrivate::tr("Heath");
    case HighwayTrafficSignals: return GeoDataFeaturePrivate::tr("Traffic Signals");
    case HighwaySteps: return GeoDataFeaturePrivate::tr("Steps");
    case HighwayUnknown: return GeoDataFeaturePrivate::tr("Unknown Road");
    case HighwayPath: return GeoDataFeaturePrivate::tr("Path");
    case HighwayFootway: return GeoDataFeaturePrivate::tr("Footway");
    case HighwayTrack: return GeoDataFeaturePrivate::tr("Track");
    case HighwayPedestrian: return GeoDataFeaturePrivate::tr("Footway");
    case HighwayCycleway: return GeoDataFeaturePrivate::tr("Cycleway");
    case HighwayService: return GeoDataFeaturePrivate::tr("Service Road");
    case HighwayRoad: return GeoDataFeaturePrivate::tr("Road");
    case HighwayResidential: return GeoDataFeaturePrivate::tr("Residential Road");
    case HighwayLivingStreet: return GeoDataFeaturePrivate::tr("Living Street");
    case HighwayUnclassified: return GeoDataFeaturePrivate::tr("Unclassified Road");
    case HighwayTertiaryLink: return GeoDataFeaturePrivate::tr("Tertiary Link Road");
    case HighwayTertiary: return GeoDataFeaturePrivate::tr("Tertiary Road");
    case HighwaySecondaryLink: return GeoDataFeaturePrivate::tr("Secondary Link Road");
    case HighwaySecondary: return GeoDataFeaturePrivate::tr("Secondary Road");
    case HighwayPrimaryLink: return GeoDataFeaturePrivate::tr("Primary Link Road");
    case HighwayPrimary: return GeoDataFeaturePrivate::tr("Primary Road");
    case HighwayTrunkLink: return GeoDataFeaturePrivate::tr("Trunk Link Road");
    case HighwayTrunk: return GeoDataFeaturePrivate::tr("Trunk Road");
    case HighwayMotorwayLink: return GeoDataFeaturePrivate::tr("Motorway Link Road");
    case HighwayMotorway: return GeoDataFeaturePrivate::tr("Motorway");
    case Building: return GeoDataFeaturePrivate::tr("Building");
    case AccomodationCamping: return GeoDataFeaturePrivate::tr("Camping");
    case AccomodationHostel: return GeoDataFeaturePrivate::tr("Hostel");
    case AccomodationHotel: return GeoDataFeaturePrivate::tr("Hotel");
    case AccomodationMotel: return GeoDataFeaturePrivate::tr("Motel");
    case AccomodationYouthHostel: return GeoDataFeaturePrivate::tr("Youth Hostel");
    case AccomodationGuestHouse: return GeoDataFeaturePrivate::tr("Guest House");
    case AmenityLibrary: return GeoDataFeaturePrivate::tr("Library");
    case AmenityKindergarten: return GeoDataFeaturePrivate::tr("Kindergarten");
    case EducationCollege: return GeoDataFeaturePrivate::tr("College");
    case EducationSchool: return GeoDataFeaturePrivate::tr("School");
    case EducationUniversity: return GeoDataFeaturePrivate::tr("University");
    case FoodBar: return GeoDataFeaturePrivate::tr("Bar");
    case FoodBiergarten: return GeoDataFeaturePrivate::tr("Biergarten");
    case FoodCafe: return GeoDataFeaturePrivate::tr("Cafe");
    case FoodFastFood: return GeoDataFeaturePrivate::tr("Fast Food");
    case FoodPub: return GeoDataFeaturePrivate::tr("Pub");
    case FoodRestaurant: return GeoDataFeaturePrivate::tr("Restaurant");
    case HealthDentist: return GeoDataFeaturePrivate::tr("Dentist");
    case HealthDoctors: return GeoDataFeaturePrivate::tr("Doctors");
    case HealthHospital: return GeoDataFeaturePrivate::tr("Hospital");
    case HealthPharmacy: return GeoDataFeaturePrivate::tr("Pharmacy");
    case HealthVeterinary: return GeoDataFeaturePrivate::tr("Veterinary");
    case MoneyAtm: return GeoDataFeaturePrivate::tr("ATM");
    case MoneyBank: return GeoDataFeaturePrivate::tr("Bank");
    case AmenityArchaeologicalSite: return GeoDataFeaturePrivate::tr("Archaeological Site");
    case AmenityEmbassy: return GeoDataFeaturePrivate::tr("Embassy");
    case AmenityEmergencyPhone: return GeoDataFeaturePrivate::tr("Emergency Phone");
    case AmenityWaterPark: return GeoDataFeaturePrivate::tr("Water Park");
    case AmenityCommunityCentre: return GeoDataFeaturePrivate::tr("Community Centre");
    case AmenityFountain: return GeoDataFeaturePrivate::tr("Fountain");
    case AmenityNightClub: return GeoDataFeaturePrivate::tr("Night Club");
    case AmenityBench: return GeoDataFeaturePrivate::tr("Bench");
    case AmenityCourtHouse: return GeoDataFeaturePrivate::tr("Court House");
    case AmenityFireStation: return GeoDataFeaturePrivate::tr("Fire Station");
    case AmenityHuntingStand: return GeoDataFeaturePrivate::tr("Hunting Stand");
    case AmenityPolice: return GeoDataFeaturePrivate::tr("Police");
    case AmenityPostBox: return GeoDataFeaturePrivate::tr("Post Box");
    case AmenityPostOffice: return GeoDataFeaturePrivate::tr("Post Office");
    case AmenityPrison: return GeoDataFeaturePrivate::tr("Prison");
    case AmenityRecycling: return GeoDataFeaturePrivate::tr("Recycling");
    case AmenityShelter: return GeoDataFeaturePrivate::tr("Shelter");
    case AmenityTelephone: return GeoDataFeaturePrivate::tr("Telephone");
    case AmenityToilets: return GeoDataFeaturePrivate::tr("Toilets");
    case AmenityTownHall: return GeoDataFeaturePrivate::tr("Town Hall");
    case AmenityWasteBasket: return GeoDataFeaturePrivate::tr("Waste Basket");
    case AmenityDrinkingWater: return GeoDataFeaturePrivate::tr("Drinking Water");
    case AmenityGraveyard: return GeoDataFeaturePrivate::tr("Graveyard");
    case BarrierCityWall: return GeoDataFeaturePrivate::tr("City Wall");
    case BarrierGate: return GeoDataFeaturePrivate::tr("Gate");
    case BarrierLiftGate: return GeoDataFeaturePrivate::tr("Lift Gate");
    case BarrierWall: return GeoDataFeaturePrivate::tr("Wall");
    case NaturalPeak: return GeoDataFeaturePrivate::tr("Peak");
    case NaturalTree: return GeoDataFeaturePrivate::tr("Tree");
    case ShopBeverages: return GeoDataFeaturePrivate::tr("Beverages");
    case ShopHifi: return GeoDataFeaturePrivate::tr("Hifi");
    case ShopSupermarket: return GeoDataFeaturePrivate::tr("Supermarket");
    case ShopAlcohol: return GeoDataFeaturePrivate::tr("Alcohol");
    case ShopBakery: return GeoDataFeaturePrivate::tr("Bakery");
    case ShopButcher: return GeoDataFeaturePrivate::tr("Butcher");
    case ShopConfectionery: return GeoDataFeaturePrivate::tr("Confectionery");
    case ShopConvenience: return GeoDataFeaturePrivate::tr("Convenience Shop");
    case ShopGreengrocer: return GeoDataFeaturePrivate::tr("Greengrocer");
    case ShopSeafood: return GeoDataFeaturePrivate::tr("Seafood");
    case ShopDepartmentStore: return GeoDataFeaturePrivate::tr("Department Store");
    case ShopKiosk: return GeoDataFeaturePrivate::tr("Kiosk");
    case ShopBag: return GeoDataFeaturePrivate::tr("Bag");
    case ShopClothes: return GeoDataFeaturePrivate::tr("Clothes");
    case ShopFashion: return GeoDataFeaturePrivate::tr("Fashion");
    case ShopJewelry: return GeoDataFeaturePrivate::tr("Jewelry");
    case ShopShoes: return GeoDataFeaturePrivate::tr("Shoes");
    case ShopVarietyStore: return GeoDataFeaturePrivate::tr("Variety Store");
    case ShopBeauty: return GeoDataFeaturePrivate::tr("Beauty");
    case ShopChemist: return GeoDataFeaturePrivate::tr("Chemist");
    case ShopCosmetics: return GeoDataFeaturePrivate::tr("Cosmetics");
    case ShopHairdresser: return GeoDataFeaturePrivate::tr("Hairdresser");
    case ShopOptician: return GeoDataFeaturePrivate::tr("Optician");
    case ShopPerfumery: return GeoDataFeaturePrivate::tr("Perfumery");
    case ShopDoitYourself: return GeoDataFeaturePrivate::tr("Doit Yourself");
    case ShopFlorist: return GeoDataFeaturePrivate::tr("Florist");
    case ShopHardware: return GeoDataFeaturePrivate::tr("Hardware");
    case ShopFurniture: return GeoDataFeaturePrivate::tr("Furniture");
    case ShopElectronics: return GeoDataFeaturePrivate::tr("Electronics");
    case ShopMobilePhone: return GeoDataFeaturePrivate::tr("Mobile Phone");
    case ShopBicycle: return GeoDataFeaturePrivate::tr("Bicycle");
    case ShopCar: return GeoDataFeaturePrivate::tr("Car");
    case ShopCarRepair: return GeoDataFeaturePrivate::tr("Car Repair");
    case ShopCarParts: return GeoDataFeaturePrivate::tr("Car Parts");
    case ShopMotorcycle: return GeoDataFeaturePrivate::tr("Motorcycle");
    case ShopOutdoor: return GeoDataFeaturePrivate::tr("Outdoor");
    case ShopMusicalInstrument: return GeoDataFeaturePrivate::tr("Musical Instrument");
    case ShopPhoto: return GeoDataFeaturePrivate::tr("Photo");
    case ShopBook: return GeoDataFeaturePrivate::tr("Book");
    case ShopGift: return GeoDataFeaturePrivate::tr("Gift");
    case ShopStationery: return GeoDataFeaturePrivate::tr("Stationery");
    case ShopLaundry: return GeoDataFeaturePrivate::tr("Laundry");
    case ShopPet: return GeoDataFeaturePrivate::tr("Pet");
    case ShopToys: return GeoDataFeaturePrivate::tr("Toys");
    case ShopTravelAgency: return GeoDataFeaturePrivate::tr("Travel Agency");
    case Shop: return GeoDataFeaturePrivate::tr("Shop");
    case ManmadeBridge: return GeoDataFeaturePrivate::tr("Bridge");
    case ManmadeLighthouse: return GeoDataFeaturePrivate::tr("Lighthouse");
    case ManmadePier: return GeoDataFeaturePrivate::tr("Pier");
    case ManmadeWaterTower: return GeoDataFeaturePrivate::tr("Water Tower");
    case ManmadeWindMill: return GeoDataFeaturePrivate::tr("Wind Mill");
    case TouristAttraction: return GeoDataFeaturePrivate::tr("Tourist Attraction");
    case TouristCastle: return GeoDataFeaturePrivate::tr("Castle");
    case TouristCinema: return GeoDataFeaturePrivate::tr("Cinema");
    case TouristInformation: return GeoDataFeaturePrivate::tr("Information");
    case TouristMonument: return GeoDataFeaturePrivate::tr("Monument");
    case TouristMuseum: return GeoDataFeaturePrivate::tr("Museum");
    case TouristRuin: return GeoDataFeaturePrivate::tr("Ruin");
    case TouristTheatre: return GeoDataFeaturePrivate::tr("Theatre");
    case TouristThemePark: return GeoDataFeaturePrivate::tr("Theme Park");
    case TouristViewPoint: return GeoDataFeaturePrivate::tr("View Point");
    case TouristZoo: return GeoDataFeaturePrivate::tr("Zoo");
    case TouristAlpineHut: return GeoDataFeaturePrivate::tr("Alpine Hut");
    case TransportAerodrome: return GeoDataFeaturePrivate::tr("Aerodrome");
    case TransportHelipad: return GeoDataFeaturePrivate::tr("Helipad");
    case TransportAirportGate: return GeoDataFeaturePrivate::tr("Airport Gate");
    case TransportAirportRunway: return GeoDataFeaturePrivate::tr("Airport Runway");
    case TransportAirportApron: return GeoDataFeaturePrivate::tr("Airport Apron");
    case TransportAirportTaxiway: return GeoDataFeaturePrivate::tr("Airport Taxiway");
    case TransportAirportTerminal: return GeoDataFeaturePrivate::tr("Airport Terminal");
    case TransportBusStation: return GeoDataFeaturePrivate::tr("Bus Station");
    case TransportBusStop: return GeoDataFeaturePrivate::tr("Bus Stop");
    case TransportCarShare: return GeoDataFeaturePrivate::tr("Car Sharing");
    case TransportFuel: return GeoDataFeaturePrivate::tr("Gas Station");
    case TransportParking: return GeoDataFeaturePrivate::tr("Parking");
    case TransportParkingSpace: return GeoDataFeaturePrivate::tr("Parking Space");
    case TransportPlatform: return GeoDataFeaturePrivate::tr("Platform");
    case TransportRentalBicycle: return GeoDataFeaturePrivate::tr("Rental Bicycle");
    case TransportRentalCar: return GeoDataFeaturePrivate::tr("Rental Car");
    case TransportTaxiRank: return GeoDataFeaturePrivate::tr("Taxi Rank");
    case TransportTrainStation: return GeoDataFeaturePrivate::tr("Train Station");
    case TransportTramStop: return GeoDataFeaturePrivate::tr("Tram Stop");
    case TransportBicycleParking: return GeoDataFeaturePrivate::tr("Bicycle Parking");
    case TransportMotorcycleParking: return GeoDataFeaturePrivate::tr("Motorcycle Parking");
    case TransportSubwayEntrance: return GeoDataFeaturePrivate::tr("Subway Entrance");
    case ReligionPlaceOfWorship: return GeoDataFeaturePrivate::tr("Place Of Worship");
    case ReligionBahai: return GeoDataFeaturePrivate::tr("Bahai");
    case ReligionBuddhist: return GeoDataFeaturePrivate::tr("Buddhist");
    case ReligionChristian: return GeoDataFeaturePrivate::tr("Christian");
    case ReligionMuslim: return GeoDataFeaturePrivate::tr("Muslim");
    case ReligionHindu: return GeoDataFeaturePrivate::tr("Hindu");
    case ReligionJain: return GeoDataFeaturePrivate::tr("Jain");
    case ReligionJewish: return GeoDataFeaturePrivate::tr("Jewish");
    case ReligionShinto: return GeoDataFeaturePrivate::tr("Shinto");
    case ReligionSikh: return GeoDataFeaturePrivate::tr("Sikh");
    case LeisureGolfCourse: return GeoDataFeaturePrivate::tr("Golf Course");
    case LeisureMarina: return GeoDataFeaturePrivate::tr("Marina");
    case LeisurePark: return GeoDataFeaturePrivate::tr("Park");
    case LeisurePlayground: return GeoDataFeaturePrivate::tr("Playground");
    case LeisurePitch: return GeoDataFeaturePrivate::tr("Pitch");
    case LeisureSportsCentre: return GeoDataFeaturePrivate::tr("Sports Centre");
    case LeisureStadium: return GeoDataFeaturePrivate::tr("Stadium");
    case LeisureTrack: return GeoDataFeaturePrivate::tr("Track");
    case LeisureSwimmingPool: return GeoDataFeaturePrivate::tr("Swimming Pool");
    case LanduseAllotments: return GeoDataFeaturePrivate::tr("Allotments");
    case LanduseBasin: return GeoDataFeaturePrivate::tr("Basin");
    case LanduseCemetery: return GeoDataFeaturePrivate::tr("Cemetery");
    case LanduseCommercial: return GeoDataFeaturePrivate::tr("Commercial");
    case LanduseConstruction: return GeoDataFeaturePrivate::tr("Construction");
    case LanduseFarmland: return GeoDataFeaturePrivate::tr("Farmland");
    case LanduseFarmyard: return GeoDataFeaturePrivate::tr("Farmyard");
    case LanduseGarages: return GeoDataFeaturePrivate::tr("Garages");
    case LanduseGrass: return GeoDataFeaturePrivate::tr("Grass");
    case LanduseIndustrial: return GeoDataFeaturePrivate::tr("Industrial");
    case LanduseLandfill: return GeoDataFeaturePrivate::tr("Landfill");
    case LanduseMeadow: return GeoDataFeaturePrivate::tr("Meadow");
    case LanduseMilitary: return GeoDataFeaturePrivate::tr("Military");
    case LanduseQuarry: return GeoDataFeaturePrivate::tr("Quarry");
    case LanduseRailway: return GeoDataFeaturePrivate::tr("Railway");
    case LanduseReservoir: return GeoDataFeaturePrivate::tr("Reservoir");
    case LanduseResidential: return GeoDataFeaturePrivate::tr("Residential");
    case LanduseRetail: return GeoDataFeaturePrivate::tr("Retail");
    case LanduseOrchard: return GeoDataFeaturePrivate::tr("Orchard");
    case LanduseVineyard: return GeoDataFeaturePrivate::tr("Vineyard");
    case RailwayRail: return GeoDataFeaturePrivate::tr("Rail");
    case RailwayNarrowGauge: return GeoDataFeaturePrivate::tr("Narrow Gauge");
    case RailwayTram: return GeoDataFeaturePrivate::tr("Tram");
    case RailwayLightRail: return GeoDataFeaturePrivate::tr("Light Rail");
    case RailwayAbandoned: return GeoDataFeaturePrivate::tr("Abandoned Railway");
    case RailwaySubway: return GeoDataFeaturePrivate::tr("Subway");
    case RailwayPreserved: return GeoDataFeaturePrivate::tr("Preserved Railway");
    case RailwayMiniature: return GeoDataFeaturePrivate::tr("Miniature Railway");
    case RailwayConstruction: return GeoDataFeaturePrivate::tr("Railway Construction");
    case RailwayMonorail: return GeoDataFeaturePrivate::tr("Monorail");
    case RailwayFunicular: return GeoDataFeaturePrivate::tr("Funicular Railway");
    case PowerTower: return GeoDataFeaturePrivate::tr("Power Tower");
    case AdminLevel1: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 1)");
    case AdminLevel2: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 2)");
    case AdminLevel3: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 3)");
    case AdminLevel4: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 4)");
    case AdminLevel5: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 5)");
    case AdminLevel6: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 6)");
    case AdminLevel7: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 7)");
    case AdminLevel8: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 8)");
    case AdminLevel9: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 9)");
    case AdminLevel10: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 10)");
    case AdminLevel11: return GeoDataFeaturePrivate::tr("Admin Boundary (Level 11)");
    case BoundaryMaritime: return GeoDataFeaturePrivate::tr("Boundary (Maritime)");
    case Landmass: return GeoDataFeaturePrivate::tr("Land Mass");
    case UrbanArea: return GeoDataFeaturePrivate::tr("Urban Area");
    case InternationalDateLine: return GeoDataFeaturePrivate::tr("International Date Line");
    case Bathymetry: return GeoDataFeaturePrivate::tr("Bathymetry");

    case Default:
    case Unknown:
    case None:
    case LastIndex: return QString();
    }

    return QString();
}

void GeoDataFeature::detach()
{
    if(d->ref.load() == 1) {
        return;
    }

    GeoDataFeaturePrivate* new_d = d->copy();

    if (!d->ref.deref()) {
        delete d;
    }

    d = new_d;

    d->ref.ref();
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_name;
    stream << d->featureExtendedData().m_address;
    stream << d->featureExtendedData().m_phoneNumber;
    stream << d->featureExtendedData().m_description;
    stream << d->m_visible;
//    stream << d->m_visualCategory;
    stream << d->m_role;
    stream << d->m_popularity;
    stream << d->m_zoomLevel;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    detach();
    GeoDataObject::unpack( stream );

    stream >> d->m_name;
    stream >> d->featureExtendedData().m_address;
    stream >> d->featureExtendedData().m_phoneNumber;
    stream >> d->featureExtendedData().m_description;
    stream >> d->m_visible;
//    stream >> (int)d->m_visualCategory;
    stream >> d->m_role;
    stream >> d->m_popularity;
    stream >> d->m_zoomLevel;
}

}
