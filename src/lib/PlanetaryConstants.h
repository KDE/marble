// Copyright 2009 David Roberts <dvdr18@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


#ifndef PLANETARYCONSTANTS_H
#define PLANETARYCONSTANTS_H

#include "global.h"
using namespace Marble;

struct PlanetaryConstants {
    qreal M_0, M_1; // for calculating mean anomaly
    qreal C_1, C_2, C_3, C_4, C_5, C_6; // for calculating equation of center
    qreal Pi; // ecliptic longitude of the perihelion
    qreal epsilon; // obliquity of the ecliptic plane
    qreal theta_0, theta_1; // for calculating sidereal time
};

// constants taken from
// http://www.astro.uu.nl/~strous/AA/en/reken/zonpositie.html
PlanetaryConstants PC_MERCURY = {
    (174.7948*DEG2RAD), (4.09233445*DEG2RAD),
    (23.4400*DEG2RAD), (2.9818*DEG2RAD), (0.5255*DEG2RAD), (0.1058*DEG2RAD),
    (0.0241*DEG2RAD), (0.0055*DEG2RAD),
    (111.5943*DEG2RAD),
    (0.02*DEG2RAD),
    (13.5964*DEG2RAD), (6.1385025*DEG2RAD)
};
PlanetaryConstants PC_VENUS = {
    (50.4161*DEG2RAD), (1.60213034*DEG2RAD),
    (0.7758*DEG2RAD), (0.0033*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (73.9519*DEG2RAD),
    (2.64*DEG2RAD),
    (215.2995*DEG2RAD), (-1.4813688*DEG2RAD)
};
PlanetaryConstants PC_EARTH = {
    (357.5291*DEG2RAD), (0.98560028*DEG2RAD),
    (1.9148*DEG2RAD), (0.0200*DEG2RAD), (0.0003*DEG2RAD), (0.0000*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (102.9372*DEG2RAD),
    (23.45*DEG2RAD),
    (280.1600*DEG2RAD), (360.9856235*DEG2RAD),
};
PlanetaryConstants PC_MARS = {
    (19.3730*DEG2RAD), (0.52402068*DEG2RAD),
    (10.6912*DEG2RAD), (0.6228*DEG2RAD), (0.0503*DEG2RAD), (0.0046*DEG2RAD),
    (0.0005*DEG2RAD), (0.0000*DEG2RAD),
    (70.9812*DEG2RAD),
    (25.19*DEG2RAD),
    (313.4803*DEG2RAD), (350.89198226*DEG2RAD)
};
PlanetaryConstants PC_JUPITER = {
    (20.0202*DEG2RAD), (0.08308529*DEG2RAD),
    (5.5549*DEG2RAD), (0.1683*DEG2RAD), (0.0071*DEG2RAD), (0.0003*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (237.2074*DEG2RAD),
    (3.12*DEG2RAD),
    (146.0727*DEG2RAD), (870.5366420*DEG2RAD)
};
PlanetaryConstants PC_SATURN = {
    (317.0207*DEG2RAD), (0.03344414*DEG2RAD),
    (6.3585*DEG2RAD), (0.2204*DEG2RAD), (0.0106*DEG2RAD), (0.0006*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (99.4571*DEG2RAD),
    (26.74*DEG2RAD),
    (174.3479*DEG2RAD), (810.7939024*DEG2RAD)
};
PlanetaryConstants PC_URANUS = {
    (141.0498*DEG2RAD), (0.01172834*DEG2RAD),
    (5.3042*DEG2RAD), (0.1534*DEG2RAD), (0.0062*DEG2RAD), (0.0003*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (5.4639*DEG2RAD),
    (82.22*DEG2RAD),
    (17.9705*DEG2RAD), (-501.1600928*DEG2RAD)
};
PlanetaryConstants PC_NEPTUNE = {
    (256.2250*DEG2RAD), (0.00598103*DEG2RAD),
    (1.0302*DEG2RAD), (0.0058*DEG2RAD), (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (0.0000*DEG2RAD), (0.0000*DEG2RAD),
    (182.1957*DEG2RAD),
    (27.84*DEG2RAD),
    (52.3996*DEG2RAD), (536.3128492*DEG2RAD)
};
PlanetaryConstants PC_PLUTO = {
    (14.882*DEG2RAD), (0.00396*DEG2RAD),
    (28.3150*DEG2RAD), (4.3408*DEG2RAD), (0.9214*DEG2RAD), (0.2235*DEG2RAD),
    (0.0627*DEG2RAD), (0.0174*DEG2RAD),
    (4.5433*DEG2RAD),
    (57.46*DEG2RAD),
    (56.3183*DEG2RAD), (-56.3623195*DEG2RAD)
};

#endif
