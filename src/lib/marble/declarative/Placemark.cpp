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

#include <GeoDataExtendedData.h>
#include <QGeoAddress>
#include <QLocale>

Placemark::Placemark(QObject *parent ) :
    QObject( parent )
{
    // nothing to do
}

void Placemark::setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark )
{
    m_placemark = placemark;
    m_coordinate.setCoordinates( placemark.coordinate() );
    emit coordinateChanged();
    emit nameChanged();
    emit addressChanged();
}

Marble::GeoDataPlacemark Placemark::placemark() const
{
    return m_placemark;
}

Coordinate *Placemark::coordinate()
{
    return &m_coordinate;
}

QString Placemark::name() const
{
    return m_placemark.name();
}

QString Placemark::address() const
{
    if (m_address.isEmpty()) {
        QGeoAddress address;
        Marble::GeoDataExtendedData data = m_placemark.extendedData();
        address.setCountry(data.value("country").value().toString());
        address.setState(data.value("state").value().toString());
        address.setCounty(data.value("county").value().toString());

        QString city = data.value("city").value().toString();
        if (city.isEmpty()) {
            city = data.value("town").value().toString();
        }
        if (city.isEmpty()) {
            city = data.value("village").value().toString();
        }
        if (city.isEmpty()) {
            city = data.value("hamlet").value().toString();
        }
        address.setCity(city);
        address.setDistrict(data.value("district").value().toString());
        address.setPostalCode(data.value("postcode").value().toString());

        if (data.value("class").value().toString() != "highway") {
            // Do not set the street for streets itself -- the placemark will have the street name included already

            // Unfortunately QGeoAddress cannot handle house number / street name ordering via its API,
            // so we have to fall back to our own translations
            QString const street = data.value("road").value().toString();
            QString const houseNumber = data.value("house_number").value().toString();
            QString const fullStreet = houseNumber.isEmpty() ? street : tr("%1 %2",
                "House number (first argument) and street name (second argument) in an adress").arg(houseNumber).arg(street).trimmed();
            address.setStreet(fullStreet);
        }

        // @todo FIXME Unfortunately QGeoAddress docs claim it wants a three-letter country code that neither OSM nor QLocale provide
        // address.setCountryCode(QLocale::system().name());
        // address.setCountryCode(data.value("country_code").value().toString());

        QString const addressString = address.text().replace("<br/>", ", ");
        m_address = addressString.isEmpty() ? m_placemark.address() : addressString;
    }

    return m_address;
}

void Placemark::setName(const QString & name)
{
    if (m_placemark.name() == name) {
        return;
    }

    m_placemark.setName(name);
    emit nameChanged();
}

#include "moc_Placemark.cpp"
