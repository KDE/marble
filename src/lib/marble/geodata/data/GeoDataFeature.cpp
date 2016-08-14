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
         p()->m_name != other.p()->m_name ||
         p()->m_snippet != other.p()->m_snippet ||
         p()->m_description != other.p()->m_description ||
         p()->m_descriptionCDATA != other.p()->m_descriptionCDATA ||
         p()->m_address != other.p()->m_address ||
         p()->m_phoneNumber != other.p()->m_phoneNumber ||
         p()->m_styleUrl != other.p()->m_styleUrl ||
         p()->m_popularity != other.p()->m_popularity ||
         p()->m_zoomLevel != other.p()->m_zoomLevel ||
         p()->m_visible != other.p()->m_visible ||
         p()->m_role != other.p()->m_role ||
         p()->m_extendedData != other.p()->m_extendedData ||
         p()->m_timeSpan != other.p()->m_timeSpan ||
         p()->m_timeStamp != other.p()->m_timeStamp ||
         p()->m_region != other.p()->m_region ||
         *style() != *other.style() ) {
        return false;
    }

    if ( (!p()->m_styleMap && other.p()->m_styleMap) ||
         (p()->m_styleMap && !other.p()->m_styleMap) ) {
        return false;
    }

    if ( (p()->m_styleMap && other.p()->m_styleMap) &&
         (*p()->m_styleMap != *other.p()->m_styleMap) ) {
        return false;
    }

    if ( !p()->m_abstractView && !other.p()->m_abstractView ) {
        return true;
    } else if ( (!p()->m_abstractView && other.p()->m_abstractView) ||
                (p()->m_abstractView && !other.p()->m_abstractView) ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() != other.p()->m_abstractView->nodeType() ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataCameraType ) {
        GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( p()->m_abstractView );
        GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.p()->m_abstractView );
        Q_ASSERT(thisCam && otherCam);

        if ( *thisCam != *otherCam ) {
            return false;
        }
    } else if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
        GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( p()->m_abstractView );
        GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.p()->m_abstractView );
        Q_ASSERT(thisLookAt && otherLookAt);

        if ( *thisLookAt != *otherLookAt ) {
            return false;
        }
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
    return d->m_snippet;
}

void GeoDataFeature::setSnippet( const GeoDataSnippet &snippet )
{
    detach();
    d->m_snippet = snippet;
}

QString GeoDataFeature::address() const
{
    return d->m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    detach();
    d->m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return d->m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    detach();
    d->m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return d->m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    detach();
    d->m_description = value;
}

bool GeoDataFeature::descriptionIsCDATA() const
{
    return d->m_descriptionCDATA;
}

void GeoDataFeature::setDescriptionCDATA( bool cdata )
{
    detach();
    d->m_descriptionCDATA = cdata;
}

const GeoDataAbstractView* GeoDataFeature::abstractView() const
{
    return d->m_abstractView;
}

GeoDataAbstractView *GeoDataFeature::abstractView()
{
    // FIXME: Calling detach() doesn't help at all because the m_abstractView
    // object isn't actually copied in the Private class as well.
    // detach();
    return d->m_abstractView;
}

