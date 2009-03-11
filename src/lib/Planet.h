//Copyright 2009 Henry de Valence <hdevalence@gmail.com>
//Copyright 2009 David Roberts <dvdr18@gmail.com>
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//

#ifndef PLANET_H
#define PLANET_H

#include "marble_export.h"

namespace Marble {

class PlanetPrivate;

class MARBLE_EXPORT Planet
{
//Not a QObject because we don't need sigs/slots
public:

    //Constructor
    Planet( qreal M_0, qreal M_1, qreal C_1, qreal C_2, qreal C_3, qreal C_4,
            qreal C_5, qreal C_6, qreal Pi, qreal epsilon, qreal theta_0,
            qreal theta_1, qreal radius, const QString& name,
            const QString& target );

    //Copy Constructor
    Planet( const Planet& other );
    //Destructor
    ~Planet();


    /* Getter functions */
    // for calculating mean anomaly
    qreal M_0() const;
    qreal M_1() const;

    // for calculating equation of center
    qreal C_1() const;
    qreal C_2() const;
    qreal C_3() const;
    qreal C_4() const;
    qreal C_5() const;
    qreal C_6() const;

    // ecliptic longitude of the perihelion
    qreal Pi() const;

    // obliquity of the ecliptic plane
    qreal epsilon() const;

    // for calculating sidereal time
    qreal theta_0() const;
    qreal theta_1() const;

    // the radius of the planet, in metres
    qreal radius() const;

    /** The user visible name of the planet */
    QString name() const;
    /** The internal, nonlocalized name of the planet */
    QString target() const;

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
    void setTarget( const QString& target );

    void operator=( const Planet& other );


    /**
    Returns a pointer to a shared Planet object.
    Use this to create planets.
    @param target the (nonlocalized) name of the planet
    @return a pointer to a shared instance of the planet
    @note if you wish to modify an existing planet, you may get a const Planet
    object here and then use the Planet copy constructor. @see addPlanet()
    */
    static const Planet* planetByName(const QString& target);
    /**
    Adds a custom planet to the list.
    @param planet a pointer to the new custom planet. Ownership of this pointer
    is <b>not</b> transferred
    @param target the nonlocalized name of the new planet
    @param overwrite if false, addPlanet() will not overwrite an existing
    planet of the same name
    @return a pointer to the planet that was added. If adding the planet failed,
    it returns a null pointer. Note that it will never equal @param planet
    since if there is an existing planet and @p overwrite is true, it assigns
    the exisiting planet the value of @p planet, so anyone already using that
    planet gets the new value, and if there was no exisitng planet, it makes a
    copy of @p planet
    */
    static const Planet* addPlanet( Planet *planet, const QString& target,
                                    bool overwrite = false );

private:
    PlanetPrivate * d;
};



} //namespace Marble


#endif
