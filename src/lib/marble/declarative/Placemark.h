// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_PLACEMARK_H
#define MARBLE_DECLARATIVE_PLACEMARK_H

#include "GeoDataPlacemark.h"
#include "RouteRelationModel.h"

#include <QObject>
#include <QStringListModel>
#include <QUrl>
#include <qqmlregistration.h>

namespace Marble
{

/**
 * Wraps a GeoDataPlacemark for QML access
 */
class Placemark : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name WRITE setName READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString address READ address NOTIFY addressChanged)
    Q_PROPERTY(QString website READ website NOTIFY websiteChanged)
    Q_PROPERTY(QString wikipedia READ wikipedia NOTIFY wikipediaChanged)
    Q_PROPERTY(QString openingHours READ openingHours NOTIFY openingHoursChanged)
    Q_PROPERTY(QString coordinates READ coordinates NOTIFY coordinatesChanged)
    Q_PROPERTY(QString wheelchairInfo READ wheelchairInfo NOTIFY wheelchairInfoChanged)
    Q_PROPERTY(QString wifiAvailable READ wifiAvailable NOTIFY wifiAvailabilityChanged)
    Q_PROPERTY(QString phone READ phone NOTIFY phoneChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY coordinatesChanged)
    Q_PROPERTY(double latitude READ latitude NOTIFY coordinatesChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)

    Q_PROPERTY(RouteRelationModel *routeRelationModel READ routeRelationModel CONSTANT)

public:
    /** Constructor */
    explicit Placemark(QObject *parent = nullptr);

    void setGeoDataPlacemark(const Marble::GeoDataPlacemark &placemark);

    Marble::GeoDataPlacemark &placemark();
    const Marble::GeoDataPlacemark &placemark() const;

    QString name() const;
    QString description() const;
    QString address() const;
    QString website() const;
    QString wikipedia() const;
    QString openingHours() const;
    QString coordinates() const;
    QString wheelchairInfo() const;
    QString wifiAvailable() const;
    QString phone() const;
    double longitude() const;
    double latitude() const;
    const QStringList &tags() const;

    RouteRelationModel *routeRelationModel();

public Q_SLOTS:
    void setName(const QString &name);

Q_SIGNALS:
    void nameChanged();
    void coordinatesChanged();
    void descriptionChanged();
    void addressChanged();
    void websiteChanged();
    void wikipediaChanged();
    void openingHoursChanged();
    void wheelchairInfoChanged();
    void wifiAvailabilityChanged();
    void phoneChanged();
    void tagsChanged();

    void routeRelationModelChanged();

private:
    bool addTagValue(QString &target, const QString &key, const QString &format = QString(), const QString &separator = QStringLiteral(" · ")) const;
    void addFirstTagValueOf(QString &target, const QStringList &keys) const;
    void addTagDescription(QString &target, const QString &key, const QString &value, const QString &description) const;
    static void append(QString &target, const QString &value);
    QString addressFromOsmData() const;
    static QString formatStreet(const QString &street, const QString &houseNumber);
    void updateTags();
    void updateRelations(const GeoDataPlacemark &placemark);

    Marble::GeoDataPlacemark m_placemark;
    mutable QString m_address; // mutable to allow lazy calculation in the getter
    mutable QString m_description;
    mutable QString m_website;
    mutable QString m_wikipedia;
    mutable QString m_openingHours;
    mutable QString m_wheelchairInfo;
    mutable QString m_wifiAvailable;
    mutable QString m_phone;
    QStringList m_tags;

    RouteRelationModel m_relationModel;
};

}

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
