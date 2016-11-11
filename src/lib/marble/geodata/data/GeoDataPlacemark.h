//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPLACEMARK_H
#define MARBLE_GEODATAPLACEMARK_H


#include <QDateTime>

#include "GeoDataCoordinates.h"
#include "GeoDataFeature.h"

#include "geodata_export.h"

class QXmlStreamWriter;

namespace Marble
{

class GeoDataPlacemarkPrivate;
class OsmPlacemarkData;
class GeoDataLookAt;
class GeoDataPolygon;
class GeoDataGeometry;

/**
 * @short a class representing a point of interest on the map
 *
 * This class represents a point of interest, e.g. a city or a
 * mountain.  It is filled with data by the KML or GPX loader and the
 * PlacemarkModel makes use of it.
 *
 * A Placemark can have an associated geometry which will be rendered to the map
 * along with the placemark's point icon. If you would like to render more than
 * one geometry for any one placemark than use @see setGeometry() to set add a
 * @see MultiGeometry.
 *
 * This is more or less only a GeoDataFeature with a geographic
 * position and a country code attached to it.  The country code is
 * not provided in a KML file.
 */

class GEODATA_EXPORT GeoDataPlacemark: public GeoDataFeature
{
 public:
    /**
     * Create a new placemark.
     */
    GeoDataPlacemark();

    /**
     * Create a new placemark from existing placemark @p placemark
     */
    GeoDataPlacemark( const GeoDataPlacemark& placemark );

    /**
     * Create a new placemark with the given @p name.
     */
    explicit GeoDataPlacemark( const QString &name );

    /**
    * Delete the placemark
    */
    ~GeoDataPlacemark();

    GeoDataPlacemark &operator=( const GeoDataPlacemark &other );

    /**
    * Equality operators.
    */
    bool operator==( const GeoDataPlacemark& other ) const;
    bool operator!=( const GeoDataPlacemark& other ) const;

    virtual const char* nodeType() const;

    GeoDataFeature * clone() const override;

    /**
     * @brief  A categorization of a placemark as defined by ...FIXME.
     * There is an additional osm tag mapping to GeoDataVisualCategory
     * in OsmPlacemarkData
     */
    enum GeoDataVisualCategory {
        None,
        Default,
        Unknown,

        // The order of the cities needs to stay fixed as the
        // algorithms rely on that.
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

        // Terrain
        Mountain,
        Volcano,
        Mons,                    // m
        Valley,                  // v
        Continent,
        Ocean,
        OtherTerrain,            // o

        // Space Terrain
        Crater,                  // c
        Mare,                    // a

        // Places of Interest
        GeographicPole,
        MagneticPole,
        ShipWreck,
        AirPort,
        Observatory,

        // Military
        MilitaryDangerArea,

        // Runners
        OsmSite,
        Coordinate,

        // Planets
        MannedLandingSite,       // h
        RoboticRover,            // r
        UnmannedSoftLandingSite, // u
        UnmannedHardLandingSite, // i

        Bookmark,

        Satellite,

        /*
         * Start of OpenStreetMap categories
         */

        PlaceCity,
        PlaceCityCapital,
        PlaceSuburb,
        PlaceHamlet,
        PlaceLocality,
        PlaceTown,
        PlaceTownCapital,
        PlaceVillage,
        PlaceVillageCapital,

        NaturalWater,
        NaturalReef,
        NaturalWood,
        NaturalBeach,
        NaturalWetland,
        NaturalGlacier,
        NaturalIceShelf,
        NaturalScrub,
        NaturalCliff,
        NaturalHeath,

        HighwayTrafficSignals,

        // OpenStreetMap highways
        HighwaySteps,
        HighwayUnknown,
        HighwayPath,
        HighwayFootway,
        HighwayTrack,
        HighwayPedestrian,
        HighwayCycleway,
        HighwayService,
        HighwayRoad,
        HighwayResidential,
        HighwayLivingStreet,
        HighwayUnclassified,
        HighwayTertiaryLink,
        HighwayTertiary,
        HighwaySecondaryLink,
        HighwaySecondary,
        HighwayPrimaryLink,
        HighwayPrimary,
        HighwayTrunkLink,
        HighwayTrunk,
        HighwayMotorwayLink,
        HighwayMotorway,

