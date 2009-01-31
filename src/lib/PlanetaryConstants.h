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

#define pc_deg2rad(x) x*M_PI/180.0

struct PlanetaryConstants {
	qreal M_0, M_1; // for calculating mean anomaly
	qreal C_1, C_2, C_3, C_4, C_5, C_6; // for calculating equation of center
	qreal Pi; // ecliptic longitude of the perihelion
	qreal epsilon; // obliquity of the ecliptic plane
	qreal theta_0, theta_1; // for calculating sidereal time
};

// constants taken from http://www.astro.uu.nl/~strous/AA/en/reken/zonpositie.html
PlanetaryConstants PC_MERCURY = {
	pc_deg2rad(174.7948), pc_deg2rad(4.09233445),
	pc_deg2rad(23.4400), pc_deg2rad(2.9818), pc_deg2rad(0.5255), pc_deg2rad(0.1058), pc_deg2rad(0.0241), pc_deg2rad(0.0055),
	pc_deg2rad(111.5943),
	pc_deg2rad(0.02),
	pc_deg2rad(13.5964), pc_deg2rad(6.1385025)
};
PlanetaryConstants PC_VENUS = {
	pc_deg2rad(50.4161), pc_deg2rad(1.60213034),
	pc_deg2rad(0.7758), pc_deg2rad(0.0033), pc_deg2rad(0.0000), pc_deg2rad(0.0000), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(73.9519),
	pc_deg2rad(2.64),
	pc_deg2rad(215.2995), pc_deg2rad(-1.4813688)
};
PlanetaryConstants PC_EARTH = {
	pc_deg2rad(357.5291), pc_deg2rad(0.98560028),
	pc_deg2rad(1.9148), pc_deg2rad(0.0200), pc_deg2rad(0.0003), pc_deg2rad(0.0000), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(102.9372),
	pc_deg2rad(23.45),
	pc_deg2rad(280.1600), pc_deg2rad(360.9856235),
};
PlanetaryConstants PC_MARS = {
	pc_deg2rad(19.3730), pc_deg2rad(0.52402068),
	pc_deg2rad(10.6912), pc_deg2rad(0.6228), pc_deg2rad(0.0503), pc_deg2rad(0.0046), pc_deg2rad(0.0005), pc_deg2rad(0.0000),
	pc_deg2rad(70.9812),
	pc_deg2rad(25.19),
	pc_deg2rad(313.4803), pc_deg2rad(350.89198226)
};
PlanetaryConstants PC_JUPITER = {
	pc_deg2rad(20.0202), pc_deg2rad(0.08308529),
	pc_deg2rad(5.5549), pc_deg2rad(0.1683), pc_deg2rad(0.0071), pc_deg2rad(0.0003), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(237.2074),
	pc_deg2rad(3.12),
	pc_deg2rad(146.0727), pc_deg2rad(870.5366420)
};
PlanetaryConstants PC_SATURN = {
	pc_deg2rad(317.0207), pc_deg2rad(0.03344414),
	pc_deg2rad(6.3585), pc_deg2rad(0.2204), pc_deg2rad(0.0106), pc_deg2rad(0.0006), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(99.4571),
	pc_deg2rad(26.74),
	pc_deg2rad(174.3479), pc_deg2rad(810.7939024)
};
PlanetaryConstants PC_URANUS = {
	pc_deg2rad(141.0498), pc_deg2rad(0.01172834),
	pc_deg2rad(5.3042), pc_deg2rad(0.1534), pc_deg2rad(0.0062), pc_deg2rad(0.0003), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(5.4639),
	pc_deg2rad(82.22),
	pc_deg2rad(17.9705), pc_deg2rad(-501.1600928)
};
PlanetaryConstants PC_NEPTUNE = {
	pc_deg2rad(256.2250), pc_deg2rad(0.00598103),
	pc_deg2rad(1.0302), pc_deg2rad(0.0058), pc_deg2rad(0.0000), pc_deg2rad(0.0000), pc_deg2rad(0.0000), pc_deg2rad(0.0000),
	pc_deg2rad(182.1957),
	pc_deg2rad(27.84),
	pc_deg2rad(52.3996), pc_deg2rad(536.3128492)
};
PlanetaryConstants PC_PLUTO = {
	pc_deg2rad(14.882), pc_deg2rad(0.00396),
	pc_deg2rad(28.3150), pc_deg2rad(4.3408), pc_deg2rad(0.9214), pc_deg2rad(0.2235), pc_deg2rad(0.0627), pc_deg2rad(0.0174),
	pc_deg2rad(4.5433),
	pc_deg2rad(57.46),
	pc_deg2rad(56.3183), pc_deg2rad(-56.3623195)
};

#endif
