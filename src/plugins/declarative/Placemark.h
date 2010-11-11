//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_PLACEMARK_H
#define MARBLE_DECLARATIVE_PLACEMARK_H

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

    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( qreal longitude READ longitude WRITE setLongitude )
    Q_PROPERTY( qreal latitude READ latitude WRITE setLatitude )

public:
    /** Constructor */
    Placemark( const QString &name, qreal lon, qreal lat, QObject *parent = 0 );

    /** Provides access to the placemark name */
    QString name() const;

    /** Change the placemark's name */
    void setName( const QString &name );

    /** Provides access to the longitude of the placemark */
    qreal longitude() const;

    /** Change the longitude of the placemark */
    void setLongitude( qreal lon );

    /** Provides access to the latitude of the placemark */
    qreal latitude() const;

    /** Change the latitude of the placemark */
    void setLatitude( qreal lat );

private:
    QString m_name;

    qreal m_longitude;

    qreal m_latitude;
};

}
}

#endif // MARBLE_DECLARATIVE_PLACEMARK_H
