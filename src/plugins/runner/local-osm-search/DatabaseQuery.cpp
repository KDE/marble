// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "DatabaseQuery.h"

#include "GeoDataLatLonBox.h"
#include "MarbleModel.h"
#include "PositionTracking.h"

#include <QMap>
#include <QRegExp>

namespace Marble
{

DatabaseQuery::DatabaseQuery(const MarbleModel *model, const QString &searchTerm, const GeoDataLatLonBox &preferred)
    : m_queryType(BroadSearch)
    , m_resultFormat(AddressFormat)
    , m_searchTerm(searchTerm.trimmed())
    , m_category(OsmPlacemark::UnknownCategory)
{
    if (model && model->positionTracking()->status() == PositionProviderStatusAvailable) {
        m_position = model->positionTracking()->currentLocation();
        m_resultFormat = DistanceFormat;
    } else if (!preferred.isEmpty()) {
        m_position = preferred.center();
        m_resultFormat = AddressFormat;
    } else {
        m_resultFormat = AddressFormat;
    }

    QStringList terms = m_searchTerm.split(QLatin1Char(','), Qt::SkipEmptyParts);

    QRegExp streetAndHouse(QStringLiteral(R"(^(.*)\s+(\d+\D?)$)"));
    if (streetAndHouse.indexIn(terms.first()) != -1) {
        if (streetAndHouse.capturedTexts().size() == 3) {
            terms.removeFirst();
            terms.push_front(streetAndHouse.capturedTexts().at(1));
            terms.push_front(streetAndHouse.capturedTexts().at(2));
        }
    }

    Q_ASSERT(terms.size() > 0);
    if (terms.size() == 1) {
        m_queryType = isPointOfInterest(m_searchTerm) ? CategorySearch : BroadSearch;
    } else if (terms.size() == 2) {
        m_street = terms.first().trimmed();
        m_region = terms.last().trimmed();
        m_queryType = isPointOfInterest(m_street) ? CategorySearch : AddressSearch;
    } else {
        m_houseNumber = terms.first().trimmed();
        m_street = terms.at(1).trimmed();
        m_region = terms.last().trimmed(); // skips 2, 3, ..., if any
        m_queryType = AddressSearch;
    }
}

bool DatabaseQuery::isPointOfInterest(const QString &category)
{
    static QMap<QString, OsmPlacemark::OsmCategory> pois;
    if (pois.isEmpty()) {
        pois[QObject::tr("pois").toLower()] = OsmPlacemark::UnknownCategory;
        pois[QStringLiteral("pois")] = OsmPlacemark::UnknownCategory;
        pois[QObject::tr("camping").toLower()] = OsmPlacemark::AccomodationCamping;
        pois[QStringLiteral("camping")] = OsmPlacemark::AccomodationCamping;
        pois[QObject::tr("hostel").toLower()] = OsmPlacemark::AccomodationHostel;
        pois[QStringLiteral("hostel")] = OsmPlacemark::AccomodationHostel;
        pois[QObject::tr("hotel").toLower()] = OsmPlacemark::AccomodationHotel;
        pois[QStringLiteral("hotel")] = OsmPlacemark::AccomodationHotel;
        pois[QObject::tr("motel").toLower()] = OsmPlacemark::AccomodationMotel;
        pois[QStringLiteral("motel")] = OsmPlacemark::AccomodationMotel;
        pois[QObject::tr("youth hostel").toLower()] = OsmPlacemark::AccomodationYouthHostel;
        pois[QStringLiteral("youth hostel")] = OsmPlacemark::AccomodationYouthHostel;
        pois[QObject::tr("library").toLower()] = OsmPlacemark::AmenityLibrary;
        pois[QStringLiteral("library")] = OsmPlacemark::AmenityLibrary;
        pois[QObject::tr("college").toLower()] = OsmPlacemark::EducationCollege;
        pois[QStringLiteral("college")] = OsmPlacemark::EducationCollege;
        pois[QObject::tr("school").toLower()] = OsmPlacemark::EducationSchool;
        pois[QStringLiteral("school")] = OsmPlacemark::EducationSchool;
        pois[QObject::tr("university").toLower()] = OsmPlacemark::EducationUniversity;
        pois[QStringLiteral("university")] = OsmPlacemark::EducationUniversity;
        pois[QObject::tr("bar").toLower()] = OsmPlacemark::FoodBar;
        pois[QStringLiteral("bar")] = OsmPlacemark::FoodBar;
        pois[QObject::tr("biergarten").toLower()] = OsmPlacemark::FoodBiergarten;
        pois[QStringLiteral("biergarten")] = OsmPlacemark::FoodBiergarten;
        pois[QObject::tr("cafe").toLower()] = OsmPlacemark::FoodCafe;
        pois[QStringLiteral("cafe")] = OsmPlacemark::FoodCafe;
        pois[QObject::tr("fast food").toLower()] = OsmPlacemark::FoodFastFood;
        pois[QStringLiteral("fast food")] = OsmPlacemark::FoodFastFood;
        pois[QObject::tr("pub").toLower()] = OsmPlacemark::FoodPub;
        pois[QStringLiteral("pub")] = OsmPlacemark::FoodPub;
        pois[QObject::tr("restaurant").toLower()] = OsmPlacemark::FoodRestaurant;
        pois[QStringLiteral("restaurant")] = OsmPlacemark::FoodRestaurant;
        pois[QObject::tr("doctor").toLower()] = OsmPlacemark::HealthDoctors;
        pois[QStringLiteral("doctor")] = OsmPlacemark::HealthDoctors;
        pois[QObject::tr("hospital").toLower()] = OsmPlacemark::HealthHospital;
        pois[QStringLiteral("hospital")] = OsmPlacemark::HealthHospital;
        pois[QObject::tr("pharmacy").toLower()] = OsmPlacemark::HealthPharmacy;
        pois[QStringLiteral("pharmacy")] = OsmPlacemark::HealthPharmacy;
        pois[QObject::tr("bank").toLower()] = OsmPlacemark::MoneyBank;
        pois[QStringLiteral("bank")] = OsmPlacemark::MoneyBank;
        pois[QObject::tr("beverages").toLower()] = OsmPlacemark::ShoppingBeverages;
        pois[QStringLiteral("beverages")] = OsmPlacemark::ShoppingBeverages;
        pois[QObject::tr("hifi").toLower()] = OsmPlacemark::ShoppingHifi;
        pois[QStringLiteral("hifi")] = OsmPlacemark::ShoppingHifi;
        pois[QObject::tr("supermarket").toLower()] = OsmPlacemark::ShoppingSupermarket;
        pois[QStringLiteral("supermarket")] = OsmPlacemark::ShoppingSupermarket;
        pois[QObject::tr("attraction").toLower()] = OsmPlacemark::TouristAttraction;
        pois[QStringLiteral("attraction")] = OsmPlacemark::TouristAttraction;
        pois[QObject::tr("castle").toLower()] = OsmPlacemark::TouristCastle;
        pois[QStringLiteral("castle")] = OsmPlacemark::TouristCastle;
        pois[QObject::tr("cinema").toLower()] = OsmPlacemark::TouristCinema;
        pois[QStringLiteral("cinema")] = OsmPlacemark::TouristCinema;
        pois[QObject::tr("monument").toLower()] = OsmPlacemark::TouristMonument;
        pois[QStringLiteral("monument")] = OsmPlacemark::TouristMonument;
        pois[QObject::tr("museum").toLower()] = OsmPlacemark::TouristMuseum;
        pois[QStringLiteral("museum")] = OsmPlacemark::TouristMuseum;
        pois[QObject::tr("ruin").toLower()] = OsmPlacemark::TouristRuin;
        pois[QStringLiteral("ruin")] = OsmPlacemark::TouristRuin;
        pois[QObject::tr("theatre").toLower()] = OsmPlacemark::TouristTheatre;
        pois[QStringLiteral("theatre")] = OsmPlacemark::TouristTheatre;
        pois[QObject::tr("theme park").toLower()] = OsmPlacemark::TouristThemePark;
        pois[QStringLiteral("theme park")] = OsmPlacemark::TouristThemePark;
        pois[QObject::tr("view point").toLower()] = OsmPlacemark::TouristViewPoint;
        pois[QStringLiteral("view point")] = OsmPlacemark::TouristViewPoint;
        pois[QObject::tr("zoo").toLower()] = OsmPlacemark::TouristZoo;
        pois[QStringLiteral("zoo")] = OsmPlacemark::TouristZoo;
        pois[QObject::tr("airport").toLower()] = OsmPlacemark::TransportAirportTerminal;
        pois[QStringLiteral("airport")] = OsmPlacemark::TransportAirportTerminal;
        pois[QObject::tr("airport runway").toLower()] = OsmPlacemark::TransportAirportRunway;
        pois[QStringLiteral("airport runway")] = OsmPlacemark::TransportAirportRunway;
        pois[QObject::tr("airport apron").toLower()] = OsmPlacemark::TransportAirportApron;
        pois[QStringLiteral("airport apron")] = OsmPlacemark::TransportAirportApron;
        pois[QObject::tr("airport taxiway").toLower()] = OsmPlacemark::TransportAirportTaxiway;
        pois[QStringLiteral("airport taxiway")] = OsmPlacemark::TransportAirportTaxiway;
        pois[QObject::tr("bus station").toLower()] = OsmPlacemark::TransportBusStation;
        pois[QStringLiteral("bus station")] = OsmPlacemark::TransportBusStation;
        pois[QObject::tr("bus stop").toLower()] = OsmPlacemark::TransportBusStop;
        pois[QStringLiteral("bus stop")] = OsmPlacemark::TransportBusStop;
        pois[QObject::tr("car share").toLower()] = OsmPlacemark::TransportCarShare;
        pois[QStringLiteral("car share")] = OsmPlacemark::TransportCarShare;
        pois[QObject::tr("fuel").toLower()] = OsmPlacemark::TransportFuel;
        pois[QStringLiteral("fuel")] = OsmPlacemark::TransportFuel;
        pois[QObject::tr("parking").toLower()] = OsmPlacemark::TransportParking;
        pois[QStringLiteral("parking")] = OsmPlacemark::TransportParking;
        pois[QObject::tr("train station").toLower()] = OsmPlacemark::TransportTrainStation;
        pois[QStringLiteral("train station")] = OsmPlacemark::TransportTrainStation;
        pois[QObject::tr("atm").toLower()] = OsmPlacemark::MoneyAtm;
        pois[QStringLiteral("atm")] = OsmPlacemark::MoneyAtm;
        pois[QObject::tr("tram stop").toLower()] = OsmPlacemark::TransportTramStop;
        pois[QStringLiteral("tram stop")] = OsmPlacemark::TransportTramStop;
        pois[QObject::tr("bicycle rental").toLower()] = OsmPlacemark::TransportRentalBicycle;
        pois[QStringLiteral("bicycle rental")] = OsmPlacemark::TransportRentalBicycle;
        pois[QObject::tr("car rental").toLower()] = OsmPlacemark::TransportRentalCar;
        pois[QStringLiteral("car rental")] = OsmPlacemark::TransportRentalCar;
        pois[QObject::tr("speed camera").toLower()] = OsmPlacemark::TransportSpeedCamera;
        pois[QStringLiteral("speed camera")] = OsmPlacemark::TransportSpeedCamera;
        pois[QObject::tr("taxi").toLower()] = OsmPlacemark::TransportTaxiRank;
        pois[QStringLiteral("taxi")] = OsmPlacemark::TransportTaxiRank;
        pois[QObject::tr("memorial").toLower()] = OsmPlacemark::HistoricMemorial;
        pois[QStringLiteral("memorial")] = OsmPlacemark::HistoricMemorial;
    }

    Q_ASSERT(!pois.isEmpty());
    QString const searchTerm = category.toLower();
    if (pois.contains(searchTerm)) {
        m_category = pois[searchTerm];
        return true;
    }

    m_category = OsmPlacemark::UnknownCategory;
    return false;
}

OsmPlacemark::OsmCategory DatabaseQuery::category() const
{
    return m_category;
}

DatabaseQuery::QueryType DatabaseQuery::queryType() const
{
    return m_queryType;
}

DatabaseQuery::ResultFormat DatabaseQuery::resultFormat() const
{
    return m_resultFormat;
}

QString DatabaseQuery::street() const
{
    return m_street;
}

QString DatabaseQuery::houseNumber() const
{
    return m_houseNumber;
}

QString DatabaseQuery::region() const
{
    return m_region;
}

QString DatabaseQuery::searchTerm() const
{
    return m_searchTerm;
}

GeoDataCoordinates DatabaseQuery::position() const
{
    return m_position;
}

}
