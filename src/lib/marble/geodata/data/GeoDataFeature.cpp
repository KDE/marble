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
#include <QPixmap>
#include <QHash>

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
#include "StyleBuilder.h"

namespace Marble
{
QFont GeoDataFeaturePrivate::s_defaultFont = QFont(QStringLiteral("Sans Serif"));
QColor GeoDataFeaturePrivate::s_defaultLabelColor = QColor( Qt::black );
StyleBuilder GeoDataFeaturePrivate::s_styleBuilder;

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


QSharedPointer<const GeoDataStyle> GeoDataFeature::presetStyle( GeoDataVisualCategory category )
{
    return GeoDataFeaturePrivate::s_styleBuilder.presetStyle(category);
}

QString GeoDataFeature::visualCategoryName(GeoDataFeature::GeoDataVisualCategory category)
{
    static QHash<GeoDataFeature::GeoDataVisualCategory, QString> s_visualCategoryNames;
    if (s_visualCategoryNames.isEmpty()) {
        s_visualCategoryNames[None] = "None";
        s_visualCategoryNames[Default] = "Default";
        s_visualCategoryNames[Unknown] = "Unknown";
        s_visualCategoryNames[SmallCity] = "SmallCity";
        s_visualCategoryNames[SmallCountyCapital] = "SmallCountyCapital";
        s_visualCategoryNames[SmallStateCapital] = "SmallStateCapital";
        s_visualCategoryNames[SmallNationCapital] = "SmallNationCapital";
        s_visualCategoryNames[MediumCity] = "MediumCity";
        s_visualCategoryNames[MediumCountyCapital] = "MediumCountyCapital";
        s_visualCategoryNames[MediumStateCapital] = "MediumStateCapital";
        s_visualCategoryNames[MediumNationCapital] = "MediumNationCapital";
        s_visualCategoryNames[BigCity] = "BigCity";
        s_visualCategoryNames[BigCountyCapital] = "BigCountyCapital";
        s_visualCategoryNames[BigStateCapital] = "BigStateCapital";
        s_visualCategoryNames[BigNationCapital] = "BigNationCapital";
        s_visualCategoryNames[LargeCity] = "LargeCity";
        s_visualCategoryNames[LargeCountyCapital] = "LargeCountyCapital";
        s_visualCategoryNames[LargeStateCapital] = "LargeStateCapital";
        s_visualCategoryNames[LargeNationCapital] = "LargeNationCapital";
        s_visualCategoryNames[Nation] = "Nation";
        s_visualCategoryNames[PlaceCity] = "PlaceCity";
        s_visualCategoryNames[PlaceSuburb] = "PlaceSuburb";
        s_visualCategoryNames[PlaceHamlet] = "PlaceHamlet";
        s_visualCategoryNames[PlaceLocality] = "PlaceLocality";
        s_visualCategoryNames[PlaceTown] = "PlaceTown";
        s_visualCategoryNames[PlaceVillage] = "PlaceVillage";
        s_visualCategoryNames[Mountain] = "Mountain";
        s_visualCategoryNames[Volcano] = "Volcano";
        s_visualCategoryNames[Mons] = "Mons";
        s_visualCategoryNames[Valley] = "Valley";
        s_visualCategoryNames[Continent] = "Continent";
        s_visualCategoryNames[Ocean] = "Ocean";
        s_visualCategoryNames[OtherTerrain] = "OtherTerrain";
        s_visualCategoryNames[Crater] = "Crater";
        s_visualCategoryNames[Mare] = "Mare";
        s_visualCategoryNames[GeographicPole] = "GeographicPole";
        s_visualCategoryNames[MagneticPole] = "MagneticPole";
        s_visualCategoryNames[ShipWreck] = "ShipWreck";
        s_visualCategoryNames[AirPort] = "AirPort";
        s_visualCategoryNames[Observatory] = "Observatory";
        s_visualCategoryNames[MilitaryDangerArea] = "MilitaryDangerArea";
        s_visualCategoryNames[Wikipedia] = "Wikipedia";
        s_visualCategoryNames[OsmSite] = "OsmSite";
        s_visualCategoryNames[Coordinate] = "Coordinate";
        s_visualCategoryNames[MannedLandingSite] = "MannedLandingSite";
        s_visualCategoryNames[RoboticRover] = "RoboticRover";
        s_visualCategoryNames[UnmannedSoftLandingSite] = "UnmannedSoftLandingSite";
        s_visualCategoryNames[UnmannedHardLandingSite] = "UnmannedHardLandingSite";
        s_visualCategoryNames[Folder] = "Folder";
        s_visualCategoryNames[Bookmark] = "Bookmark";
        s_visualCategoryNames[NaturalWater] = "NaturalWater";
        s_visualCategoryNames[NaturalReef] = "NaturalReef";
        s_visualCategoryNames[NaturalWood] = "NaturalWood";
        s_visualCategoryNames[NaturalBeach] = "NaturalBeach";
        s_visualCategoryNames[NaturalWetland] = "NaturalWetland";
        s_visualCategoryNames[NaturalGlacier] = "NaturalGlacier";
        s_visualCategoryNames[NaturalScrub] = "NaturalScrub";
        s_visualCategoryNames[NaturalCliff] = "NaturalCliff";
        s_visualCategoryNames[NaturalHeath] = "NaturalHeath";
        s_visualCategoryNames[HighwayTrafficSignals] = "HighwayTrafficSignals";
        s_visualCategoryNames[HighwaySteps] = "HighwaySteps";
        s_visualCategoryNames[HighwayUnknown] = "HighwayUnknown";
        s_visualCategoryNames[HighwayPath] = "HighwayPath";
        s_visualCategoryNames[HighwayFootway] = "HighwayFootway";
        s_visualCategoryNames[HighwayTrack] = "HighwayTrack";
        s_visualCategoryNames[HighwayPedestrian] = "HighwayPedestrian";
        s_visualCategoryNames[HighwayCycleway] = "HighwayCycleway";
        s_visualCategoryNames[HighwayService] = "HighwayService";
        s_visualCategoryNames[HighwayRoad] = "HighwayRoad";
        s_visualCategoryNames[HighwayResidential] = "HighwayResidential";
        s_visualCategoryNames[HighwayLivingStreet] = "HighwayLivingStreet";
        s_visualCategoryNames[HighwayUnclassified] = "HighwayUnclassified";
        s_visualCategoryNames[HighwayTertiaryLink] = "HighwayTertiaryLink";
        s_visualCategoryNames[HighwayTertiary] = "HighwayTertiary";
        s_visualCategoryNames[HighwaySecondaryLink] = "HighwaySecondaryLink";
        s_visualCategoryNames[HighwaySecondary] = "HighwaySecondary";
        s_visualCategoryNames[HighwayPrimaryLink] = "HighwayPrimaryLink";
        s_visualCategoryNames[HighwayPrimary] = "HighwayPrimary";
        s_visualCategoryNames[HighwayTrunkLink] = "HighwayTrunkLink";
        s_visualCategoryNames[HighwayTrunk] = "HighwayTrunk";
        s_visualCategoryNames[HighwayMotorwayLink] = "HighwayMotorwayLink";
        s_visualCategoryNames[HighwayMotorway] = "HighwayMotorway";
        s_visualCategoryNames[Building] = "Building";
        s_visualCategoryNames[AccomodationCamping] = "AccomodationCamping";
        s_visualCategoryNames[AccomodationHostel] = "AccomodationHostel";
        s_visualCategoryNames[AccomodationHotel] = "AccomodationHotel";
        s_visualCategoryNames[AccomodationMotel] = "AccomodationMotel";
        s_visualCategoryNames[AccomodationYouthHostel] = "AccomodationYouthHostel";
        s_visualCategoryNames[AccomodationGuestHouse] = "AccomodationGuestHouse";
        s_visualCategoryNames[AmenityLibrary] = "AmenityLibrary";
        s_visualCategoryNames[EducationCollege] = "EducationCollege";
        s_visualCategoryNames[EducationSchool] = "EducationSchool";
        s_visualCategoryNames[EducationUniversity] = "EducationUniversity";
        s_visualCategoryNames[FoodBar] = "FoodBar";
        s_visualCategoryNames[FoodBiergarten] = "FoodBiergarten";
        s_visualCategoryNames[FoodCafe] = "FoodCafe";
        s_visualCategoryNames[FoodFastFood] = "FoodFastFood";
        s_visualCategoryNames[FoodPub] = "FoodPub";
        s_visualCategoryNames[FoodRestaurant] = "FoodRestaurant";
        s_visualCategoryNames[HealthDentist] = "HealthDentist";
        s_visualCategoryNames[HealthDoctors] = "HealthDoctors";
        s_visualCategoryNames[HealthHospital] = "HealthHospital";
        s_visualCategoryNames[HealthPharmacy] = "HealthPharmacy";
        s_visualCategoryNames[HealthVeterinary] = "HealthVeterinary";
        s_visualCategoryNames[MoneyAtm] = "MoneyAtm";
        s_visualCategoryNames[MoneyBank] = "MoneyBank";
        s_visualCategoryNames[AmenityArchaeologicalSite] = "AmenityArchaeologicalSite";
        s_visualCategoryNames[AmenityEmbassy] = "AmenityEmbassy";
        s_visualCategoryNames[AmenityEmergencyPhone] = "AmenityEmergencyPhone";
        s_visualCategoryNames[AmenityWaterPark] = "AmenityWaterPark";
        s_visualCategoryNames[AmenityCommunityCentre] = "AmenityCommunityCentre";
        s_visualCategoryNames[AmenityFountain] = "AmenityFountain";
        s_visualCategoryNames[AmenityNightClub] = "AmenityNightClub";
        s_visualCategoryNames[AmenityBench] = "AmenityBench";
        s_visualCategoryNames[AmenityCourtHouse] = "AmenityCourtHouse";
        s_visualCategoryNames[AmenityFireStation] = "AmenityFireStation";
        s_visualCategoryNames[AmenityHuntingStand] = "AmenityHuntingStand";
        s_visualCategoryNames[AmenityPolice] = "AmenityPolice";
        s_visualCategoryNames[AmenityPostBox] = "AmenityPostBox";
        s_visualCategoryNames[AmenityPostOffice] = "AmenityPostOffice";
        s_visualCategoryNames[AmenityPrison] = "AmenityPrison";
        s_visualCategoryNames[AmenityRecycling] = "AmenityRecycling";
        s_visualCategoryNames[AmenityTelephone] = "AmenityTelephone";
        s_visualCategoryNames[AmenityToilets] = "AmenityToilets";
        s_visualCategoryNames[AmenityTownHall] = "AmenityTownHall";
        s_visualCategoryNames[AmenityWasteBasket] = "AmenityWasteBasket";
        s_visualCategoryNames[AmenityDrinkingWater] = "AmenityDrinkingWater";
        s_visualCategoryNames[AmenityGraveyard] = "AmenityGraveyard";
        s_visualCategoryNames[BarrierCityWall] = "BarrierCityWall";
        s_visualCategoryNames[BarrierGate] = "BarrierGate";
        s_visualCategoryNames[BarrierLiftGate] = "BarrierLiftGate";
        s_visualCategoryNames[BarrierWall] = "BarrierWall";
        s_visualCategoryNames[NaturalPeak] = "NaturalPeak";
        s_visualCategoryNames[NaturalTree] = "NaturalTree";
        s_visualCategoryNames[ShopBeverages] = "ShopBeverages";
        s_visualCategoryNames[ShopHifi] = "ShopHifi";
        s_visualCategoryNames[ShopSupermarket] = "ShopSupermarket";
        s_visualCategoryNames[ShopAlcohol] = "ShopAlcohol";
        s_visualCategoryNames[ShopBakery] = "ShopBakery";
        s_visualCategoryNames[ShopButcher] = "ShopButcher";
        s_visualCategoryNames[ShopConfectionery] = "ShopConfectionery";
        s_visualCategoryNames[ShopConvenience] = "ShopConvenience";
        s_visualCategoryNames[ShopGreengrocer] = "ShopGreengrocer";
        s_visualCategoryNames[ShopSeafood] = "ShopSeafood";
        s_visualCategoryNames[ShopDepartmentStore] = "ShopDepartmentStore";
        s_visualCategoryNames[ShopKiosk] = "ShopKiosk";
        s_visualCategoryNames[ShopBag] = "ShopBag";
        s_visualCategoryNames[ShopClothes] = "ShopClothes";
        s_visualCategoryNames[ShopFashion] = "ShopFashion";
        s_visualCategoryNames[ShopJewelry] = "ShopJewelry";
        s_visualCategoryNames[ShopShoes] = "ShopShoes";
        s_visualCategoryNames[ShopVarietyStore] = "ShopVarietyStore";
        s_visualCategoryNames[ShopBeauty] = "ShopBeauty";
        s_visualCategoryNames[ShopChemist] = "ShopChemist";
        s_visualCategoryNames[ShopCosmetics] = "ShopCosmetics";
        s_visualCategoryNames[ShopHairdresser] = "ShopHairdresser";
        s_visualCategoryNames[ShopOptician] = "ShopOptician";
        s_visualCategoryNames[ShopPerfumery] = "ShopPerfumery";
        s_visualCategoryNames[ShopDoitYourself] = "ShopDoitYourself";
        s_visualCategoryNames[ShopFlorist] = "ShopFlorist";
        s_visualCategoryNames[ShopHardware] = "ShopHardware";
        s_visualCategoryNames[ShopFurniture] = "ShopFurniture";
        s_visualCategoryNames[ShopElectronics] = "ShopElectronics";
        s_visualCategoryNames[ShopMobilePhone] = "ShopMobilePhone";
        s_visualCategoryNames[ShopBicycle] = "ShopBicycle";
        s_visualCategoryNames[ShopCar] = "ShopCar";
        s_visualCategoryNames[ShopCarRepair] = "ShopCarRepair";
        s_visualCategoryNames[ShopCarParts] = "ShopCarParts";
        s_visualCategoryNames[ShopMotorcycle] = "ShopMotorcycle";
        s_visualCategoryNames[ShopOutdoor] = "ShopOutdoor";
        s_visualCategoryNames[ShopMusicalInstrument] = "ShopMusicalInstrument";
        s_visualCategoryNames[ShopPhoto] = "ShopPhoto";
        s_visualCategoryNames[ShopBook] = "ShopBook";
        s_visualCategoryNames[ShopGift] = "ShopGift";
        s_visualCategoryNames[ShopStationery] = "ShopStationery";
        s_visualCategoryNames[ShopLaundry] = "ShopLaundry";
        s_visualCategoryNames[ShopPet] = "ShopPet";
        s_visualCategoryNames[ShopToys] = "ShopToys";
        s_visualCategoryNames[ShopTravelAgency] = "ShopTravelAgency";
        s_visualCategoryNames[Shop] = "Shop";
        s_visualCategoryNames[ManmadeBridge] = "ManmadeBridge";
        s_visualCategoryNames[ManmadeLighthouse] = "ManmadeLighthouse";
        s_visualCategoryNames[ManmadePier] = "ManmadePier";
        s_visualCategoryNames[ManmadeWaterTower] = "ManmadeWaterTower";
        s_visualCategoryNames[ManmadeWindMill] = "ManmadeWindMill";
        s_visualCategoryNames[TouristAttraction] = "TouristAttraction";
        s_visualCategoryNames[TouristCastle] = "TouristCastle";
        s_visualCategoryNames[TouristCinema] = "TouristCinema";
        s_visualCategoryNames[TouristInformation] = "TouristInformation";
        s_visualCategoryNames[TouristMonument] = "TouristMonument";
        s_visualCategoryNames[TouristMuseum] = "TouristMuseum";
        s_visualCategoryNames[TouristRuin] = "TouristRuin";
        s_visualCategoryNames[TouristTheatre] = "TouristTheatre";
        s_visualCategoryNames[TouristThemePark] = "TouristThemePark";
        s_visualCategoryNames[TouristViewPoint] = "TouristViewPoint";
        s_visualCategoryNames[TouristZoo] = "TouristZoo";
        s_visualCategoryNames[TouristAlpineHut] = "TouristAlpineHut";
        s_visualCategoryNames[TransportAerodrome] = "TransportAerodrome";
        s_visualCategoryNames[TransportHelipad] = "TransportHelipad";
        s_visualCategoryNames[TransportAirportTerminal] = "TransportAirportTerminal";
        s_visualCategoryNames[TransportBusStation] = "TransportBusStation";
        s_visualCategoryNames[TransportBusStop] = "TransportBusStop";
        s_visualCategoryNames[TransportCarShare] = "TransportCarShare";
        s_visualCategoryNames[TransportFuel] = "TransportFuel";
        s_visualCategoryNames[TransportParking] = "TransportParking";
        s_visualCategoryNames[TransportParkingSpace] = "TransportParkingSpace";
        s_visualCategoryNames[TransportPlatform] = "TransportPlatform";
        s_visualCategoryNames[TransportRentalBicycle] = "TransportRentalBicycle";
        s_visualCategoryNames[TransportRentalCar] = "TransportRentalCar";
        s_visualCategoryNames[TransportTaxiRank] = "TransportTaxiRank";
        s_visualCategoryNames[TransportTrainStation] = "TransportTrainStation";
        s_visualCategoryNames[TransportTramStop] = "TransportTramStop";
        s_visualCategoryNames[TransportBicycleParking] = "TransportBicycleParking";
        s_visualCategoryNames[TransportMotorcycleParking] = "TransportMotorcycleParking";
        s_visualCategoryNames[TransportSubwayEntrance] = "TransportSubwayEntrance";
        s_visualCategoryNames[ReligionPlaceOfWorship] = "ReligionPlaceOfWorship";
        s_visualCategoryNames[ReligionBahai] = "ReligionBahai";
        s_visualCategoryNames[ReligionBuddhist] = "ReligionBuddhist";
        s_visualCategoryNames[ReligionChristian] = "ReligionChristian";
        s_visualCategoryNames[ReligionMuslim] = "ReligionMuslim";
        s_visualCategoryNames[ReligionHindu] = "ReligionHindu";
        s_visualCategoryNames[ReligionJain] = "ReligionJain";
        s_visualCategoryNames[ReligionJewish] = "ReligionJewish";
        s_visualCategoryNames[ReligionShinto] = "ReligionShinto";
        s_visualCategoryNames[ReligionSikh] = "ReligionSikh";
        s_visualCategoryNames[LeisureGolfCourse] = "LeisureGolfCourse";
        s_visualCategoryNames[LeisurePark] = "LeisurePark";
        s_visualCategoryNames[LeisurePlayground] = "LeisurePlayground";
        s_visualCategoryNames[LeisurePitch] = "LeisurePitch";
        s_visualCategoryNames[LeisureSportsCentre] = "LeisureSportsCentre";
        s_visualCategoryNames[LeisureStadium] = "LeisureStadium";
        s_visualCategoryNames[LeisureTrack] = "LeisureTrack";
        s_visualCategoryNames[LeisureSwimmingPool] = "LeisureSwimmingPool";
        s_visualCategoryNames[LanduseAllotments] = "LanduseAllotments";
        s_visualCategoryNames[LanduseBasin] = "LanduseBasin";
        s_visualCategoryNames[LanduseCemetery] = "LanduseCemetery";
        s_visualCategoryNames[LanduseCommercial] = "LanduseCommercial";
        s_visualCategoryNames[LanduseConstruction] = "LanduseConstruction";
        s_visualCategoryNames[LanduseFarmland] = "LanduseFarmland";
        s_visualCategoryNames[LanduseFarmyard] = "LanduseFarmyard";
        s_visualCategoryNames[LanduseGarages] = "LanduseGarages";
        s_visualCategoryNames[LanduseGrass] = "LanduseGrass";
        s_visualCategoryNames[LanduseIndustrial] = "LanduseIndustrial";
        s_visualCategoryNames[LanduseLandfill] = "LanduseLandfill";
        s_visualCategoryNames[LanduseMeadow] = "LanduseMeadow";
        s_visualCategoryNames[LanduseMilitary] = "LanduseMilitary";
        s_visualCategoryNames[LanduseQuarry] = "LanduseQuarry";
        s_visualCategoryNames[LanduseRailway] = "LanduseRailway";
        s_visualCategoryNames[LanduseReservoir] = "LanduseReservoir";
        s_visualCategoryNames[LanduseResidential] = "LanduseResidential";
        s_visualCategoryNames[LanduseRetail] = "LanduseRetail";
        s_visualCategoryNames[LanduseOrchard] = "LanduseOrchard";
        s_visualCategoryNames[LanduseVineyard] = "LanduseVineyard";
        s_visualCategoryNames[RailwayRail] = "RailwayRail";
        s_visualCategoryNames[RailwayNarrowGauge] = "RailwayNarrowGauge";
        s_visualCategoryNames[RailwayTram] = "RailwayTram";
        s_visualCategoryNames[RailwayLightRail] = "RailwayLightRail";
        s_visualCategoryNames[RailwayAbandoned] = "RailwayAbandoned";
        s_visualCategoryNames[RailwaySubway] = "RailwaySubway";
        s_visualCategoryNames[RailwayPreserved] = "RailwayPreserved";
        s_visualCategoryNames[RailwayMiniature] = "RailwayMiniature";
        s_visualCategoryNames[RailwayConstruction] = "RailwayConstruction";
        s_visualCategoryNames[RailwayMonorail] = "RailwayMonorail";
        s_visualCategoryNames[RailwayFunicular] = "RailwayFunicular";
        s_visualCategoryNames[PowerTower] = "PowerTower";
        s_visualCategoryNames[Satellite] = "Satellite";
        s_visualCategoryNames[Landmass] = "Landmass";
        s_visualCategoryNames[AdminLevel1] = "AdminLevel1";
        s_visualCategoryNames[AdminLevel2] = "AdminLevel2";
        s_visualCategoryNames[AdminLevel3] = "AdminLevel3";
        s_visualCategoryNames[AdminLevel4] = "AdminLevel4";
        s_visualCategoryNames[AdminLevel5] = "AdminLevel5";
        s_visualCategoryNames[AdminLevel6] = "AdminLevel6";
        s_visualCategoryNames[AdminLevel7] = "AdminLevel7";
        s_visualCategoryNames[AdminLevel8] = "AdminLevel8";
        s_visualCategoryNames[AdminLevel9] = "AdminLevel9";
        s_visualCategoryNames[AdminLevel10] = "AdminLevel10";
        s_visualCategoryNames[AdminLevel11] = "AdminLevel11";
        s_visualCategoryNames[LastIndex] = "LastIndex";
    }

    Q_ASSERT(s_visualCategoryNames.contains(category));
    return s_visualCategoryNames[category];
}

QFont GeoDataFeature::defaultFont()
{
    return GeoDataFeaturePrivate::s_defaultFont;
}

void GeoDataFeature::setDefaultFont( const QFont& font )
{
    GeoDataFeaturePrivate::s_defaultFont = font;
    GeoDataFeaturePrivate::s_styleBuilder.reset();
}

QColor GeoDataFeature::defaultLabelColor()
{
    return GeoDataFeaturePrivate::s_defaultLabelColor;
}

void GeoDataFeature::setDefaultLabelColor( const QColor& color )
{
    GeoDataFeaturePrivate::s_defaultLabelColor = color;
    GeoDataFeaturePrivate::s_styleBuilder.reset();
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

GeoDataStyle::ConstPtr GeoDataFeature::style() const
{
    if (d->m_style) {
        return d->m_style;
    }
    return d->s_styleBuilder.createStyle(StyleParameters(this));
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

GeoDataExtendedData& GeoDataFeature::extendedData() const
{
    // FIXME: Should call detach(). Maybe don't return reference.
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

void GeoDataFeature::resetDefaultStyles()
{
    GeoDataFeaturePrivate::s_styleBuilder.reset();
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
