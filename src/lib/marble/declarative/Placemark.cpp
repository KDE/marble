//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Placemark.h"

#ifdef HAVE_QT5_POSITIONING
#include <GeoDataExtendedData.h>
#include <QGeoAddress>
#endif // HAVE_QT5_POSITIONING

#include <osm/OsmPlacemarkData.h>
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"

namespace Marble {

Placemark::Placemark(QObject *parent ) :
    QObject( parent )
{
    // nothing to do
}

void Placemark::setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark )
{
    m_placemark = placemark;
    m_address = QString();
    m_description = QString();
    m_website = QString();
    m_wikipedia = QString();
    m_fuelDetails = QString();
    m_openingHours = QString();
    m_elevation = QString();
    m_amenity = QString();
    m_shop = QString();
    emit coordinatesChanged();
    emit nameChanged();
    emit descriptionChanged();
    emit addressChanged();
    emit websiteChanged();
    emit wikipediaChanged();
    emit openingHoursChanged();
    emit elevationChanged();
    emit amenityChanged();
    emit shopChanged();
    if (m_placemark.visualCategory() == GeoDataFeature::TransportFuel) {
        emit fuelDetailsChanged();
    }
}

Marble::GeoDataPlacemark & Placemark::placemark()
{
    return m_placemark;
}

const GeoDataPlacemark &Placemark::placemark() const
{
    return m_placemark;
}

QString Placemark::name() const
{
    return m_placemark.name();
}

QString Placemark::description() const
{
    if (m_description.isEmpty()) {
        auto const category = m_placemark.visualCategory();
        m_description = m_placemark.categoryName();
        if (category >= GeoDataFeature::FoodBar && category <= GeoDataFeature::FoodRestaurant) {
            addTagValue(m_description, "brand");
            addTagValue(m_description, "cuisine");
            addTagDescription(m_description, "self_service", "yes", "Self Service");
            addTagDescription(m_description, "takeaway", "yes", "Take Away");
            addTagDescription(m_description, "outdoor_seating", "yes", "Outdoor Seating");
            addTagDescription(m_description, "ice_cream", "yes", "Ice Cream");
            addTagDescription(m_description, "smoking", "dedicated", "Smoking (dedicated)");
            addTagDescription(m_description, "smoking", "yes", "Smoking allowed");
            addTagDescription(m_description, "smoking", "separated", "Smoking (separated)");
            addTagDescription(m_description, "smoking", "isolated", "Smoking (isolated)");
            addTagDescription(m_description, "smoking", "no", "No smoking");
            addTagDescription(m_description, "smoking", "outside", "Smoking (outside)");
            addTagDescription(m_description, "smoking:outside", "yes", "Smoking (outside)");
            addTagDescription(m_description, "smoking:outside", "separated", "Smoking (outside separated)");
            addTagDescription(m_description, "smoking:outside", "no", "No smoking outside");
        } else if (category >= GeoDataFeature::ShopBeverages && category <= GeoDataFeature::Shop) {
            addTagValue(m_description, "operator");
        } else if (category == GeoDataFeature::TransportBusStop) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
            addTagValue(m_description, "ref");
        } else if (category == GeoDataFeature::TransportCarShare) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
        } else if (category == GeoDataFeature::TransportFuel) {
            addTagValue(m_description, "brand");
            addTagValue(m_description, "operator");
        } else if (category == GeoDataFeature::NaturalTree) {
            addTagValue(m_description, "species:en");
            addTagValue(m_description, "genus:en");
            addTagValue(m_description, "leaf_type");
        }
    }

    return m_description;
}

QString Placemark::address() const
{
    if (m_address.isEmpty()) {
        m_address = addressFromOsmData();
    }
    return m_address;
}

QString Placemark::fuelDetails() const
{
    if (m_fuelDetails.isEmpty() && m_placemark.visualCategory() == GeoDataFeature::TransportFuel) {
        addTagDescription(m_fuelDetails, "fuel:diesel", "yes", tr("Diesel"));
        addTagDescription(m_fuelDetails, "fuel:octane_91", "yes", tr("Octane 91"));
        addTagDescription(m_fuelDetails, "fuel:octane_95", "yes", tr("Octane 95"));
        addTagDescription(m_fuelDetails, "fuel:octane_98", "yes", tr("Octane 98"));
        addTagDescription(m_fuelDetails, "fuel:e10", "yes", tr("E10"));
        addTagDescription(m_fuelDetails, "fuel:lpg", "yes", tr("LPG"));

    }
    return m_fuelDetails;
}