        //OSM building
        Building,

        // OpenStreetMap category Accomodation
        AccomodationCamping,
        AccomodationHostel,
        AccomodationHotel,
        AccomodationMotel,
        AccomodationYouthHostel,
        AccomodationGuestHouse,

        // OpenStreetMap category Amenity
        AmenityLibrary,
        AmenityKindergarten, ///< @since 0.26.0

        // OpenStreetMap category Education
        EducationCollege,
        EducationSchool,
        EducationUniversity,

        // OpenStreetMap category Food
        FoodBar,
        FoodBiergarten,
        FoodCafe,
        FoodFastFood,
        FoodPub,
        FoodRestaurant,

        // OpenStreetMap category Health
        HealthDentist,
        HealthDoctors,
        HealthHospital,
        HealthPharmacy,
        HealthVeterinary,

        // OpenStreetMap category Money
        MoneyAtm,
        MoneyBank,

        AmenityArchaeologicalSite,
        AmenityEmbassy,
        AmenityEmergencyPhone,
        AmenityWaterPark,
        AmenityCommunityCentre,
        AmenityFountain,
        AmenityNightClub,
        AmenityBench,
        AmenityCourtHouse,
        AmenityFireStation,
        AmenityHuntingStand,
        AmenityPolice,
        AmenityPostBox,
        AmenityPostOffice,
        AmenityPrison,
        AmenityRecycling,
        AmenityShelter, ///< @since 0.26.0
        AmenityTelephone,
        AmenityToilets,
        AmenityTownHall,
        AmenityWasteBasket,
        AmenityDrinkingWater,
        AmenityGraveyard,

        // OpenStreetMap category Barrier
        BarrierCityWall,
        BarrierGate,
        BarrierLiftGate,
        BarrierWall,

        NaturalPeak,
        NaturalTree,

        // OpenStreetMap category Shopping
        ShopBeverages,
        ShopHifi,
        ShopSupermarket,
        ShopAlcohol,
        ShopBakery,
        ShopButcher,
        ShopConfectionery,
        ShopConvenience,
        ShopGreengrocer,
        ShopSeafood,
        ShopDepartmentStore,
        ShopKiosk,
        ShopBag,
        ShopClothes,
        ShopFashion,
        ShopJewelry,
        ShopShoes,
        ShopVarietyStore,
        ShopBeauty,
        ShopChemist,
        ShopCosmetics,
        ShopHairdresser,
        ShopOptician,
        ShopPerfumery,
        ShopDoitYourself,
        ShopFlorist,
        ShopHardware,
        ShopFurniture,
        ShopElectronics,
        ShopMobilePhone,
        ShopBicycle,
        ShopCar,
        ShopCarRepair,
        ShopCarParts,
        ShopMotorcycle,
        ShopOutdoor,
        ShopMusicalInstrument,
        ShopPhoto,
        ShopBook,
        ShopGift,
        ShopStationery,
        ShopLaundry,
        ShopPet,
        ShopToys,
        ShopTravelAgency,
        Shop,

        ManmadeBridge,
        ManmadeLighthouse,
        ManmadePier,
        ManmadeWaterTower,
        ManmadeWindMill,


        // OpenStreetMap category Tourist
        TouristAttraction,
        TouristCastle,
        TouristCinema,
        TouristInformation,
        TouristMonument,
        TouristMuseum,
        TouristRuin,
        TouristTheatre,
        TouristThemePark,
        TouristViewPoint,
        TouristZoo,
        TouristAlpineHut,

