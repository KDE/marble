// Copyright 2009 Henry de Valence <hdevalence@gmail.com>
// Copyright 2009 David Roberts <dvdr18@gmail.com>
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//

#ifndef MARBLE_PLANET_H
#define MARBLE_PLANET_H

#include "marble_export.h"

#include <QtCore/QStringList>

namespace Marble
{

class PlanetPrivate;

class MARBLE_EXPORT Planet
{
//Not a QObject because we don't need sigs/slots
public:

    /** Constructor to use for a custom planet
     * All of the orbital elements are left empty, so you can fill them in yourself.
     */
    Planet();

    ///Constructs a planet with @p id, filling orbital elements
    explicit Planet( const QString& id );

    ///Copy Constructor
    Planet( const Planet& other );
    ///Destructor
    ~Planet();

    /* Getter functions */
    /// for calculating mean anomaly
    qreal M_0() const;
    qreal M_1() const;

    /// for calculating equation of center
    qreal C_1() const;
    qreal C_2() const;
    qreal C_3() const;
    qreal C_4() const;
    qreal C_5() const;
    qreal C_6() const;

    /// ecliptic longitude of the perihelion
    qreal Pi() const;

    /// obliquity of the ecliptic plane
    qreal epsilon() const;

    /// for calculating sidereal time
    qreal theta_0() const;
    qreal theta_1() const;

    /// the radius of the planet, in metres
    qreal radius() const;

    /** The user visible name of the planet */
    QString name() const;
    /** The internal, nonlocalized name of the planet */
    QString id() const;

    /* Setter functions */
    void setM_0( qreal M_0 );
    void setM_1( qreal M_1 );

    void setC_1( qreal C_1 );
    void setC_2( qreal C_2 );
    void setC_3( qreal C_3 );
    void setC_4( qreal C_4 );
    void setC_5( qreal C_5 );
    void setC_6( qreal C_6 );

    void setPi( qreal Pi );

    void setEpsilon( qreal epsilon );

    void setTheta_0( qreal theta_0 );
    void setTheta_1( qreal theta_1 );

    void setRadius( qreal radius );

    void setName( const QString& name );
    void setId( const QString& id );

    Planet& operator=( const Planet& rhs );

    static QString name( const QString& id );
    static QStringList planetList();

private:
    PlanetPrivate * const d;
};



} //namespace Marble


#endif
