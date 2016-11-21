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
    m_openingHours = QString();
    updateTags();
    emit coordinatesChanged();
    emit nameChanged();
    emit descriptionChanged();
    emit addressChanged();
    emit websiteChanged();
    emit wikipediaChanged();
    emit openingHoursChanged();
    emit tagsChanged();
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

        if (category == GeoDataPlacemark::AccomodationHotel || category == GeoDataPlacemark::FoodRestaurant) {
            QString const stars = m_placemark.osmData().tagValue(QStringLiteral("stars"));
            if (!stars.isEmpty()) {
                bool hasStars;
                int const numStars = stars.mid(0, 1).toInt(&hasStars);
                if (hasStars) {
                    m_description += QString(' ') + QString("*").repeated(numStars) + stars.mid(1);
                } else {
                    addTagValue(m_description, QStringLiteral("stars"));
                }
            }
            addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");
        }

        if (category >= GeoDataPlacemark::FoodBar && category <= GeoDataPlacemark::FoodRestaurant) {
            if (category != GeoDataPlacemark::FoodRestaurant) {
                addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");
            } else {
                // Do nothing, already added in stars section above
            }
            addTagValue(m_description, "cuisine");
            addTagValue(m_description, "brewery");
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
        } else if (category >= GeoDataPlacemark::ShopBeverages && category <= GeoDataPlacemark::Shop) {
            addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");
            addTagValue(m_description, "clothes");
            addTagValue(m_description, "designation");
        } else if (category == GeoDataPlacemark::TransportBusStop) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
            addTagValue(m_description, "ref");
        } else if (category == GeoDataPlacemark::TransportCarShare) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
        } else if (category == GeoDataPlacemark::TransportRentalBicycle ||
                   category == GeoDataPlacemark::TransportRentalCar) {
            addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");

        } else if (category == GeoDataPlacemark::TransportFuel) {
            addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");
            addTagDescription(m_description, "fuel:diesel", "yes", tr("Diesel"));
            addTagDescription(m_description, "fuel:biodiesel", "yes", tr("Biodiesel"));
            addTagDescription(m_description, "fuel:octane_91", "yes", tr("Octane 91"));
            addTagDescription(m_description, "fuel:octane_95", "yes", tr("Octane 95"));
            addTagDescription(m_description, "fuel:octane_98", "yes", tr("Octane 98"));
            addTagDescription(m_description, "fuel:octane_100", "yes", tr("Octane 100"));
            addTagDescription(m_description, "fuel:e10", "yes", tr("E10"));
            addTagDescription(m_description, "fuel:lpg", "yes", tr("LPG"));
        } else if (category == GeoDataPlacemark::NaturalTree) {
            addTagValue(m_description, "species:en");
            addTagValue(m_description, "genus:en");
            addTagValue(m_description, "leaf_type");
        } else if (category == GeoDataPlacemark::AmenityRecycling) {
            addTagDescription(m_description, QStringLiteral("recycling:batteries"), "yes", tr("Batteries"));
            addTagDescription(m_description, QStringLiteral("recycling:clothes"), "yes", tr("Clothes"));
            addTagDescription(m_description, QStringLiteral("recycling:glass"), "yes", tr("Glass"));
            addTagDescription(m_description, QStringLiteral("recycling:glass_bottles"), "yes", tr("Glass bottles"));
            addTagDescription(m_description, QStringLiteral("recycling:green_waste"), "yes", tr("Green waste"));
            addTagDescription(m_description, QStringLiteral("recycling:garden_waste"), "yes", tr("Garden waste"));
            addTagDescription(m_description, QStringLiteral("recycling:electrical_items"), "yes", tr("Electrical items"));
            addTagDescription(m_description, QStringLiteral("recycling:metal"), "yes", tr("Metal"));
            addTagDescription(m_description, QStringLiteral("recycling:mobile_phones"), "yes", tr("Mobile phones"));
            addTagDescription(m_description, QStringLiteral("recycling:newspaper"), "yes", tr("Newspaper"));
            addTagDescription(m_description, QStringLiteral("recycling:paint"), "yes", tr("Paint"));
            addTagDescription(m_description, QStringLiteral("recycling:paper"), "yes", tr("Paper"));
            addTagDescription(m_description, QStringLiteral("recycling:paper_packaging"), "yes", tr("Paper packaging"));
            addTagDescription(m_description, QStringLiteral("recycling:PET"), "yes", tr("PET"));
            addTagDescription(m_description, QStringLiteral("recycling:plastic"), "yes", tr("Plastic"));
            addTagDescription(m_description, QStringLiteral("recycling:plastic_bags"), "yes", tr("Plastic bags"));
            addTagDescription(m_description, QStringLiteral("recycling:plastic_bottles"), "yes", tr("Plastic bottles"));
            addTagDescription(m_description, QStringLiteral("recycling:plastic_packaging"), "yes", tr("Plastic packaging"));
            addTagDescription(m_description, QStringLiteral("recycling:polyester"), "yes", tr("Polyester"));
            addTagDescription(m_description, QStringLiteral("recycling:tyres"), "yes", tr("Tyres"));
            addTagDescription(m_description, QStringLiteral("recycling:waste"), "yes", tr("Waste"));
            addTagDescription(m_description, QStringLiteral("recycling:white_goods"), "yes", tr("White goods"));
            addTagDescription(m_description, QStringLiteral("recycling:wood"), "yes", tr("Wood"));
        } else if (category == GeoDataPlacemark::NaturalPeak) {
            addTagValue(m_description, QStringLiteral("ele"), tr("%1 m"));
        }

        if (category == GeoDataPlacemark::AmenityRecycling || category == GeoDataPlacemark::AmenityPostBox) {
            addTagValue(m_description, QStringLiteral("collection_times"), tr("Collection times %1"), QStringLiteral(", "));
        }

        addTagDescription(m_description, "access", "no", tr("no access"));
        addTagDescription(m_description, "access", "private", tr("private"));
        addTagDescription(m_description, "access", "customers", tr("customers only"));

        addTagDescription(m_description, QStringLiteral("fee"), QStringLiteral("no"), tr("no fee"));
        addTagValue(m_description, QStringLiteral("description"));
        addTagValue(m_description, QStringLiteral("old_name"), tr("formerly <i>%1</i>"));
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