        // OpenStreetMap category Transport
        TransportAerodrome,
        TransportHelipad,
        TransportAirportTerminal,
        TransportAirportGate, ///< @since 0.26.0
        TransportAirportRunway, ///< @since 0.26.0
        TransportAirportTaxiway, ///< @since 0.26.0
        TransportAirportApron, ///< @since 0.26.0
        TransportBusStation,
        TransportBusStop,
        TransportCarShare,
        TransportFuel,
        TransportParking,
        TransportParkingSpace,
        TransportPlatform,
        TransportRentalBicycle,
        TransportRentalCar,
        TransportTaxiRank,
        TransportTrainStation,
        TransportTramStop,
        TransportBicycleParking,
        TransportMotorcycleParking,
        TransportSubwayEntrance,

        // OpenStreetMap category religion
        ReligionPlaceOfWorship,
        ReligionBahai,
        ReligionBuddhist,
        ReligionChristian,
        ReligionMuslim,
        ReligionHindu,
        ReligionJain,
        ReligionJewish,
        ReligionShinto,
        ReligionSikh,

        // OpenStreetMap category Leisure
        LeisureGolfCourse,
        LeisureMarina, ///< @since 0.26.0
        LeisurePark,
        LeisurePlayground,
        LeisurePitch,
        LeisureSportsCentre,
        LeisureStadium,
        LeisureTrack,
        LeisureSwimmingPool,

        LanduseAllotments,
        LanduseBasin,
        LanduseCemetery,
        LanduseCommercial,
        LanduseConstruction,
        LanduseFarmland,
        LanduseFarmyard,
        LanduseGarages,
        LanduseGrass,
        LanduseIndustrial,
        LanduseLandfill,
        LanduseMeadow,
        LanduseMilitary,
        LanduseQuarry,
        LanduseRailway,
        LanduseReservoir,
        LanduseResidential,
        LanduseRetail,
        LanduseOrchard,
        LanduseVineyard,

        RailwayRail,
        RailwayNarrowGauge,
        RailwayTram,
        RailwayLightRail,
        RailwayAbandoned,
        RailwaySubway,
        RailwayPreserved,
        RailwayMiniature,
        RailwayConstruction,
        RailwayMonorail,
        RailwayFunicular,

        // OpenStreetMap category Power
        PowerTower,

        //Admin level tags for depicting boundary
        AdminLevel1,
        AdminLevel2,
        AdminLevel3,
        AdminLevel4,
        AdminLevel5,
        AdminLevel6,
        AdminLevel7,
        AdminLevel8,
        AdminLevel9,
        AdminLevel10,
        AdminLevel11,

        BoundaryMaritime,

        //Custom OSM Tags
        Landmass,
        UrbanArea,
        InternationalDateLine,
        Bathymetry, ///< @since 0.26.0

        // Important: Make sure that this is always the last
        // item and just use it to specify the array size
        LastIndex

    };

    /**
     * Return the symbol index of the placemark.
     */
    GeoDataVisualCategory visualCategory() const;

    /**
     * Sets the symbol @p index of the placemark.
     * @param  category  the new category to be used.
     */
    void setVisualCategory(GeoDataVisualCategory category);

    /**
     * Return the coordinates of the placemark at time @p dateTime as a GeoDataCoordinates
     *
     * The @p dateTime parameter should be used if the placemark geometry() is a
     * GeoDataTrack and thus contains several coordinates associated with a date and time.
     *
     * The @p iconAtCoordinates boolean is set to true if an icon should be drawn to
     * represent the placemark at these coordinates as described in
     * https://code.google.com/apis/kml/documentation/kmlreference.html#placemark,
     * it is set to false otherwise.
     *
     * @see GeoDataTrack::GeoDataTrack
     */
    GeoDataCoordinates coordinate( const QDateTime &dateTime = QDateTime(), bool *iconAtCoordinates = 0 ) const;

    /**
     * The geometry of the GeoDataPlacemark is to be rendered to the marble map
     * along with the icon at the coordinate associated with this Placemark.
     * @return a pointer to the current Geometry object
     */
    GeoDataGeometry* geometry();
    const GeoDataGeometry* geometry() const;

