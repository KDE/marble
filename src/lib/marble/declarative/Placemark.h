//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_PLACEMARK_H
#define MARBLE_DECLARATIVE_PLACEMARK_H

#include "Coordinate.h"

#include "GeoDataPlacemark.h"
#include "GeoDataFeature.h"

#include <QObject>
#include <QtQml>

namespace Marble {

/**
  * Wraps a GeoDataPlacemark for QML access
  */
class Placemark : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString name WRITE setName READ name NOTIFY nameChanged )
    Q_PROPERTY( QString description READ description NOTIFY descriptionChanged )
    Q_PROPERTY( QString address READ address NOTIFY addressChanged )
    Q_PROPERTY( QString website READ website NOTIFY websiteChanged)
    Q_PROPERTY( QString wikipedia READ wikipedia NOTIFY wikipediaChanged)
    Q_PROPERTY( QString fuelDetails READ fuelDetails NOTIFY fuelDetailsChanged)
    Q_PROPERTY( QString openingHours READ openingHours NOTIFY openingHoursChanged)

public:
    /** Constructor */
    explicit Placemark( QObject *parent = 0 );

    void setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark );

    Marble::GeoDataPlacemark & placemark();

    QString name() const;
    QString description() const;
    QString address() const;
    QString website() const;
    QString wikipedia() const;
    QString fuelDetails() const;
    QString openingHours() const;

public Q_SLOTS:
    void setName(const QString &name);

Q_SIGNALS:
    void nameChanged();
    void coordinatesChanged();
    void descriptionChanged();
    void addressChanged();
    void websiteChanged();
    void wikipediaChanged();
    void fuelDetailsChanged();
    void openingHoursChanged();

private:
    QString categoryName(GeoDataFeature::GeoDataVisualCategory category) const;
    void addTagValue(const QString &key) const;
    void addTagDescription(QString &target, const QString &key, const QString &value, const QString &description) const;
    QString addressFromExtendedData() const;
    QString addressFromOsmData() const;
    QString formatStreet(const QString &street, const QString &houseNumber) const;

    Marble::GeoDataPlacemark m_placemark;
    mutable QString m_address; // mutable to allow lazy calculation in the getter
    mutable QString m_description;
    mutable QString m_fuelDetails;
    mutable QString m_website;
    mutable QString m_wikipedia;
    mutable QString m_openingHours;
};

}

QML_DECLARE_TYPE(Marble::Placemark)

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