QString Placemark::website() const
{
    if (!m_website.isEmpty()) {
        return m_website;
    }
    foreach(const QString &tag, QStringList() << "website" << "contact:website" << "facebook" << "contact:facebook" << "url") {
        QString const value = m_placemark.osmData().tagValue(tag);
        if (!value.isEmpty()) {
            QUrl url = QUrl(value);
            if (url.isValid()) {
                if (url.scheme().isEmpty()) {
                    m_website = QStringLiteral("http://%1").arg(value);
                } else {
                    m_website = value;
                }
                if (!m_website.isEmpty()) {
                    return m_website;
                }
            }
        }
    }

    return m_website;
}

QString Placemark::wikipedia() const
{
    if (!m_wikipedia.isEmpty()) {
        return m_wikipedia;
    }

    // TODO: also support "wikipedia:lang=page title" tags
    const QString wikipedia = m_placemark.osmData().tagValue("wikipedia");
    if (!wikipedia.isEmpty()) {
        // full URL?
        if (wikipedia.startsWith(QLatin1String("http://")) ||
            wikipedia.startsWith(QLatin1String("https://"))) {
            m_wikipedia = wikipedia;
        } else {
            // match "(lang:)human readable title"
            QRegularExpression re("^(?:([a-z]{2,}):)?(.*)$");
            QRegularExpressionMatch match = re.match(wikipedia);
            QString lang = match.captured(1);
            if (lang.isEmpty()) {
                lang = QStringLiteral("en");
            }
            const QString title = QString::fromLatin1(QUrl::toPercentEncoding(match.captured(2)));

            m_wikipedia = QLatin1String("https://") + lang + QLatin1String(".wikipedia.org/wiki/") + title;
        }
    }

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

const QStringList &Placemark::tags() const
{
    return m_tags;
}

bool Placemark::addTagValue(QString &target, const QString &key, const QString &format, const QString separator) const
{
    auto const & osmData = m_placemark.osmData();
    QString const value = osmData.tagValue(key);
    if (!value.isEmpty()) {
        QString description = format.isEmpty() ? value : format.arg(value);
        description.replace(QLatin1Char(';'), separator);
        append(target, description);
        return true;
    }
    return false;
}

void Placemark::addFirstTagValueOf(QString &target, const QStringList &keys) const
{
    for (auto const &key: keys) {
        if (addTagValue(target, key)) {
            return;
        }
    }
}

void Placemark::addTagDescription(QString &target, const QString &key, const QString &value, const QString &description) const
{
    auto const & osmData = m_placemark.osmData();
    if (osmData.containsTag(key, value)) {
        append(target, description);
    }
}

void Placemark::append(QString &target, const QString &value) const
{
    if (!target.isEmpty()) {
        target += QStringLiteral(" · "); // non-latin1
    }
    target += value;
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

void Placemark::updateTags()
{
    m_tags.clear();
    QString const tag = QStringLiteral("%1 = %2");
    for (auto iter = m_placemark.osmData().tagsBegin(), end = m_placemark.osmData().tagsEnd(); iter != end; ++iter) {
        m_tags << tag.arg(iter.key()).arg(iter.value());
    }
}

}

#include "moc_Placemark.cpp"
