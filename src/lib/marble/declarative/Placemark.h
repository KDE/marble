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
#include <QAbstractListModel>
#include <QQmlComponent>

namespace Marble {

/**
  * Wraps a GeoDataPlacemark for QML access
  */
class Placemark : public QObject
{
    Q_OBJECT

    Q_PROPERTY( Coordinate* coordinate READ coordinate NOTIFY coordinateChanged )
    Q_PROPERTY( QString name WRITE setName READ name NOTIFY nameChanged )
    Q_PROPERTY( QString description READ description NOTIFY descriptionChanged )
    Q_PROPERTY( QString address READ address NOTIFY addressChanged )

public:
    /** Constructor */
    explicit Placemark( QObject *parent = 0 );

    void setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark );

    Marble::GeoDataPlacemark placemark() const;

    Coordinate* coordinate();

    QString name() const;
    QString description() const;
    QString address() const;

public Q_SLOTS:
    void setName(const QString &name);

Q_SIGNALS:
    void coordinateChanged();

    void nameChanged();
    void descriptionChanged();
    void addressChanged();

private:
    QString categoryName(GeoDataFeature::GeoDataVisualCategory category) const;
    void addTagValue(const QString &key) const;
    void addTagDescription(const QString &key, const QString &value, const QString &description) const;

    Marble::GeoDataPlacemark m_placemark;
    Coordinate m_coordinate;
    mutable QString m_address; // mutable to allow lazy calculation in the getter
    mutable QString m_description;
};

}

QML_DECLARE_TYPE( Marble::Placemark )

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
