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

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QDebug>

#include "global.h"

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
    QString name, target; //localized and nonlocalized names
};


//Constructor
Planet::Planet(qreal M_0, qreal M_1, qreal C_1, qreal C_2, qreal C_3, qreal C_4,
               qreal C_5, qreal C_6, qreal Pi, qreal epsilon, qreal theta_0,
               qreal theta_1, qreal radius, const QString& name,
               const QString& target )
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
    d->name = name;
    d->target = target;
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
    d->name = other.name();
    d->target = other.target();
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

QString Planet::name() const
{
    return d->name;
}

QString Planet::target() const
{
    return d->target;
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

void Planet::setName( const QString& name )
{
    d->name = name;
}

void Planet::setTarget( const QString& target )
{
    d->target = target;
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
    d->name = other.name();
    d->target = other.target();
}

//Planet Creation stuff:

QHash<QString,Planet*> *existingPlanets = 0;

const Planet* Planet::addPlanet(Planet *planet, const QString& target,
                                bool overwrite)
{
    //Check if the planet registry exists
    if( !existingPlanets )
        existingPlanets = new QHash<QString,Planet*>();

    //If we're not to overwrite an existing planet and there is one, fail.
    bool exists = existingPlanets->contains( target );
    if( exists && !overwrite )
        return 0;

    if( !exists ) {
        //We make a copy to protect against e.g. the original one going out of
        //scope or some other unexpected circumstance.
        Planet *newplanet = new Planet( *planet );
        existingPlanets->insert( target, newplanet );
        //Return a pointer to the new planet.
        return newplanet;
    } else {
        //Doing it this way means everything still using the existing pointer
        //gets the new value.
        Planet *oldplanet = existingPlanets->value( target );
        *oldplanet = *planet;
        //Return a pointer to the old planet with new values.
        return oldplanet;
    }

}

const Planet* Planet::planetByName( const QString& target )
{
    //qDebug() << "Slartibartfast making planet" << target;
    //Check if the planet registry exists
    if( !existingPlanets )
        existingPlanets = new QHash<QString,Planet*>();

    //If there's no existing planet, return a null pointer
    Planet *p = existingPlanets->value( target, 0 );
    if( p ) {
        //Success, we have this planet already made!
        //qDebug() << "Planet already exists!";
        return p;
    } else {
        //qDebug() << "Manufacturing custom planet";
        // constants taken from
        // http://www.astro.uu.nl/~strous/AA/en/reken/zonpositie.html
        if ( target == "mercury" ) {
            p = new Planet( (174.7948*DEG2RAD), (4.09233445*DEG2RAD),
                    (23.4400*DEG2RAD), (2.9818*DEG2RAD), (0.5255*DEG2RAD),
                    (0.1058*DEG2RAD), (0.0241*DEG2RAD), (0.0055*DEG2RAD),
                    (111.5943*DEG2RAD), (0.02*DEG2RAD), (13.5964*DEG2RAD),
                    (6.1385025*DEG2RAD), 2440000.0,
                    QObject::tr("Mercury", "the planet"), QString("mercury") );
        } else if ( target == "venus" ) {
            p = new Planet( (50.4161*DEG2RAD), (1.60213034*DEG2RAD),
                    (0.7758*DEG2RAD), (0.0033*DEG2RAD), (0.0000*DEG2RAD),
                    (0.0000*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (73.9519*DEG2RAD), (2.64*DEG2RAD), (215.2995*DEG2RAD),
                    (-1.4813688*DEG2RAD), 6051800.0,
                    QObject::tr("Venus", "the planet"), QString("venus") );
        } else if ( target == "earth" ) {
            p = new Planet( (357.5291*DEG2RAD), (0.98560028*DEG2RAD),
                    (1.9148*DEG2RAD), (0.0200*DEG2RAD), (0.0003*DEG2RAD),
                    (0.0000*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (102.9372*DEG2RAD), (23.45*DEG2RAD), (280.1600*DEG2RAD),
                    (360.9856235*DEG2RAD), 6378000.0,
                    QObject::tr("Earth", "the planet"), QString("earth") );
        } else if ( target == "mars" ) {
            p = new Planet( (19.3730*DEG2RAD), (0.52402068*DEG2RAD),
                    (10.6912*DEG2RAD), (0.6228*DEG2RAD), (0.0503*DEG2RAD),
                    (0.0046*DEG2RAD), (0.0005*DEG2RAD), (0.0000*DEG2RAD),
                    (70.9812*DEG2RAD), (25.19*DEG2RAD), (313.4803*DEG2RAD),
                    (350.89198226*DEG2RAD), 3397000.0,
                    QObject::tr("Mars", "the planet"), QString("mars") );
        } else if ( target == "jupiter" ) {
            p = new Planet( (20.0202*DEG2RAD), (0.08308529*DEG2RAD),
                    (5.5549*DEG2RAD), (0.1683*DEG2RAD), (0.0071*DEG2RAD),
                    (0.0003*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (237.2074*DEG2RAD), (3.12*DEG2RAD), (146.0727*DEG2RAD),
                    (870.5366420*DEG2RAD), 71492000.0,
                    QObject::tr("Jupiter", "the planet"), QString("jupiter") );
        } else if ( target == "saturn" ) {
            p = new Planet( (317.0207*DEG2RAD), (0.03344414*DEG2RAD),
                    (6.3585*DEG2RAD), (0.2204*DEG2RAD), (0.0106*DEG2RAD),
                    (0.0006*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (99.4571*DEG2RAD), (26.74*DEG2RAD), (174.3479*DEG2RAD),
                    (810.7939024*DEG2RAD), 60268000.0,
                    QObject::tr("Saturn", "the planet"), QString("saturn") );
        } else if ( target == "uranus" ) {
            p = new Planet( (141.0498*DEG2RAD), (0.01172834*DEG2RAD),
                    (5.3042*DEG2RAD), (0.1534*DEG2RAD), (0.0062*DEG2RAD),
                    (0.0003*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (5.4639*DEG2RAD), (82.22*DEG2RAD), (17.9705*DEG2RAD),
                    (-501.1600928*DEG2RAD), 25559000.0,
                    QObject::tr("Uranus", "the planet"), QString("uranus") );
        } else if ( target == "neptune" ) {
            p = new Planet( (256.2250*DEG2RAD), (0.00598103*DEG2RAD),
                    (1.0302*DEG2RAD), (0.0058*DEG2RAD), (0.0000*DEG2RAD),
                    (0.0000*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
                    (182.1957*DEG2RAD), (27.84*DEG2RAD), (52.3996*DEG2RAD),
                    (536.3128492*DEG2RAD), 24766000.0,
                    QObject::tr("Neptune", "the planet"), QString("neptune") );
        // dwarf planets ... (everybody likes pluto)
        } else if ( target == "pluto" ) {
            p = new Planet( (14.882*DEG2RAD), (0.00396*DEG2RAD),
                    (28.3150*DEG2RAD), (4.3408*DEG2RAD), (0.9214*DEG2RAD),
                    (0.2235*DEG2RAD), (0.0627*DEG2RAD), (0.0174*DEG2RAD),
                    (4.5433*DEG2RAD), (57.46*DEG2RAD), (56.3183*DEG2RAD),
                    (-56.3623195*DEG2RAD), 1151000.0,
                    QObject::tr("Pluto", "the planet"), QString("pluto") );
        // sun and moon
        } else if ( target == "sun" ) {
            qDebug() << "WARNING:";
            qDebug() << "creating \"sun\" which has invalid orbital elements";
            //FIXME: fill in with valid data
            p = new Planet( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 695000000.0,
                    QObject::tr("Sun", "the earth's star"), QString("sun") );
        } else if ( target == "moon" ) {
            qDebug() << "WARNING:";
            qDebug() << "creating \"moon\" which has invalid orbital elements";
            //FIXME: fill in with valid data
            p = new Planet( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1738000.0,
                    QObject::tr("Moon", "the earth's moon"), QString("moon") );
        }

        //If we created a planet, add it to the list for later
        if( p )
            existingPlanets->insert( target, p );
        else {
            qDebug() << "Warning: couldn't create planet" << target;
            qDebug() << "Returning 0, hope it doesn't crash";
        }

        //If we didn't make a valid planet, return 0
        return p;
    }
}

} //namespace Marble

