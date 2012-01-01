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


#ifndef MARBLE_GEODATAFEATURE_H
#define MARBLE_GEODATAFEATURE_H


#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QColor>

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

// forward define all features we can find.
class GeoDataContainer;
class GeoDataFolder;
class GeoDataDocument;
class GeoDataPlacemark;
class GeoDataRegion;
class GeoDataAbstractView;

class GeoDataStyle;
class GeoDataStyleMap;

class GeoDataExtendedData;

class GeoDataTimeSpan;
class GeoDataTimeStamp;

class GeoDataFeaturePrivate;

/**
 * @short A base class for all geodata features
 *
 * GeoDataFeature is the base class for most geodata classes that
 * correspond to places on a map. It is never instantiated by itself,
 * but is always used as part of a derived class.
 *
 * @see GeoDataPlacemark
 * @see GeoDataContainer
 */

// FIXME: Later also add NetworkLink and Overlay

class GEODATA_EXPORT GeoDataFeature : public GeoDataObject
{
    friend class GeoDataContainer;
    friend class GeoDataFolder;
    friend class GeoDataDocument;
    friend class GeoDataPlacemark;
 public:
    GeoDataFeature();
    /// Create a new GeoDataFeature with @p name as its name.
    GeoDataFeature( const QString& name );

    GeoDataFeature( const GeoDataFeature& other );

    virtual ~GeoDataFeature();

    GeoDataFeature& operator=( const GeoDataFeature& other );
    bool operator==( const GeoDataFeature& ) const { return false; };

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

    EnumFeatureId featureId() const;
    /**
     * @brief  A categorization of a placemark as defined by ...FIXME.
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

        // Runners
        Wikipedia,
        OsmSite,
        Coordinate,

        // Planets
        MannedLandingSite,       // h
        RoboticRover,            // r
        UnmannedSoftLandingSite, // u
        UnmannedHardLandingSite, // i

        Folder,
        Bookmark,
        
        NaturalWater,
        NaturalWood,

        // OpenStreetMap highways
        HighwaySteps,
        HighwayUnknown,
        HighwayPath,
        HighwayTrack,
        HighwayPedestrian,
        HighwayService,
        HighwayRoad,
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

        // OpenStreetMap category Amenity
        AmenityLibrary,

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
        HealthDoctors,
        HealthHospital,
        HealthPharmacy,

        // OpenStreetMap category Money
        MoneyAtm,
        MoneyBank,

        // OpenStreetMap category Shopping
        ShoppingBeverages,
        ShoppingHifi,
        ShoppingSupermarket,

        // OpenStreetMap category Tourist
        TouristAttraction,
        TouristCastle,
        TouristCinema,
        TouristMonument,
        TouristMuseum,
        TouristRuin,
        TouristTheatre,
        TouristThemePark,
        TouristViewPoint,
        TouristZoo,

        // OpenStreetMap category Transport
        TransportAerodrome,
        TransportAirportTerminal,
        TransportBusStation,
        TransportBusStop,
        TransportCarShare,
        TransportFuel,
        TransportParking,
        TransportRentalBicycle,
        TransportRentalCar,
        TransportTaxiRank,
        TransportTrainStation,
        TransportTramStop,

        // OpenStreetMap category religion
        ReligionPlaceOfWorship,
        ReligionBahai,
        ReligionBuddhist,
        ReligionChristian,
        ReligionHindu,
        ReligionJain,
        ReligionJewish,
        ReligionShinto,
        ReligionSikh,
        // OpenStreetMap category Leisure
        LeisurePark,

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

        RailwayRail,
        RailwayTram,
        RailwayLightRail,
        RailwayAbandoned,
        RailwaySubway,
        RailwayPreserved,
        RailwayMiniature,
        RailwayConstruction,
        RailwayMonorail,
        RailwayFunicular,

        Satellite,

        // Important: Make sure that this is always the last 
        // item and just use it to specify the array size
        LastIndex
    };

    /**
     * @brief  Convenience categorization of placemarks for Osm key=value pairs
     */
    static GeoDataVisualCategory OsmVisualCategory(const QString &keyValue );

    /**
     * @brief The name of the feature
     *
     * The name of the feature should be a short string. It is often
     * shown directly on the map and need therefore not take up much
     * space.
     *
     * @return The name of this feature
     */
    QString name() const;
    /**
     * @brief Set a new name for this feature
     * @param value  the new name
     */
    void setName( const QString &value );

    /// Return the address of the feature
    QString address() const;
    /// Set the address of this feature to @p value.
    void setAddress( const QString &value);

    /// Return the phone number of the feature
    QString phoneNumber() const;
    /// Set the phone number of this feature to @p value.
    void setPhoneNumber( const QString &value );

    /// Return the text description of the feature.
    QString description() const;
    /// Set the description of this feature to @p value.
    void setDescription( const QString &value );

    /**
     * @brief test if the description is CDATA or not
     * CDATA allows for special characters to be included in XML and also allows
     * for other XML formats to be embedded in the XML without interfering with
     * parser namespace.
     * @return @true if the description should be treated as CDATA
     *         @false if the description is a plain string
     */
    bool descriptionIsCDATA() const;
    /// Set the description to be CDATA See: @see descriptionIsCDATA()
    void setDescriptionCDATA( bool cdata );

