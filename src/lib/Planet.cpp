//Copyright 2009 Henry de Valence <hdevalence@gmail.com>
//Copyright 2009 David Roberts <dvdr18@gmail.com>
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
#include "Planet.h"

namespace Marble {

class PlanetPrivate
{
public:
    qreal M_0, M_1; // for calculating mean anomaly
    qreal C_1, C_2, C_3, C_4, C_5, C_6; // for calculating equation of center
    qreal Pi; // ecliptic longitude of the perihelion
    qreal epsilon; // obliquity of the ecliptic plane
    qreal theta_0, theta_1; // for calculating sidereal time
    qreal radius; //in metres
};


//Constructor
Planet::Planet(qreal M_0, qreal M_1, qreal C_1, qreal C_2, qreal C_3, qreal C_4,
               qreal C_5, qreal C_6, qreal Pi, qreal epsilon, qreal theta_0,
               qreal theta_1, qreal radius )
{
    d = new PlanetPrivate;
    d->M_0 = M_0;
    d->M_1 = M_1;
    d->C_1 = C_1;
    d->C_2 = C_2;
    d->C_3 = C_3;
    d->C_4 = C_4;
    d->C_5 = C_5;
    d->C_6 = C_6;
    d->Pi = Pi;
    d->epsilon = epsilon;
    d->theta_0 = theta_0;
    d->theta_1 = theta_1;
    d->radius = radius;
}

//Copy Constructor
Planet::Planet( const Planet& other )
{
    d = new PlanetPrivate;
    d->M_0 = other.M_0();
    d->M_1 = other.M_1();
    d->C_1 = other.C_1();
    d->C_2 = other.C_2();
    d->C_3 = other.C_3();
    d->C_4 = other.C_4();
    d->C_5 = other.C_5();
    d->C_6 = other.C_6();
    d->Pi = other.Pi();
    d->epsilon = other.epsilon();
    d->theta_0 = other.theta_0();
    d->theta_1 = other.theta_1();
    d->radius = other.radius();
}
//Destructor
Planet::~Planet()
{
    delete d;
}


    /* Getter functions */
    // for calculating mean anomaly
qreal Planet::M_0() const
{
    return d->M_0;
}
qreal Planet::M_1() const
{
    return d->M_1;
}

    // for calculating equation of center
qreal Planet::C_1() const
{
    return d->C_1;
}
qreal Planet::C_2() const
{
    return d->C_2;
}
qreal Planet::C_3() const
{
    return d->C_3;
}
qreal Planet::C_4() const
{
    return d->C_4;
}
qreal Planet::C_5() const
{
    return d->C_5;
}
qreal Planet::C_6() const
{
    return d->C_6;
}

    // ecliptic longitude of the perihelion
qreal Planet::Pi() const
{
    return d->Pi;
}

    // obliquity of the ecliptic plane
qreal Planet::epsilon() const
{
    return d->epsilon;
}

    // for calculating sidereal time
qreal Planet::theta_0() const
{
    return d->theta_0;
}
qreal Planet::theta_1() const
{
    return d->theta_1;
}

    // the radius of the planet, in metres
qreal Planet::radius() const
{
    return d->radius;
}

    /* Setter functions */
void Planet::setM_0( qreal M_0 )
{
    d->M_0 = M_0;
}
void Planet::setM_1( qreal M_1 )
{
    d->M_1 = M_1;
}

void Planet::setC_1( qreal C_1 )
{
    d->C_1 = C_1;
}
void Planet::setC_2( qreal C_2 )
{
    d->C_2 = C_2;
}
void Planet::setC_3( qreal C_3 )
{
    d->C_3 = C_3;
}
void Planet::setC_4( qreal C_4 )
{
    d->C_4 = C_4;
}
void Planet::setC_5( qreal C_5 )
{
    d->C_5 = C_5;
}
void Planet::setC_6( qreal C_6 )
{
    d->C_6 = C_6;
}

void Planet::setPi( qreal Pi )
{
    d->Pi = Pi;
}

void Planet::setEpsilon( qreal epsilon )
{
    d->epsilon = epsilon;
}

void Planet::setTheta_0( qreal theta_0 )
{
    d->theta_0 = theta_0;
}
void Planet::setTheta_1( qreal theta_1 )
{
    d->theta_1 = theta_1;
}

void Planet::setRadius( qreal radius )
{
    d->radius = radius;
}

void Planet::operator=(const Planet& other)
{
    d->M_0 = other.M_0();
    d->M_1 = other.M_1();
    d->C_1 = other.C_1();
    d->C_2 = other.C_2();
    d->C_3 = other.C_3();
    d->C_4 = other.C_4();
    d->C_5 = other.C_5();
    d->C_6 = other.C_6();
    d->Pi = other.Pi();
    d->epsilon = other.epsilon();
    d->theta_0 = other.theta_0();
    d->theta_1 = other.theta_1();
    d->radius = other.radius();
}


} //namespace Marble

