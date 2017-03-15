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
#include "GeoDataTypes.h"
#include "GeoDataDocument.h"

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
    m_wheelchairInfo = QString();
    m_wifiAvailable = QString();
    updateTags();
    updateRelations(placemark);
    emit coordinatesChanged();
    emit nameChanged();
    emit descriptionChanged();
    emit addressChanged();
    emit websiteChanged();
    emit wikipediaChanged();
    emit openingHoursChanged();
    emit wheelchairInfoChanged();
    emit wifiAvailabilityChanged();
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
    return m_placemark.displayName();
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
                    m_description += QString(' ') + QString("★").repeated(numStars) + stars.mid(1);
                } else {
                    addTagValue(m_description, QStringLiteral("stars"));
                }
            }
            addFirstTagValueOf(m_description, QStringList() << "brand" << "operator");
        }

        if ((category >= GeoDataPlacemark::AccomodationHostel &&
                category <= GeoDataPlacemark::AccomodationGuestHouse) ||
                category == GeoDataPlacemark::HealthHospital) {
            int const rooms = m_placemark.osmData().tagValue(QStringLiteral("rooms")).toInt();
            if (rooms > 0) {
                //~ singular %n room
                //~ plural %n rooms
                addTagValue(m_description, QStringLiteral("rooms"), tr("%n rooms", 0, rooms));
            }
            int const beds = m_placemark.osmData().tagValue(QStringLiteral("beds")).toInt();
            if (beds > 0) {
                //~ singular %n bed
                //~ plural %n beds
                addTagValue(m_description, QStringLiteral("beds"), tr("%n beds", 0, beds));
            }
        }

        if (category == GeoDataPlacemark::TransportParking || category == GeoDataPlacemark::TransportBicycleParking || category == GeoDataPlacemark::TransportMotorcycleParking) {
            addTagValue(m_description, QStringLiteral("capacity"), tr("%1 parking spaces"));
            addTagValue(m_description, QStringLiteral("maxstay"), tr("Maximum parking time %1"));
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
            if (category == GeoDataPlacemark::ShopButcher) {
                addTagValue(m_description, "butcher");
            } else if (category == GeoDataPlacemark::ShopCopy) {
                addTagDescription(m_description, QStringLiteral("service:computer"), QStringLiteral("yes"), tr("Computers available", "A copy shop provides computers for customer use"));
                addTagDescription(m_description, QStringLiteral("service:computer"), QStringLiteral("no"), tr("No computers available", "A copy shop does not provide computers for customer use"));
                addTagDescription(m_description, QStringLiteral("service:copy"), QStringLiteral("yes"), tr("Photocopying service", "A copy shop provides photocopying service"));
                addTagDescription(m_description, QStringLiteral("service:copy"), QStringLiteral("no"), tr("No photocopying service", "A copy shop does not provide photocopying service"));
                addTagDescription(m_description, QStringLiteral("service:scan"), QStringLiteral("yes"), tr("Digital scanning", "A copy shop provides a service for scanning documents into digital files"));
                addTagDescription(m_description, QStringLiteral("service:scan"), QStringLiteral("no"), tr("No digital scanning", "A copy shop does not provide a service for scanning documents into digital files"));
                addTagDescription(m_description, QStringLiteral("service:fax"), QStringLiteral("yes"), tr("Fax service", "A copy shop provides a service to send documents through fax"));
                addTagDescription(m_description, QStringLiteral("service:fax"), QStringLiteral("no"), tr("No fax service", "A copy shop does not provide a service to send documents through fax"));
                addTagDescription(m_description, QStringLiteral("service:phone"), QStringLiteral("yes"), tr("Phone service", "A copy shop provides a paid service to make phone calls"));
                addTagDescription(m_description, QStringLiteral("service:phone"), QStringLiteral("no"), tr("No phone service", "A copy shop does not provide a paid service to make phone calls"));
                addTagDescription(m_description, QStringLiteral("service:print"), QStringLiteral("yes"), tr("Digital printing", "A copy shop provides services to print paper documents from digital files"));
                addTagDescription(m_description, QStringLiteral("service:print"), QStringLiteral("no"), tr("No digital printing", "A copy shop does not provide services to print paper documents from digital files"));
                addTagDescription(m_description, QStringLiteral("service:press"), QStringLiteral("yes"), tr("Press printing service", "A copy shop provides a professional service to print a large number of copies of a document"));
                addTagDescription(m_description, QStringLiteral("service:press"), QStringLiteral("no"), tr("No press printing service", "A copy shop does not provide a professional service to print a large number of copies of a document"));
                addTagDescription(m_description, QStringLiteral("service:prepress"), QStringLiteral("yes"), tr("Press printing assistance", "A copy shop provides help with preparing special printing techniques"));
                addTagDescription(m_description, QStringLiteral("service:prepress"), QStringLiteral("no"), tr("No press printing assistance", "A copy shop does not provide help with preparing special printing techniques"));
                addTagDescription(m_description, QStringLiteral("service:self"), QStringLiteral("yes"), tr("Self service", "A copy shop provides individual copy machines for self-service"));
                addTagDescription(m_description, QStringLiteral("service:self"), QStringLiteral("no"), tr(" No self service", "A copy shop does not provide individual machines for self-service"));
            } else if (category == GeoDataPlacemark::ShopDeli) {
                addTagDescription(m_description, QStringLiteral("organic"), QStringLiteral("yes"), tr("Sells organic food", "A deli that sells organic food"));
                addTagDescription(m_description, QStringLiteral("organic"), QStringLiteral("no"), tr("Does not sell organic food", "A deli that does not sell organic food"));
                addTagDescription(m_description, QStringLiteral("organic"), QStringLiteral("only"), tr("Only sells organic food", "A deli that only sells organic food"));

                addTagDescription(m_description, QStringLiteral("diet:gluten_free"), QStringLiteral("yes"), tr("Sells gluten free food", "A deli that sells gluten free food"));
                addTagDescription(m_description, QStringLiteral("diet:gluten_free"), QStringLiteral("no"), tr("Does not sell gluten free food", "A deli that does not sell gluten free food"));
                addTagDescription(m_description, QStringLiteral("diet:gluten_free"), QStringLiteral("only"), tr("Only sells gluten free food", "A deli that only sells gluten free food"));

                addTagDescription(m_description, QStringLiteral("diet:lactose_free"), QStringLiteral("yes"), tr("Sells lactose free food", "A deli that sells lactose free food"));
                addTagDescription(m_description, QStringLiteral("diet:lactose_free"), QStringLiteral("no"), tr("Does not sell lactose free food", "A deli that does not sell lactose free food"));
                addTagDescription(m_description, QStringLiteral("diet:lactose_free"), QStringLiteral("only"), tr("Only sells lactose free food", "A deli that only sells lactose free food"));
            } else if (category == GeoDataPlacemark::ShopTobacco) {
                addTagDescription(m_description, QStringLiteral("lottery"), QStringLiteral("yes"), tr("Sells lottery tickets", "A tobacco shop that also sells lottery tickets"));
                addTagDescription(m_description, QStringLiteral("stamps"), QStringLiteral("yes"), tr("Sells revenue stamps", "A tobacco shop that also sells revenue stamps"));
                addTagDescription(m_description, QStringLiteral("salt"), QStringLiteral("yes"), tr("Sells salt", "A tobacco shop that also sells salt"));
            }
        } else if (category == GeoDataPlacemark::TransportBusStop) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
            addTagValue(m_description, "ref");
        } else if (category == GeoDataPlacemark::TransportCarShare) {
            addTagValue(m_description, "network");
            addTagValue(m_description, "operator");
        } else if (category == GeoDataPlacemark::TransportRentalBicycle ||
                   category == GeoDataPlacemark::TransportRentalCar ||
                   category == GeoDataPlacemark::TransportRentalSki) {
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
        } else if (category == GeoDataPlacemark::NaturalCave){
            addTagValue(m_description, "cave:ref");
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
        } else if (category == GeoDataPlacemark::NaturalVolcano) {
            addTagDescription(m_description, QStringLiteral("volcano:status"), QStringLiteral("active"), tr("Active", "An active volcano"));
            addTagDescription(m_description, QStringLiteral("volcano:status"), QStringLiteral("dormant"), tr("Dormant", "A dormant volcano that will erupt at some point in the future."));
            addTagDescription(m_description, QStringLiteral("volcano:status"), QStringLiteral("extinct"), tr("Extinct", "A volcano considered extinct, it has not erupted within the last 10000 years and likely never will again."));
            addTagDescription(m_description, QStringLiteral("volcano:type"), QStringLiteral("stratovolcano"), tr("Stratovolcano"));
            addTagDescription(m_description, QStringLiteral("volcano:type"), QStringLiteral("shield"), tr("Shield volcano"));
            addTagDescription(m_description, QStringLiteral("volcano:type"), QStringLiteral("scoria"), tr("Scoria cone", "A scoria cone volcano."));
        } else if (category == GeoDataPlacemark::HealthDoctors) {
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("alternative"), tr("Alternative medicine"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("audiologist"), tr("Audiologist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("blood_bank"), tr("Blood bank"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("blood_donation"), tr("Blood donation"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("centre"), tr("Medical center"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("clinic"), tr("Clinic"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("dentist"), tr("Dentist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("doctor"), tr("Medical practitioner"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("hospital"), tr("Hospital"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("midwife"), tr("Midwife"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("optometrist"), tr("Optometrist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("physiotherapist"), tr("Physiotherapist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("podiatrist"), tr("Podiatrist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("psychotherapist"), tr("Psychotherapist"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("rehabilitation"), tr("Rehabilitation"));
            addTagDescription(m_description, QStringLiteral("healthcare"), QStringLiteral("speech_therapist"), tr("Speech therapist"));
            addTagValue(m_description, QStringLiteral("healthcare:speciality"));
        } else if (category == GeoDataPlacemark::AmenityBench) {
            int const seats = m_placemark.osmData().tagValue(QStringLiteral("seats")).toInt();
            if (seats > 0) {
                //~ singular %n seat
                //~ plural %n seats
                addTagValue(m_description, QStringLiteral("seats"), tr("%n seats", "number of seats a bench provides", seats));
            }
            addTagValue(m_description, QStringLiteral("material"));
            addTagDescription(m_description, QStringLiteral("backrest"), QStringLiteral("yes"), tr("Has backrest", "A bench provides a backrest to lean against"));
            addTagDescription(m_description, QStringLiteral("backrest"), QStringLiteral("no"), tr("No backrest", "A bench provides no backrest to lean against"));
        } else if (category == GeoDataPlacemark::AmenityWasteBasket) {
            addTagValue(m_description, QStringLiteral("waste"));
        } else if (category == GeoDataPlacemark::TransportSpeedCamera) {
            addTagValue(m_description, QStringLiteral("maxspeed"), tr("%1 km/h"));
            addTagValue(m_description, "ref");
        } else if (category == GeoDataPlacemark::TransportParking) {
            addTagDescription(m_description, QStringLiteral("supervised"), QStringLiteral("yes"), tr("Is supervised", "Parking spaces are supervised by guards"));
            addTagDescription(m_description, QStringLiteral("supervised"), QStringLiteral("no"), tr("Not supervised", "Parking spaces are not supervised by guards"));

            int const disabledSpaces = m_placemark.osmData().tagValue(QStringLiteral("capacity:disabled")).toInt();
            if (disabledSpaces > 0) {
                addTagValue(m_description, QStringLiteral("capacity:disabled"), tr("%1 disabled spaces", "Parking spaces"));
            }
            int const womenSpaces = m_placemark.osmData().tagValue(QStringLiteral("capacity:women")).toInt();
            if (womenSpaces > 0) {
                addTagValue(m_description, QStringLiteral("capacity:women"), tr("%1 women spaces", "Parking spaces"));
            }
            int const parentSpaces = m_placemark.osmData().tagValue(QStringLiteral("capacity:parent")).toInt();
            if (parentSpaces > 0) {
                addTagValue(m_description, QStringLiteral("capacity:parent"), tr("%1 parent and child spaces", "Parking spaces"));
            }
            int const electricChargers = m_placemark.osmData().tagValue(QStringLiteral("capacity:charging")).toInt();
            if (electricChargers > 0) {
                addTagValue(m_description, QStringLiteral("capacity:charging"), tr("%1 spaces with electric chargers", "Parking spaces"));
            }
        } else if (category == GeoDataPlacemark::TransportBicycleParking) {
            addTagDescription(m_description, QStringLiteral("surveillance"), QStringLiteral("outdoor"), tr("Has outdoor surveillance", "A parking space has outdoor surveillance"));
            addTagDescription(m_description, QStringLiteral("surveillance"), QStringLiteral("indoor"), tr("Has indoor surveillance", "A parking space has indoor surveillance"));
            addTagDescription(m_description, QStringLiteral("surveillance"), QStringLiteral("public"), tr("Has public surveillance", "A parking space has public surveillance"));
        } else if (category == GeoDataPlacemark::TourismWildernessHut) {
            addTagDescription(m_description, QStringLiteral("shower"), QStringLiteral("yes"), tr("Has shower", "A hut provides showers inside or aside"));
            addTagDescription(m_description, QStringLiteral("shower"), QStringLiteral("no"), tr("Has no shower", "A hut does not provide showers inside or aside"));
            addTagDescription(m_description, QStringLiteral("mattress"), QStringLiteral("yes"), tr("Has mattress", "A hut provides mattress"));
            addTagDescription(m_description, QStringLiteral("mattress"), QStringLiteral("no"), tr("Has no mattress", "A hut does not provide mattress"));
            addTagDescription(m_description, QStringLiteral("drinking_water"), QStringLiteral("yes"), tr("Has water", "Water is available inside or aside"));
            addTagDescription(m_description, QStringLiteral("drinking_water"), QStringLiteral("no"), tr("Has no water", "Water is not available inside nor aside"));
            addTagDescription(m_description, QStringLiteral("toilets"), QStringLiteral("yes"), tr("Has toilets", "A hut provides toilets"));
            addTagDescription(m_description, QStringLiteral("toilets"), QStringLiteral("no"), tr("Has no toilets", "A hut does not provide toilets"));
            addTagDescription(m_description, QStringLiteral("reservation"), QStringLiteral("yes"), tr("Reservation is possible"));
            addTagDescription(m_description, QStringLiteral("reservation"), QStringLiteral("no"), tr("No reservation possible"));
            addTagDescription(m_description, QStringLiteral("reservation"), QStringLiteral("required"), tr("Reservation is required"));
            addTagDescription(m_description, QStringLiteral("reservation"), QStringLiteral("recommended"), tr("Reservation is recommended", "You should make reservation"));
            addTagDescription(m_description, QStringLiteral("reservation"), QStringLiteral("members_only"), tr("Only for members", "Reservation is only possible for members of the organisation running the hut"));
        } else if (category == GeoDataPlacemark::TourismArtwork) {
            addTagValue(m_description, QStringLiteral("artist_name"), tr("By %1"));
        } else if (category == GeoDataPlacemark::AmenityChargingStation) {
            addTagValue(m_description, QStringLiteral("capacity"), tr("%1 vehicles"));
            addTagValue(m_description, QStringLiteral("amperage"), tr("%1 ampere"));
            addTagValue(m_description, QStringLiteral("voltage"), tr("%1 volt"));

            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("cee_blue"), tr("%1 blue CEE sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("cee_red_16a"), tr("%1 red CEE sockets (16 A)"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("cee_red_32a"), tr("%1 red CEE sockets (32 A)"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("cee_red_64a"), tr("%1 red CEE sockets (64 A)"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("cee_red_125a"), tr("%1 red CEE sockets (125 A)"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("nema_5_15"), tr("%1 NEMA-5-15P plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("typeb"), tr("%1 NEMA-5-15P plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("nema_5_20"), tr("%1 NEMA-5-20P plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("nema_14_30"), tr("%1 NEMA 14-30 sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("nema_14_50"), tr("%1 NEMA 14-50 sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("schuko"), tr("%1 Schuko sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("bs1363"), tr("%1 BS 1363 sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("type1"), tr("%1 Type 1 plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("type1_combo"), tr("%1 Type 1 combo plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("type2"), tr("%1 Type 2 sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("type2_combo"), tr("%1 Type 2 combo sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("type3"), tr("%1 Type 3 sockets"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("chademo"), tr("%1 CHAdeMO plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("magne_charge"), tr("%1 Magne Charge plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("tesla_standard"), tr("%1 Tesla standard plugs"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("tesla_supercharger"), tr("%1 Tesla standard plugs (Supercharger)"));
            addTagDescription(m_description, QStringLiteral("socket"), QStringLiteral("tesla_roadster"), tr("%1 Tesla roadster plugs"));
        } else if (category == GeoDataPlacemark::AmenityCarWash) {
            addTagValue(m_description, QStringLiteral("maxwidth"), tr("Maximum vehicle width: %1"));
            addTagValue(m_description, QStringLiteral("maxheight"), tr("Maximum vehicle height: %1"));

            addTagDescription(m_description, QStringLiteral("self_service"), QStringLiteral("yes"), tr("Self-service"));
            addTagDescription(m_description, QStringLiteral("self_service"), QStringLiteral("no"), tr("No self-service"));
            addTagDescription(m_description, QStringLiteral("automated"), QStringLiteral("yes"), tr("Automated"));
            addTagDescription(m_description, QStringLiteral("automated"), QStringLiteral("no"), tr("Manual"));
        } else if (category == GeoDataPlacemark::AmenitySocialFacility) {
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("group_home"), tr("Group home"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("nursing_home"), tr("Nursing home"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("assisted_living"), tr("Assisted living", "Like group home but for more independent people, e.g. who have flats"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("day_care"), tr("Nursing services on a daily basis"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("shelter"), tr("Shelter"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("ambulatory_care"), tr("Ambulatory care"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("outreach"), tr("Social welfare services"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("workshop"), tr("Employment and workshops for offenders and people with disabilities"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("food_bank"), tr("Pre-packaged food for free or below market price"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("soup_kitchen"), tr("Prepared meals for free or below market price"));
            addTagDescription(m_description, QStringLiteral("social_facility"), QStringLiteral("dairy_kitchen"), tr("Free dairy food (subject to local regulations)"));

            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("abused"), tr("For abused"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("child"), tr("For children"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("disabled"), tr("For people with physical disabilities"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("diseased"), tr("For those who suffer of a disease"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("drug_addicted"), tr("For drug-addicted"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("homeless"), tr("For homeless"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("juvenile"), tr("For juvenile"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("mental_health"), tr("For those with mental/psychological problems"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("migrant"), tr("For migrants"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("orphan"), tr("For orphans"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("senior"), tr("For elder people"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("underprivileged"), tr("For poor or disadvantaged people"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("unemployed"), tr("For unemployed"));
            addTagDescription(m_description, QStringLiteral("social_facility:for"), QStringLiteral("victim"), tr("For victims of crimes"));
        } else if (category == GeoDataPlacemark::HistoricMemorial) {
            addTagValue(m_description, QStringLiteral("inscription"), tr("Inscription: %1"));
        } else if (category >= GeoDataPlacemark::AerialwayCableCar && category <= GeoDataPlacemark::AerialwayGoods) {
            addTagValue(m_description, QStringLiteral("occupancy"), tr("%1 places per carriage"));
            addTagValue(m_description, QStringLiteral("capacity"), tr("%1 people per hour"));
            addTagValue(m_description, QStringLiteral("duration"), tr("%1 minutes"));

            addTagDescription(m_description, QStringLiteral("bubble"), QStringLiteral("yes"), tr("Has weather protection", "A carriage is protected from the weather"));
            addTagDescription(m_description, QStringLiteral("bubble"), QStringLiteral("no"), tr("No weather protection", "A carriage is not protected from the weather"));
            addTagDescription(m_description, QStringLiteral("heating"), QStringLiteral("yes"), tr("Is heated", "A carriage is heated"));
            addTagDescription(m_description, QStringLiteral("heating"), QStringLiteral("no"), tr("Not heated", "A carriage is not heated"));
            addTagDescription(m_description, QStringLiteral("bicycle"), QStringLiteral("yes"), tr("Bicycle transportation possible", "Bicycles can be transported"));
            addTagDescription(m_description, QStringLiteral("bicycle"), QStringLiteral("summer"), tr("Bicycle transportation only in summer", "Bicycles can only be transported in summer"));
            addTagDescription(m_description, QStringLiteral("bicycle"), QStringLiteral("no"), tr("Bicycle transportation impossible", "Bicyles cannot be transported"));
        } else if (category >= GeoDataPlacemark::PisteDownhill && category <= GeoDataPlacemark::PisteSkiJump) {
            addTagDescription(m_description, QStringLiteral("lit"), QStringLiteral("yes"), tr("Lit at night"));
            addTagDescription(m_description, QStringLiteral("piste:lit"), QStringLiteral("yes"), tr("Lit in winter"));
            addTagDescription(m_description, QStringLiteral("gladed"), QStringLiteral("yes"), tr("Contains trees", "A ski piste with trees (gladed)"));
            addTagDescription(m_description, QStringLiteral("patrolled"), QStringLiteral("no"), tr("Not patrolled"));

            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("classic"), tr("Groomed for classic style nordic or downhill skiing"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("mogul"), tr("Mogul piste"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("skating"), tr("Groomed for free style or skating"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("classic;skating"), tr("Groomed for classic and free style skiing"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("classic+skating"), tr("Groomed for classic and free style skiing"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("scooter"), tr("Groomed by a smaller snowmobile"));
            addTagDescription(m_description, QStringLiteral("piste:grooming"), QStringLiteral("backcountry"), tr("Unmarked piste (backcountry touring)"));
        }

        if (category == GeoDataPlacemark::TransportBicycleParking || category == GeoDataPlacemark::TransportMotorcycleParking) {
            addTagDescription(m_description, QStringLiteral("covered"), QStringLiteral("yes"), tr("Is covered", "A parking space is covered"));
            addTagDescription(m_description, QStringLiteral("covered"), QStringLiteral("no"), tr("Not covered", "A parking space is not covered"));
        }

        if (category == GeoDataPlacemark::AmenityRecycling || category == GeoDataPlacemark::AmenityPostBox) {
            addTagValue(m_description, QStringLiteral("collection_times"), tr("Collection times %1"), QStringLiteral(", "));
        }

        if (category == GeoDataPlacemark::AerialwayStation) {
            addTagDescription(m_description, "aerialway:access", "entry", tr("Entry", "Entry station of an aerialway"));
            addTagDescription(m_description, "aerialway:access", "exit", tr("Exit", "Exit station of an aerialway"));
            addTagDescription(m_description, "aerialway:access", "both", tr("Entry and exit", "Entry and exit station of an aerialway"));
            addTagDescription(m_description, "aerialway:access", "no", tr("No entry or exit", "Transit only station of an aerialway"));
            addTagDescription(m_description, "aerialway:summer:access", "entry", tr("Entry during summer", "Entry station of an aerialway during summer"));
            addTagDescription(m_description, "aerialway:summer:access", "exit", tr("Exit during summer", "Exit station of an aerialway during summer"));
            addTagDescription(m_description, "aerialway:summer:access", "both", tr("Entry and exit during summer", "Entry and exit station of an aerialway during summer"));
            addTagDescription(m_description, "aerialway:summer:access", "no", tr("No entry or exit during summer", "Transit only station of an aerialway during summer"));
        }

        if (category != GeoDataPlacemark::AerialwayStation) {
            addTagValue(m_description, QStringLiteral("ele"), tr("Elevation: %1 m"));
        }

        addTagDescription(m_description, "access", "customers", tr("Customers only"));
        addTagDescription(m_description, QStringLiteral("access"), QStringLiteral("yes"), tr("Accessible by anyone", "The public has an official, legally-enshrined right of access; i.e., it's a right of way"));
        addTagDescription(m_description, QStringLiteral("access"), QStringLiteral("private"), tr("Private", "Only with permission of the owner on an individual basis."));
        addTagDescription(m_description, QStringLiteral("access"), QStringLiteral("permissive"), tr("Open to general traffic", "Open to general traffic but permission can be revoked by the owner"));
        addTagDescription(m_description, QStringLiteral("access"), QStringLiteral("no"), tr("No access", "No access for the general public"));


        addTagDescription(m_description, QStringLiteral("fee"), QStringLiteral("no"), tr("no fee"));
        addTagValue(m_description, QStringLiteral("description"));
        addTagValue(m_description, QStringLiteral("old_name"), tr("formerly <i>%1</i>"));

        const int level = m_placemark.osmData().tagValue(QStringLiteral("level")).toInt();
        if (level > 2) {
            addTagValue(m_description, QStringLiteral("level"), tr("Floor %1", "Positive floor level"));
        } else if (level < -2) {
            addTagValue(m_description, QStringLiteral("level"), tr("Basement %1", "Negative floor level"));
        } else {
            addTagDescription(m_description, QStringLiteral("level"), QStringLiteral("2"), tr("Floor 2", "Floor level 2, two levels above ground level"));
            addTagDescription(m_description, QStringLiteral("level"), QStringLiteral("1"), tr("Floor 1", "Floor level 1, one level above ground level"));
            addTagDescription(m_description, QStringLiteral("level"), QStringLiteral("0"), tr("Ground floor", "Floor level 0"));
            addTagDescription(m_description, QStringLiteral("level"), QStringLiteral("-1"), tr("Basement 1", "Floor level -1, one level below ground level"));
            addTagDescription(m_description, QStringLiteral("level"), QStringLiteral("-2"), tr("Basement 2", "Floor level -2, two levels below ground level"));
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

QString Placemark::website() const
{
    if (!m_website.isEmpty()) {
        return m_website;
    }
    auto const tags = QStringList() << "website" << "contact:website" << "facebook" << "contact:facebook" << "url";
    for(const QString &tag: tags) {
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

QString Placemark::wheelchairInfo() const
{
    if (!m_wheelchairInfo.isEmpty())
        return m_wheelchairInfo;

    addTagDescription(m_wheelchairInfo, QStringLiteral("wheelchair"), QStringLiteral("yes"), tr("Wheelchair accessible"));
    addTagDescription(m_wheelchairInfo, QStringLiteral("wheelchair"), QStringLiteral("no"), tr("Wheelchair inaccessible"));
    addTagDescription(m_wheelchairInfo, QStringLiteral("wheelchair"), QStringLiteral("limited"), tr("Limited wheelchair accessibility"));
    addTagDescription(m_wheelchairInfo, QStringLiteral("wheelchair"), QStringLiteral("designated"), tr("Designated wheelchair access"));

    // Check if there is localized description
    auto const & osmData = m_placemark.osmData();
    QStringList const uiLanguages = QLocale::system().uiLanguages();
    const QString tag = QLatin1String("wheelchair:description:");
    for (const QString &uiLanguage: uiLanguages) {
        for (auto tagIter = osmData.tagsBegin(), end = osmData.tagsEnd(); tagIter != end; ++tagIter) {
            if (tagIter.key().startsWith(tag)) {
                QStringRef const tagLanguage = tagIter.key().midRef(tag.length());
                if (tagLanguage == uiLanguage) {
                    append(m_wheelchairInfo, tagIter.value());
                    return m_wheelchairInfo;
                }
            }
        }
    }

    addTagValue(m_wheelchairInfo, "wheelchair:description");

    return m_wheelchairInfo;
}

QString Placemark::wifiAvailable() const
{
    if (!m_wifiAvailable.isEmpty()) {
        return m_wifiAvailable;
    }

    const auto& osmData = m_placemark.osmData();
    addTagDescription(m_wifiAvailable, QStringLiteral("internet_access"), QStringLiteral("no"), tr("No public Internet access", "This location does not provide public Internet access"));
    addTagDescription(m_wifiAvailable, QStringLiteral("internet_access"), QStringLiteral("yes"), tr("Public Internet access available", "This location provides an unknown type of public Internet access."));

    if (osmData.containsTag(QStringLiteral("internet_access:fee"), QStringLiteral("yes"))) {
        addTagDescription(m_wifiAvailable, QStringLiteral("internet_access"), QStringLiteral("wlan"), tr("Charged public wifi available", "Public wireless Internet access is available here for a fee."));
    } else if (osmData.containsTag(QStringLiteral("internet_access:fee"), QStringLiteral("no"))) {
        addTagDescription(m_wifiAvailable, QStringLiteral("internet_access"), QStringLiteral("wlan"), tr("Free public wifi available", "Public wireless Internet access is available here for no cost."));
    } else {
        addTagDescription(m_wifiAvailable, QStringLiteral("internet_access"), QStringLiteral("wlan"), tr("Public wifi available", "Public wireless Internet access is available here."));
    }

    if (m_wifiAvailable.isEmpty()) {
        addTagDescription(m_wifiAvailable, QStringLiteral("wifi"), QStringLiteral("no"), tr("No public wifi", "Public wifi is not available here."));
        addTagDescription(m_wifiAvailable, QStringLiteral("wifi"), QStringLiteral("yes"), tr("Public wifi available", "Public wireless Internet is available here."));
        addTagDescription(m_wifiAvailable, QStringLiteral("wifi"), QStringLiteral("free"), tr("Free public wifi available", "Public wireless Internet is available here for no cost."));
    }

    return m_wifiAvailable;
}

void Placemark::setName(const QString & name)
{
    if (m_placemark.displayName() == name) {
        return;
    }

    m_placemark.setName(name);
    emit nameChanged();
}

RouteRelationModel* Placemark::routeRelationModel()
{
    return &m_relationModel;
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

void Placemark::updateRelations(const Marble::GeoDataPlacemark &placemark)
{
    if (const auto document = (placemark.parent() ? geodata_cast<GeoDataDocument>(placemark.parent()) : 0)) {
        QVector<const GeoDataRelation*> allRelations;
        QSet<const GeoDataRelation*> relevantRelations;
        QSet<qint64> placemarkIds;
        auto const & osmData = placemark.osmData();
        placemarkIds << osmData.oid();
        bool searchRelations = true;
        for (auto feature: document->featureList()) {
            if (const auto relation = geodata_cast<GeoDataRelation>(feature)) {
                allRelations << relation;
                if (relation->memberIds().contains(osmData.oid())) {
                    relevantRelations << relation;
                    auto const isRoute = relation->osmData().tagValue(QStringLiteral("type")) == QStringLiteral("route");
                    searchRelations &= !isRoute;
                }
            }
        }
        if (searchRelations) {
            for (auto feature: document->featureList()) {
                if (const auto relation = geodata_cast<GeoDataRelation>(feature)) {
                    if (relevantRelations.contains(relation) &&
                            relation->osmData().containsTag(QStringLiteral("type"), QStringLiteral("public_transport")) &&
                            relation->osmData().containsTag(QStringLiteral("public_transport"), QStringLiteral("stop_area"))) {
                        for (auto iter = relation->osmData().relationReferencesBegin(), end = relation->osmData().relationReferencesEnd();
                             iter != end; ++iter) {
                            if (iter.value() == QStringLiteral("stop") || iter.value() == QStringLiteral("platform")) {
                                placemarkIds << iter.key();
                            }
                        }
                    }
                }
            }
        }
        for (auto relation: allRelations) {
            if (relation->containsAnyOf(placemarkIds)) {
                relevantRelations << relation;
            }
        }
        m_relationModel.setRelations(relevantRelations);
    }
}

}

#include "moc_Placemark.cpp"