QString Placemark::website() const
{
    if (!m_website.isEmpty()) {
        return m_website;
    }
    foreach(const QString &tag, QStringList() << "website" << "contact:website" << "facebook" << "contact:facebook" << "url") {
        m_website = m_placemark.osmData().tagValue(tag);
        if (!m_website.isEmpty()) {
            return m_website;
        }
    }

    return m_website;
}

QString Placemark::wikipedia() const
{
    if (!m_wikipedia.isEmpty()) {
        return m_wikipedia;
    }

    m_wikipedia = m_placemark.osmData().tagValue("wikipedia");
    return m_wikipedia;
}

QString Placemark::openingHours() const
{
    if (!m_openingHours.isEmpty()) {
        return m_openingHours;
    }

    addTagValue(m_openingHours, "opening_hours");
    return m_openingHours;
}

QString Placemark::coordinates() const
{
    return m_placemark.coordinate().toString(GeoDataCoordinates::Decimal).trimmed();
}

QString Placemark::elevation() const
{
    if (!m_elevation.isEmpty()){
        return m_elevation;
    }

    m_elevation = m_placemark.osmData().tagValue(QStringLiteral("ele"));

    return m_elevation;
}

QString Placemark::amenity() const
{
    if (!m_amenity.isEmpty()){
        return m_amenity;
    }

    m_amenity = m_placemark.osmData().tagValue(QStringLiteral("amenity"));
    if (!m_amenity.isEmpty()) {
        m_amenity[0] = m_amenity[0].toUpper();
    }

    return m_amenity;
}

QString Placemark::shop() const
{
    if (!m_shop.isEmpty()){
        return m_shop;
    }

    const OsmPlacemarkData& osmData = m_placemark.osmData();

    QString shop = osmData.tagValue(QStringLiteral("shop"));
    if (!shop.isEmpty()) {
        shop[0] = shop[0].toUpper();

        if (shop == QLatin1String("Clothes")) {
            QString type = osmData.tagValue(QStringLiteral("clothes"));
            if (type.isEmpty()) {
                type = osmData.tagValue(QStringLiteral("designation"));
            }
            if (!type.isEmpty()) {
                type[0] = type[0].toUpper();
                m_shop = QLatin1String("Shop : ") + shop + QLatin1String(" (") + type + QLatin1Char(')');
            }
        }
        if (m_shop.isEmpty()) {
            m_shop = QLatin1String("Shop : ") + shop;
        }
    }

    return m_shop;
}

void Placemark::setName(const QString & name)
{
    if (m_placemark.name() == name) {
        return;
    }

    m_placemark.setName(name);
    emit nameChanged();
}

double Placemark::longitude() const
{
    return m_placemark.coordinate().longitude(GeoDataCoordinates::Degree);
}

double Placemark::latitude() const
{
    return m_placemark.coordinate().latitude(GeoDataCoordinates::Degree);
}

void Placemark::addTagValue(QString &target, const QString &key) const
{
    auto const & osmData = m_placemark.osmData();
    QString const value = osmData.tagValue(key);
    QString description = value;
    description.replace(QLatin1Char(';'), " · ");
    addTagDescription(target, key, value, description);
}

void Placemark::addTagDescription(QString &target, const QString &key, const QString &value, const QString &description) const
{
    auto const & osmData = m_placemark.osmData();
    if (osmData.containsTag(key, value)) {
        if (!target.isEmpty()) {
            target += QStringLiteral(" · "); // non-latin1
        }
        target += description;
    }
}

QString Placemark::addressFromOsmData() const
{
#ifdef HAVE_QT5_POSITIONING
    QGeoAddress address;
    OsmPlacemarkData const data = m_placemark.osmData();
    address.setCountry(data.tagValue("addr:country"));
    address.setState(data.tagValue("addr:state"));
    address.setCity(data.tagValue("addr:city"));
    address.setDistrict(data.tagValue("district"));
    address.setPostalCode(data.tagValue("addr:postcode"));
    QString const street = data.tagValue("addr:street");
    QString const houseNumber = data.tagValue("addr:housenumber");
    address.setStreet(formatStreet(street, houseNumber));
    return address.text().replace("<br/>", ", ");
#else
    return QString();
#endif
}

QString Placemark::formatStreet(const QString &street, const QString &houseNumber) const
{
    return houseNumber.isEmpty() ? street : tr("%1 %2",
        "House number (first argument) and street name (second argument) in an address").arg(houseNumber).arg(street).trimmed();
}

}

#include "moc_Placemark.cpp"
