// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2009 David Roberts <dvdr18@gmail.com>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>

#include "PlanetFactory.h"
#include "MarbleColors.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "Planet.h"

namespace Marble
{

QList<QString> PlanetFactory::planetList()
{
    QStringList planets;

    planets << QStringLiteral("mercury") << QStringLiteral("venus") << QStringLiteral("earth") << QStringLiteral("mars") << QStringLiteral("jupiter")
            << QStringLiteral("saturn") << QStringLiteral("uranus") << QStringLiteral("neptune") << QStringLiteral("pluto") << QStringLiteral("sun")
            << QStringLiteral("moon") << QStringLiteral("sky");

    return planets;
}

Planet PlanetFactory::construct(const QString &id)
{
    Planet planet;
    planet.setId(id);

    // constants taken from https://aa.quae.nl/en/reken/zonpositie.html

    if (id == QLatin1StringView("mercury")) {
        planet.setM_0(174.7948 * DEG2RAD);
        planet.setM_1(4.09233445 * DEG2RAD);
        planet.setC_1(23.4400 * DEG2RAD);
        planet.setC_2(2.9818 * DEG2RAD);
        planet.setC_3(0.5255 * DEG2RAD);
        planet.setC_4(0.1058 * DEG2RAD);
        planet.setC_5(0.0241 * DEG2RAD);
        planet.setC_6(0.0055 * DEG2RAD);
        planet.setPi(111.5943 * DEG2RAD);
        planet.setEpsilon(0.02 * DEG2RAD);
        planet.setTheta_0(13.5964 * DEG2RAD);
        planet.setTheta_1(6.1385025 * DEG2RAD);
        planet.setRadius(2440000.0);
        planet.setName(QStringLiteral("Mercury"));
        planet.setHasAtmosphere(false);
    } else if (id == QLatin1StringView("venus")) {
        planet.setM_0(50.4161 * DEG2RAD);
        planet.setM_1(1.60213034 * DEG2RAD);
        planet.setC_1(0.7758 * DEG2RAD);
        planet.setC_2(0.0033 * DEG2RAD);
        planet.setC_3(0);
        planet.setC_4(0);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(73.9519 * DEG2RAD);
        planet.setEpsilon(2.64 * DEG2RAD);
        planet.setTheta_0(215.2995 * DEG2RAD);
        planet.setTheta_1(-1.4813688 * DEG2RAD);
        planet.setRadius(6051800.0);
        planet.setTwilightZone(18 * DEG2RAD);
        planet.setName(QStringLiteral("Venus"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::sunYellow4);
    } else if (id == QLatin1StringView("earth")) {
        planet.setM_0(357.5291 * DEG2RAD);
        planet.setM_1(0.98560028 * DEG2RAD);
        planet.setC_1(1.9148 * DEG2RAD);
        planet.setC_2(0.0200 * DEG2RAD);
        planet.setC_3(0.0003 * DEG2RAD);
        planet.setC_4(0);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(102.9372 * DEG2RAD);
        planet.setEpsilon(23.43686 * DEG2RAD);
        planet.setTheta_0(280.1600 * DEG2RAD);
        planet.setTheta_1(360.9856235 * DEG2RAD);
        planet.setRadius(6378137.0);
        planet.setTwilightZone(18 * DEG2RAD);
        planet.setName(QStringLiteral("Earth"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Qt::white);
    } else if (id == QLatin1StringView("mars")) {
        planet.setM_0(19.3730 * DEG2RAD);
        planet.setM_1(0.52402068 * DEG2RAD);
        planet.setC_1(10.6912 * DEG2RAD);
        planet.setC_2(0.6228 * DEG2RAD);
        planet.setC_3(0.0503 * DEG2RAD);
        planet.setC_4(0.0046 * DEG2RAD);
        planet.setC_5(0.0005 * DEG2RAD);
        planet.setC_6(0);
        planet.setPi(70.9812 * DEG2RAD);
        planet.setEpsilon(25.19 * DEG2RAD);
        planet.setTheta_0(313.4803 * DEG2RAD);
        planet.setTheta_1(350.89198226 * DEG2RAD);
        planet.setRadius(3397000.0);
        planet.setTwilightZone(9.0 * DEG2RAD);
        planet.setName(QStringLiteral("Mars"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::hotOrange2);
    } else if (id == QLatin1StringView("jupiter")) {
        planet.setM_0(20.0202 * DEG2RAD);
        planet.setM_1(0.08308529 * DEG2RAD);
        planet.setC_1(5.5549 * DEG2RAD);
        planet.setC_2(0.1683 * DEG2RAD);
        planet.setC_3(0.0071 * DEG2RAD);
        planet.setC_4(0.0003 * DEG2RAD);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(237.2074 * DEG2RAD);
        planet.setEpsilon(3.12 * DEG2RAD);
        planet.setTheta_0(146.0727 * DEG2RAD);
        planet.setTheta_1(870.5366420 * DEG2RAD);
        planet.setRadius(71492000.0);
        planet.setName(QStringLiteral("Jupiter"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::sunYellow2);
    } else if (id == QLatin1StringView("saturn")) {
        planet.setM_0(317.0207 * DEG2RAD);
        planet.setM_1(0.03344414 * DEG2RAD);
        planet.setC_1(6.3585 * DEG2RAD);
        planet.setC_2(0.2204 * DEG2RAD);
        planet.setC_3(0.0106 * DEG2RAD);
        planet.setC_4(0.0006 * DEG2RAD);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(99.4571 * DEG2RAD);
        planet.setEpsilon(26.74 * DEG2RAD);
        planet.setTheta_0(174.3479 * DEG2RAD);
        planet.setTheta_1(810.7939024 * DEG2RAD);
        planet.setRadius(60268000.0);
        planet.setName(QStringLiteral("Saturn"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::sunYellow2);
    } else if (id == QLatin1StringView("uranus")) {
        planet.setM_0(141.0498 * DEG2RAD);
        planet.setM_1(0.01172834 * DEG2RAD);
        planet.setC_1(5.3042 * DEG2RAD);
        planet.setC_2(0.1534 * DEG2RAD);
        planet.setC_3(0.0062 * DEG2RAD);
        planet.setC_4(0.0003 * DEG2RAD);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(5.4639 * DEG2RAD);
        planet.setEpsilon(82.22 * DEG2RAD);
        planet.setTheta_0(17.9705 * DEG2RAD);
        planet.setTheta_1(-501.1600928 * DEG2RAD);
        planet.setRadius(25559000.0);
        planet.setName(QStringLiteral("Uranus"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::seaBlue4);
    } else if (id == QLatin1StringView("neptune")) {
        planet.setM_0(256.2250 * DEG2RAD);
        planet.setM_1(0.00598103 * DEG2RAD);
        planet.setC_1(1.0302 * DEG2RAD);
        planet.setC_2(0.0058 * DEG2RAD);
        planet.setC_3(0);
        planet.setC_4(0);
        planet.setC_5(0);
        planet.setC_6(0);
        planet.setPi(182.1957 * DEG2RAD);
        planet.setEpsilon(27.84 * DEG2RAD);
        planet.setTheta_0(52.3996 * DEG2RAD);
        planet.setTheta_1(536.3128492 * DEG2RAD);
        planet.setRadius(24766000.0);
        planet.setName(QStringLiteral("Neptune"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Oxygen::skyBlue2);
    } else if (id == QLatin1StringView("pluto")) {
        // dwarf planets ... (everybody likes pluto)
        planet.setM_0(14.882 * DEG2RAD);
        planet.setM_1(0.00396 * DEG2RAD);
        planet.setC_1(28.3150 * DEG2RAD);
        planet.setC_2(4.3408 * DEG2RAD);
        planet.setC_3(0.9214 * DEG2RAD);
        planet.setC_4(0.2235 * DEG2RAD);
        planet.setC_5(0.0627 * DEG2RAD);
        planet.setC_6(0.0174 * DEG2RAD);
        planet.setPi(4.5433 * DEG2RAD);
        planet.setEpsilon(57.46 * DEG2RAD);
        planet.setTheta_0(56.3183 * DEG2RAD);
        planet.setTheta_1(-56.3623195 * DEG2RAD);
        planet.setRadius(1151000.0);
        planet.setName(QStringLiteral("Pluto"));
        planet.setHasAtmosphere(false);
    } else if (id == QLatin1StringView("sun")) {
        mDebug() << "WARNING: Creating Planet instance" << id << "with invalid orbital elements";
        planet.setRadius(695000000.0);
        // FIXME: fill in with valid data
        planet.setName(QStringLiteral("Sun"));
        planet.setHasAtmosphere(true);
        planet.setAtmosphereColor(Qt::white);
    } else if (id == QLatin1StringView("moon")) {
        mDebug() << "WARNING: Creating Planet instance" << id << "with invalid orbital elements";
        planet.setRadius(1738000.0);
        // FIXME: fill in with valid data
        planet.setName(QStringLiteral("Moon"));
        planet.setHasAtmosphere(false);
    } else if (id == QLatin1StringView("sky")) {
        mDebug() << "WARNING: Creating Planet instance" << id << "with invalid orbital elements";
        planet.setRadius(10000000.0);
        planet.setName(QStringLiteral("Sky"));
        planet.setHasAtmosphere(false);
    } else {
        mDebug() << "WARNING: Creating Planet instance" << id << "with invalid orbital elements";
        planet.setRadius(10000000.0);
        planet.setName(QStringLiteral("Unknown"));
        planet.setHasAtmosphere(false);
    }

    return planet;
}

QString PlanetFactory::localizedName(const QString &id)
{
    if (id == QLatin1StringView("mercury")) {
        return QObject::tr("Mercury", "the planet");
    } else if (id == QLatin1StringView("venus")) {
        return QObject::tr("Venus", "the planet");
    } else if (id == QLatin1StringView("earth")) {
        return QObject::tr("Earth", "the planet");
    } else if (id == QLatin1StringView("mars")) {
        return QObject::tr("Mars", "the planet");
    } else if (id == QLatin1StringView("jupiter")) {
        return QObject::tr("Jupiter", "the planet");
    } else if (id == QLatin1StringView("saturn")) {
        return QObject::tr("Saturn", "the planet");
    } else if (id == QLatin1StringView("uranus")) {
        return QObject::tr("Uranus", "the planet");
    } else if (id == QLatin1StringView("neptune")) {
        return QObject::tr("Neptune", "the planet");
        // dwarf planets ... (everybody likes pluto)
    } else if (id == QLatin1StringView("pluto")) {
        return QObject::tr("Pluto", "the planet");
        // sun, moon and sky
    } else if (id == QLatin1StringView("sun")) {
        return QObject::tr("Sun", "the earth's star");
    } else if (id == QLatin1StringView("moon")) {
        return QObject::tr("Moon", "the earth's moon");
    } else if (id == QLatin1StringView("sky")) {
        return QObject::tr("Sky");
    } else if (id.isEmpty()) {
        mDebug() << "Warning: empty id";
        return QObject::tr("Unknown Planet", "a planet without data");
    }

    return id;
}

}
