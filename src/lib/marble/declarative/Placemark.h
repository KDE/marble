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

#include <QObject>
#include <QAbstractListModel>
#include <QQmlComponent>

/**
  * Wraps a GeoDataPlacemark for QML access
  */
class Placemark : public QObject
{
    Q_OBJECT

    Q_PROPERTY( Coordinate* coordinate READ coordinate NOTIFY coordinateChanged )
    Q_PROPERTY( QString name WRITE setName READ name NOTIFY nameChanged )
    Q_PROPERTY( QString address READ address NOTIFY addressChanged )

public:
    /** Constructor */
    explicit Placemark( QObject *parent = 0 );

    void setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark );

    Marble::GeoDataPlacemark placemark() const;

    Coordinate* coordinate();

    QString name() const;
    QString address() const;

public slots:
    void setName(const QString &name);

Q_SIGNALS:
    void coordinateChanged();

    void nameChanged();
    void addressChanged();

private:
    Marble::GeoDataPlacemark m_placemark;

    Coordinate m_coordinate;
};

QML_DECLARE_TYPE( Placemark )

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