void GeoDataFeature::setAbstractView( GeoDataAbstractView *abstractView )
{
    detach();
    d->m_abstractView = abstractView;
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
    styleUrl.remove('#');
    GeoDataObject *object = parent();
    bool found = false;
    while ( object && !found ) {
        if( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *doc = static_cast<GeoDataDocument*> ( object );
            GeoDataStyleMap &styleMap = doc->styleMap( styleUrl );
            if( !styleMap.value( QString( "normal" ) ).isEmpty() ) {
                styleUrl = styleMap.value( QString( "normal" ) );
                styleUrl.remove('#');
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
    return d->m_timeSpan;
}

GeoDataTimeSpan &GeoDataFeature::timeSpan()
{
    detach();
    return d->m_timeSpan;
}

void GeoDataFeature::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    detach();
    d->m_timeSpan = timeSpan;
}

const GeoDataTimeStamp &GeoDataFeature::timeStamp() const
{
    return d->m_timeStamp;
}

GeoDataTimeStamp &GeoDataFeature::timeStamp()
{
    detach();
    return d->m_timeStamp;
}

void GeoDataFeature::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    detach();
    d->m_timeStamp = timeStamp;
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
    return d->m_region;
}

void GeoDataFeature::setRegion( const GeoDataRegion& region )
{
    detach();
    d->m_region = region;
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
    case Valley: return QObject::tr("Valley");
    case OtherTerrain: return QObject::tr("Terrain");
    case Crater: return QObject::tr("Crater");
    case Mare: return QObject::tr("Sea");
    case MannedLandingSite: return QObject::tr("Manned Landing Site");
    case RoboticRover: return QObject::tr("Robotic Rover");
    case UnmannedSoftLandingSite: return QObject::tr("Unmanned Soft Landing Site");
    case UnmannedHardLandingSite: return QObject::tr("Unmanned Hard Landing Site");
    case Mons: return QObject::tr("Mountain");
    case SmallCity: return QObject::tr("City");
    case SmallCountyCapital: return QObject::tr("County Capital");
    case SmallStateCapital: return QObject::tr("State Capital");
    case SmallNationCapital: return QObject::tr("Nation Capital");
    case MediumCity: return QObject::tr("City");
    case MediumCountyCapital: return QObject::tr("County Capital");
    case MediumStateCapital: return QObject::tr("State Capital");
    case MediumNationCapital: return QObject::tr("Nation Capital");
    case BigCity: return QObject::tr("City");
    case BigCountyCapital: return QObject::tr("County Capital");
    case BigStateCapital: return QObject::tr("State Capital");
    case BigNationCapital: return QObject::tr("Nation Capital");
    case LargeCity: return QObject::tr("City");
    case LargeCountyCapital: return QObject::tr("County Capital");
    case LargeStateCapital: return QObject::tr("State Capital");
    case LargeNationCapital: return QObject::tr("Nation Capital");
    case Nation: return QObject::tr("Nation");
    case Mountain: return QObject::tr("Mountain");
    case Volcano: return QObject::tr("Volcano");
    case Continent: return QObject::tr("Continent");
    case Ocean: return QObject::tr("Ocean");
    case GeographicPole: return QObject::tr("Geographic Pole");
    case MagneticPole: return QObject::tr("Magnetic Pole");
    case ShipWreck: return QObject::tr("Ship Wreck");
    case AirPort: return QObject::tr("Air Port");
    case Observatory: return QObject::tr("Observatory");
    case MilitaryDangerArea: return QObject::tr("Military Danger Area");
    case OsmSite: return QObject::tr("OSM Site");
    case Coordinate: return QObject::tr("Coordinate");
    case Folder: return QObject::tr("Folder");
    case Bookmark: return QObject::tr("Bookmark");
    case Satellite: return QObject::tr("Satellite");

    // OpenStreetMap categories
    case PlaceCity: return QObject::tr("City");
    case PlaceSuburb: return QObject::tr("Suburb");
    case PlaceHamlet: return QObject::tr("Hamlet");
    case PlaceLocality: return QObject::tr("Locality");
    case PlaceTown: return QObject::tr("Town");
    case PlaceVillage: return QObject::tr("Village");
    case NaturalWater: return QObject::tr("Water");
    case NaturalReef: return QObject::tr("Reef");
    case NaturalWood: return QObject::tr("Wood");
    case NaturalBeach: return QObject::tr("Beach");
    case NaturalWetland: return QObject::tr("Wetland");
    case NaturalGlacier: return QObject::tr("Glacier");
    case NaturalIceShelf: return QObject::tr("Ice Shelf");
    case NaturalScrub: return QObject::tr("Scrub");
    case NaturalCliff: return QObject::tr("Cliff");
    case NaturalHeath: return QObject::tr("Heath");
    case HighwayTrafficSignals: return QObject::tr("Traffic Signals");
    case HighwaySteps: return QObject::tr("Steps");
    case HighwayUnknown: return QObject::tr("Unknown Road");
    case HighwayPath: return QObject::tr("Path");
    case HighwayFootway: return QObject::tr("Footway");
    case HighwayTrack: return QObject::tr("Track");
    case HighwayPedestrian: return QObject::tr("Footway");
    case HighwayCycleway: return QObject::tr("Cycleway");
    case HighwayService: return QObject::tr("Service Road");
    case HighwayRoad: return QObject::tr("Road");
    case HighwayResidential: return QObject::tr("Residential Road");
    case HighwayLivingStreet: return QObject::tr("Living Street");
    case HighwayUnclassified: return QObject::tr("Unclassified Road");
    case HighwayTertiaryLink: return QObject::tr("Tertiary Link Road");
    case HighwayTertiary: return QObject::tr("Tertiary Road");
    case HighwaySecondaryLink: return QObject::tr("Secondary Link Road");
    case HighwaySecondary: return QObject::tr("Secondary Road");
    case HighwayPrimaryLink: return QObject::tr("Primary Link Road");
    case HighwayPrimary: return QObject::tr("Primary Road");
    case HighwayTrunkLink: return QObject::tr("Trunk Link Road");
    case HighwayTrunk: return QObject::tr("Trunk Road");
    case HighwayMotorwayLink: return QObject::tr("Motorway Link Road");
    case HighwayMotorway: return QObject::tr("Motorway");
    case Building: return QObject::tr("Building");
    case AccomodationCamping: return QObject::tr("Camping");
    case AccomodationHostel: return QObject::tr("Hostel");
    case AccomodationHotel: return QObject::tr("Hotel");
    case AccomodationMotel: return QObject::tr("Motel");
    case AccomodationYouthHostel: return QObject::tr("Youth Hostel");
    case AccomodationGuestHouse: return QObject::tr("Guest House");
    case AmenityLibrary: return QObject::tr("Library");
    case AmenityKindergarten: return QObject::tr("Kindergarten");
    case EducationCollege: return QObject::tr("College");
    case EducationSchool: return QObject::tr("School");
    case EducationUniversity: return QObject::tr("University");
    case FoodBar: return QObject::tr("Bar");
    case FoodBiergarten: return QObject::tr("Biergarten");
    case FoodCafe: return QObject::tr("Cafe");
    case FoodFastFood: return QObject::tr("Fast Food");
    case FoodPub: return QObject::tr("Pub");
    case FoodRestaurant: return QObject::tr("Restaurant");
    case HealthDentist: return QObject::tr("Dentist");
    case HealthDoctors: return QObject::tr("Doctors");
    case HealthHospital: return QObject::tr("Hospital");
    case HealthPharmacy: return QObject::tr("Pharmacy");
    case HealthVeterinary: return QObject::tr("Veterinary");
    case MoneyAtm: return QObject::tr("ATM");
    case MoneyBank: return QObject::tr("Bank");
    case AmenityArchaeologicalSite: return QObject::tr("Archaeological Site");
    case AmenityEmbassy: return QObject::tr("Embassy");
    case AmenityEmergencyPhone: return QObject::tr("Emergency Phone");
    case AmenityWaterPark: return QObject::tr("Water Park");
    case AmenityCommunityCentre: return QObject::tr("Community Centre");
    case AmenityFountain: return QObject::tr("Fountain");
    case AmenityNightClub: return QObject::tr("Night Club");
    case AmenityBench: return QObject::tr("Bench");
    case AmenityCourtHouse: return QObject::tr("Court House");
    case AmenityFireStation: return QObject::tr("Fire Station");
    case AmenityHuntingStand: return QObject::tr("Hunting Stand");
    case AmenityPolice: return QObject::tr("Police");
    case AmenityPostBox: return QObject::tr("Post Box");
    case AmenityPostOffice: return QObject::tr("Post Office");
    case AmenityPrison: return QObject::tr("Prison");
    case AmenityRecycling: return QObject::tr("Recycling");
    case AmenityShelter: return QObject::tr("Shelter");
    case AmenityTelephone: return QObject::tr("Telephone");
    case AmenityToilets: return QObject::tr("Toilets");
    case AmenityTownHall: return QObject::tr("Town Hall");
    case AmenityWasteBasket: return QObject::tr("Waste Basket");
    case AmenityDrinkingWater: return QObject::tr("Drinking Water");
    case AmenityGraveyard: return QObject::tr("Graveyard");
    case BarrierCityWall: return QObject::tr("City Wall");
    case BarrierGate: return QObject::tr("Gate");
    case BarrierLiftGate: return QObject::tr("Lift Gate");
    case BarrierWall: return QObject::tr("Wall");
    case NaturalPeak: return QObject::tr("Peak");
    case NaturalTree: return QObject::tr("Tree");
    case ShopBeverages: return QObject::tr("Beverages");
    case ShopHifi: return QObject::tr("Hifi");
    case ShopSupermarket: return QObject::tr("Supermarket");
    case ShopAlcohol: return QObject::tr("Alcohol");
    case ShopBakery: return QObject::tr("Bakery");
    case ShopButcher: return QObject::tr("Butcher");
    case ShopConfectionery: return QObject::tr("Confectionery");
    case ShopConvenience: return QObject::tr("Convenience Shop");
    case ShopGreengrocer: return QObject::tr("Greengrocer");
    case ShopSeafood: return QObject::tr("Seafood");
    case ShopDepartmentStore: return QObject::tr("Department Store");
    case ShopKiosk: return QObject::tr("Kiosk");
    case ShopBag: return QObject::tr("Bag");
    case ShopClothes: return QObject::tr("Clothes");
    case ShopFashion: return QObject::tr("Fashion");
    case ShopJewelry: return QObject::tr("Jewelry");
    case ShopShoes: return QObject::tr("Shoes");
    case ShopVarietyStore: return QObject::tr("Variety Store");
    case ShopBeauty: return QObject::tr("Beauty");
    case ShopChemist: return QObject::tr("Chemist");
    case ShopCosmetics: return QObject::tr("Cosmetics");
    case ShopHairdresser: return QObject::tr("Hairdresser");
    case ShopOptician: return QObject::tr("Optician");
    case ShopPerfumery: return QObject::tr("Perfumery");
    case ShopDoitYourself: return QObject::tr("Doit Yourself");
    case ShopFlorist: return QObject::tr("Florist");
    case ShopHardware: return QObject::tr("Hardware");
    case ShopFurniture: return QObject::tr("Furniture");
    case ShopElectronics: return QObject::tr("Electronics");
    case ShopMobilePhone: return QObject::tr("Mobile Phone");
    case ShopBicycle: return QObject::tr("Bicycle");
    case ShopCar: return QObject::tr("Car");
    case ShopCarRepair: return QObject::tr("Car Repair");
    case ShopCarParts: return QObject::tr("Car Parts");
    case ShopMotorcycle: return QObject::tr("Motorcycle");
    case ShopOutdoor: return QObject::tr("Outdoor");
    case ShopMusicalInstrument: return QObject::tr("Musical Instrument");
    case ShopPhoto: return QObject::tr("Photo");
    case ShopBook: return QObject::tr("Book");
    case ShopGift: return QObject::tr("Gift");
    case ShopStationery: return QObject::tr("Stationery");
    case ShopLaundry: return QObject::tr("Laundry");
    case ShopPet: return QObject::tr("Pet");
    case ShopToys: return QObject::tr("Toys");
    case ShopTravelAgency: return QObject::tr("Travel Agency");
    case Shop: return QObject::tr("Shop");
    case ManmadeBridge: return QObject::tr("Bridge");
    case ManmadeLighthouse: return QObject::tr("Lighthouse");
    case ManmadePier: return QObject::tr("Pier");
    case ManmadeWaterTower: return QObject::tr("Water Tower");
    case ManmadeWindMill: return QObject::tr("Wind Mill");
    case TouristAttraction: return QObject::tr("Tourist Attraction");
    case TouristCastle: return QObject::tr("Castle");
    case TouristCinema: return QObject::tr("Cinema");
    case TouristInformation: return QObject::tr("Information");
    case TouristMonument: return QObject::tr("Monument");
    case TouristMuseum: return QObject::tr("Museum");
    case TouristRuin: return QObject::tr("Ruin");
    case TouristTheatre: return QObject::tr("Theatre");
    case TouristThemePark: return QObject::tr("Theme Park");
    case TouristViewPoint: return QObject::tr("View Point");
    case TouristZoo: return QObject::tr("Zoo");
    case TouristAlpineHut: return QObject::tr("Alpine Hut");
    case TransportAerodrome: return QObject::tr("Aerodrome");
    case TransportHelipad: return QObject::tr("Helipad");
    case TransportAirportGate: return QObject::tr("Airport Gate");
    case TransportAirportRunway: return QObject::tr("Airport Runway");
    case TransportAirportApron: return QObject::tr("Airport Apron");
    case TransportAirportTaxiway: return QObject::tr("Airport Taxiway");
    case TransportAirportTerminal: return QObject::tr("Airport Terminal");
    case TransportBusStation: return QObject::tr("Bus Station");
    case TransportBusStop: return QObject::tr("Bus Stop");
    case TransportCarShare: return QObject::tr("Car Sharing");
    case TransportFuel: return QObject::tr("Gas Station");
    case TransportParking: return QObject::tr("Parking");
    case TransportParkingSpace: return QObject::tr("Parking Space");
    case TransportPlatform: return QObject::tr("Platform");
    case TransportRentalBicycle: return QObject::tr("Rental Bicycle");
    case TransportRentalCar: return QObject::tr("Rental Car");
    case TransportTaxiRank: return QObject::tr("Taxi Rank");
    case TransportTrainStation: return QObject::tr("Train Station");
    case TransportTramStop: return QObject::tr("Tram Stop");
    case TransportBicycleParking: return QObject::tr("Bicycle Parking");
    case TransportMotorcycleParking: return QObject::tr("Motorcycle Parking");
    case TransportSubwayEntrance: return QObject::tr("Subway Entrance");
    case ReligionPlaceOfWorship: return QObject::tr("Place Of Worship");
    case ReligionBahai: return QObject::tr("Bahai");
    case ReligionBuddhist: return QObject::tr("Buddhist");
    case ReligionChristian: return QObject::tr("Christian");
    case ReligionMuslim: return QObject::tr("Muslim");
    case ReligionHindu: return QObject::tr("Hindu");
    case ReligionJain: return QObject::tr("Jain");
    case ReligionJewish: return QObject::tr("Jewish");
    case ReligionShinto: return QObject::tr("Shinto");
    case ReligionSikh: return QObject::tr("Sikh");
    case LeisureGolfCourse: return QObject::tr("Golf Course");
    case LeisureMarina: return QObject::tr("Marina");
    case LeisurePark: return QObject::tr("Park");
    case LeisurePlayground: return QObject::tr("Playground");
    case LeisurePitch: return QObject::tr("Pitch");
    case LeisureSportsCentre: return QObject::tr("Sports Centre");
    case LeisureStadium: return QObject::tr("Stadium");
    case LeisureTrack: return QObject::tr("Track");
    case LeisureSwimmingPool: return QObject::tr("Swimming Pool");
    case LanduseAllotments: return QObject::tr("Allotments");
    case LanduseBasin: return QObject::tr("Basin");
    case LanduseCemetery: return QObject::tr("Cemetery");
    case LanduseCommercial: return QObject::tr("Commercial");
    case LanduseConstruction: return QObject::tr("Construction");
    case LanduseFarmland: return QObject::tr("Farmland");
    case LanduseFarmyard: return QObject::tr("Farmyard");
    case LanduseGarages: return QObject::tr("Garages");
    case LanduseGrass: return QObject::tr("Grass");
    case LanduseIndustrial: return QObject::tr("Industrial");
    case LanduseLandfill: return QObject::tr("Landfill");
    case LanduseMeadow: return QObject::tr("Meadow");
    case LanduseMilitary: return QObject::tr("Military");
    case LanduseQuarry: return QObject::tr("Quarry");
    case LanduseRailway: return QObject::tr("Railway");
    case LanduseReservoir: return QObject::tr("Reservoir");
    case LanduseResidential: return QObject::tr("Residential");
    case LanduseRetail: return QObject::tr("Retail");
    case LanduseOrchard: return QObject::tr("Orchard");
    case LanduseVineyard: return QObject::tr("Vineyard");
    case RailwayRail: return QObject::tr("Rail");
    case RailwayNarrowGauge: return QObject::tr("Narrow Gauge");
    case RailwayTram: return QObject::tr("Tram");
    case RailwayLightRail: return QObject::tr("Light Rail");
    case RailwayAbandoned: return QObject::tr("Abandoned Railway");
    case RailwaySubway: return QObject::tr("Subway");
    case RailwayPreserved: return QObject::tr("Preserved Railway");
    case RailwayMiniature: return QObject::tr("Miniature Railway");
    case RailwayConstruction: return QObject::tr("Railway Construction");
    case RailwayMonorail: return QObject::tr("Monorail");
    case RailwayFunicular: return QObject::tr("Funicular Railway");
    case PowerTower: return QObject::tr("Power Tower");
    case AdminLevel1: return QObject::tr("Admin Boundary (Level 1)");
    case AdminLevel2: return QObject::tr("Admin Boundary (Level 2)");
    case AdminLevel3: return QObject::tr("Admin Boundary (Level 3)");
    case AdminLevel4: return QObject::tr("Admin Boundary (Level 4)");
    case AdminLevel5: return QObject::tr("Admin Boundary (Level 5)");
    case AdminLevel6: return QObject::tr("Admin Boundary (Level 6)");
    case AdminLevel7: return QObject::tr("Admin Boundary (Level 7)");
    case AdminLevel8: return QObject::tr("Admin Boundary (Level 8)");
    case AdminLevel9: return QObject::tr("Admin Boundary (Level 9)");
    case AdminLevel10: return QObject::tr("Admin Boundary (Level 10)");
    case AdminLevel11: return QObject::tr("Admin Boundary (Level 11)");
    case BoundaryMaritime: return QObject::tr("Boundary (Maritime)");
    case Landmass: return QObject::tr("Land Mass");
    case UrbanArea: return QObject::tr("Urban Area");
    case InternationalDateLine: return QObject::tr("International Date Line");
    case Bathymetry: return QObject::tr("Bathymetry");

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
    stream << d->m_address;
    stream << d->m_phoneNumber;
    stream << d->m_description;
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
    stream >> d->m_address;
    stream >> d->m_phoneNumber;
    stream >> d->m_description;
    stream >> d->m_visible;
//    stream >> (int)d->m_visualCategory;
    stream >> d->m_role;
    stream >> d->m_popularity;
    stream >> d->m_zoomLevel;
}

}
