//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_PLACEMARK_H
#define MARBLE_DECLARATIVE_PLACEMARK_H

#include "Coordinate.h"

#include <QtCore/QObject>

namespace Marble
{
namespace Declarative
{

/**
  * Represents a placemark with the properties of a name and coordinates
  *
  * @todo: Introduce GeoDataCoordinates
  */
class Placemark : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( Marble::Declarative::Coordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged )

public:
    /** Constructor */
    Placemark( const QString &name, Coordinate *coordinate, QObject *parent = 0 );

    /** Provides access to the placemark name */
    QString name() const;

    /** Change the placemark's name */
    void setName( const QString &name );

    /** Returns the coordinate of the place */
    Coordinate *coordinate();

    /** Change the coordinate of the placemark */
    void setCoordinate( Coordinate *coordinate );

Q_SIGNALS:
    void nameChanged();
    void coordinateChanged();

private:
    QString m_name;

    Coordinate m_coordinate;
};

}
}

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