    /// Get the Abstract view of the feature
    GeoDataAbstractView abstractView() const;
    /// Set the abstract view of the feature
    void setAbstractView( const GeoDataAbstractView &abstractView );

    /// Return the styleUrl of the feature.
    QString styleUrl() const;
    /// Set the styleUrl of this feature to @p value.
    void setStyleUrl( const QString &value );

    /// Return whether this feature is visible or not
    bool isVisible() const;
    /**
     * @brief Set a new value for visibility
     * @param value  new value for the visibility
     *
     * This function sets the visibility, i.e. whether this feature
     * should be shown or not.  This can be changed either from a GUI
     * or through some action of the program.
     */ 
    void setVisible( bool value );

    /**
     * Return the timespan of the feature.
     */
    GeoDataTimeSpan& timeSpan() const;

    /**
     * Set the timespan of the feature.
     * @param timeSpan new of timespan.	
     */
    void setTimeSpan( GeoDataTimeSpan timeSpan );

    /**
     * Return the timestamp of the feature.
     */
    GeoDataTimeStamp& timeStamp() const;

    /**
     * Set the timestamp of the feature.
     * @param timeStamp new of the timestamp.
     */
    void setTimeStamp( GeoDataTimeStamp timeStamp );

    /**
     * Return the style assigned to the placemark.
     */
    GeoDataStyle* style() const;
    /**
     * Sets the style of the placemark.
     * @param  style  the new style to be used.
     */
    void setStyle( GeoDataStyle* style );

    /**
     * Return the ExtendedData assigned to the feature.
     */
    GeoDataExtendedData& extendedData() const;

    /**
     * Sets the ExtendedData of the feature.
     * @param  extendedData  the new ExtendedData to be used.
     */
    void setExtendedData( const GeoDataExtendedData& extendedData );

    /**
     * Return the region assigned to the placemark.
     */
    GeoDataRegion& region() const;
    /**
     * @brief Sets the region of the placemark.
     * @param region new value for the region
     *
     * The feature is only shown when the region if active.
     */
    void setRegion( const GeoDataRegion& region );

    /**
     * Return the symbol index of the placemark.
     */
    GeoDataVisualCategory visualCategory() const;
    /**
     * Sets the symbol @p index of the placemark.
     * @param  category  the new category to be used.
     */
    void setVisualCategory( GeoDataVisualCategory category );

    /**
     * Return the role of the placemark.
     *
     * FIXME: describe roles here!
     */
    const QString role() const;
    /**
     * Sets the role of the placemark.
     * @param  role  the new role to be used.
     */
    void setRole( const QString &role );

    /**
     * @brief Return the popularity index of the placemark.
     *
     * The popularity index is a value which describes at which zoom
     * level the placemark will be shown.
     */
    int popularityIndex() const;
    /**
     * Sets the popularity @p index of the placemark.
     * @param  index  the new index to be used.
     */
    void setPopularityIndex( int index );

    /**
     * Return the popularity of the feature.
     */
    qint64 popularity() const;
    /**
     * Sets the @p popularity of the feature.
     * @param  popularity  the new popularity value
     */
    void setPopularity( qint64 popularity );

    /**
     * Return a pointer to a GeoDataStyleMap object which represents the styleMap
     * of this feature. A styleMap is simply a QMap<QString,QString> which can connect
     * two styles with a keyword. This can be used to have a highlighted and a
     * normal style.
     * @see GeoDataStyleMap
     */
    GeoDataStyleMap* styleMap() const;
    /**
     * Sets the styleMap of the feature
     */
    void setStyleMap( GeoDataStyleMap* map );


    // ----------------------------------------------------------------
    // The following functions are use for painting, and mostly for placemarks.

    /**
     * Return the symbol size of the feature in pixels.
     */
    const QSize symbolSize() const;

    /**
     * Return the symbol of the feature as a pixmap.
     *
     * Note that the pixmaps are shared between all features with the
     * same visual category
     */
    const QPixmap symbolPixmap() const;

    /**
     * Return the label font of the placemark.
     */
    static void resetDefaultStyles();

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

    static QFont defaultFont();
    static void setDefaultFont( const QFont& font );

    virtual void detach();
 private:
    static void initializeDefaultStyles();
    static void initializeOsmVisualCategories();

 protected:
    // the d-pointer needs to be protected to be accessible from derived classes
    GeoDataFeaturePrivate* d;
    GeoDataFeature( GeoDataFeaturePrivate* priv );

 private:
    // the private d pointer accessor - use it instead of the d pointer directly
    GeoDataFeaturePrivate* p() const;
    // Static members
    static QFont         s_defaultFont;

    static GeoDataStyle* s_defaultStyle[GeoDataFeature::LastIndex];
    static bool          s_defaultStyleInitialized;

    static QMap<QString, GeoDataVisualCategory> s_visualCategories;
};

}

#endif