    /**
     * @brief displays the name of a place in the locale language of the user
     */
    QString displayName() const;

    /**
     * @since 0.26.0
     */
    QString categoryName() const;

    /**
     * Return the coordinates of the placemark as @p longitude,
     * @p latitude and @p altitude.
     */
    void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    /**
      * Quick, safe accessor to the placemark's OsmPlacemarkData stored within it's
      * ExtendedData. If the extendedData does not contain osmData, the function
      * inserts a default-constructed one, and returns a reference to it.
      */
    OsmPlacemarkData &osmData();
    const OsmPlacemarkData &osmData() const;

    void setOsmData( const OsmPlacemarkData &osmData );
    bool hasOsmData() const;
    /**
     * @since 0.26.0
     */
    void clearOsmData();

    /**
     * Set the coordinate of the placemark in @p longitude and
     * @p latitude.
     */
    void setCoordinate( qreal longitude, qreal latitude, qreal altitude = 0,
                        GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian );

    /**
    * Set the coordinate of the placemark with an @p GeoDataPoint.
    */
    void setCoordinate( const GeoDataCoordinates &coordinate );

    /**
     * Sets the current Geometry of this Placemark. @see geometry() and the class 
     * overview for description of the geometry concept. The geometry can be set 
     * to any @see GeoDataGeometry like @see GeoDataPoint,@see GeoDataLineString,
     * @see GeoDataLinearRing and @see GeoDataMultiGeometry
     */
    void setGeometry( GeoDataGeometry *entry );

    /**
     * Return the area size of the feature in square km.
     *
     * FIXME: Once we make Marble more area-aware we need to 
     * move this into the GeoDataArea class which will get
     * inherited from GeoDataPlacemark (or GeoDataFeature).
     */
    qreal area() const;

    /**
     * Set the area size of the feature in square km.
     */
    void setArea( qreal area );

    /**
     * Return the population of the placemark.
     */
    qint64 population() const;
    /**
     * Sets the @p population of the placemark.
     * @param  population  the new population value
     */
    void setPopulation( qint64 population );

    /**
     * Return the state of the placemark.
     */
    const QString state() const;

    /**
     * Set the state @p state of the placemark.
     */
    void setState( const QString &state );

    /**
     * Return the country code of the placemark.
     */
    const QString countryCode() const;

    /**
     * Set the country @p code of the placemark.
     */
    void setCountryCode( const QString &code );

    /**
     * Returns whether balloon is visible or not
     */
    bool isBalloonVisible() const;

    /**
     * Set visibility of the balloon
     */
    void setBalloonVisible( bool visible );

    /**
     * Serialize the Placemark to a data stream. This is a binary serialisation
     * and is deserialised using @see unpack()
     * @param stream the QDataStream to serialise object to.
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * Serialise this Placemark to a XML stream writer @see QXmlStreamWriter in
     * the Qt documentation for more info. This will output the XML
     * representation of this Placemark. The default XML format is KML, to have
     * other formats supported you need to create a subclass and override this
     * method.
     * @param stream the XML Stream Reader to output to.
     */
    virtual QXmlStreamWriter& pack( QXmlStreamWriter& stream ) const;

    virtual QXmlStreamWriter& operator <<( QXmlStreamWriter& stream ) const;

    /**
     * Deserialize the Placemark from a data stream. This has the opposite effect
     * from @see pack()
     * @param stream the QDataStream to deserialise from.
     */
    virtual void unpack( QDataStream& stream );

    /**
     * Returns GeoDataLookAt object if lookAt is setup earlier
     * otherwise It will convert GeoDataCoordinates of Placemark
     * to GeoDataLookAt with range equals to altitude of
     * GeoDataCoordinate
     */
    const GeoDataLookAt *lookAt() const;
    GeoDataLookAt *lookAt();

    static bool placemarkLayoutOrderCompare(const GeoDataPlacemark *a, const GeoDataPlacemark* b);

 private:
    Q_DECLARE_PRIVATE(GeoDataPlacemark)
};

}

#endif
