//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Gerhard Holtkamp
//

/***************************************************************************
* Calculate positions and physical ephemerides of Solar System bodies.     * *                                                                          *
* The algorithm for the Modified Julian Date are based on                  *
* O.Montebruck and T.Pfleger, "Astronomy with a PC",                       *
* Springer Verlag, Berlin, Heidelberg, New York, 1989                      *
* 
* The calculations of the positions and physical ephemerides of the        *
* various solar system bodies are based on                                 *
* O.Montebruck, "Astronomie mit dem Personal Computer",                    *
* Springer Verlag, Berlin, Heidelberg, 1989;                               *
* O.Montebruck, "Practical Ephemeris Calculations",                        *
* Springer Verlag, Berlin, Heidelberg, New York, 1989                      *
* and on the                                                               *
* "Explanatory Supplement to the Astronomical Almanac"                     *
* University Science Books, Mill Valley, CA, 1992                          *
*                                                                          *
* Open Source Code. License: GNU LGPL Version 2+                           *
*                                                                          *
* Author: Gerhard HOLTKAMP,        30-SEP-2013                             *
***************************************************************************/

/*------------ include files and definitions -----------------------------*/
#include "solarsystem.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
using namespace std;

#include "astrolib.h"
#include "astr2lib.h"
#include "attlib.h"

const double degrad = M_PI / 180.0;

// ################ Solar Eclipse Class ####################

SolarSystem::SolarSystem()
{
 ssinit();
}

SolarSystem::~SolarSystem()
{

}

double SolarSystem::atan23 (double y, double x)
 {
  /* redefine atan2 so that it does'nt crash when both x and y are 0 */
  double result;

  if ((x == 0) && (y == 0)) result = 0;
  else result = atan2 (y, x);

  return result;
 }

double SolarSystem::DegFDms (double h)
 {
  /* convert degrees from d.fff -> d.mmsss where .mm are the minutes
     and sss are seconds (and fractions of seconds).
  */
  int mm, deg;
  double hh, t, sec;

  hh = fabs(h);
  dms (hh,deg,mm,sec);
  if (sec>=59.5)
   {
    mm++;
    sec = 0;
   };
  if (mm>59)
   {
    deg++;
    mm=0;
   };
  hh = double(deg);
  t = double(mm)/100.0;
  hh += t;
  t = sec/10000.0;
  hh += t;
  if (h < 0) hh = -hh;

  return hh;
 }

double SolarSystem::DmsDegF (double h)
 {
  /* convert degrees from d.mmsss -> d.fff where .mm are the minutes
     and sss are seconds (and fractions of seconds).
     Returned is the angle in decimal degrees.
  */
  int mm, deg;
  double hh, t, sec, dlt;

  hh = fabs(h);
  dlt = hh*3.33e-16;
  hh += dlt;
  deg = int(hh);
  t = fmod(hh,1.0)*100.0;
  mm = int(t + 0.1e-12);
  sec = fmod(hh*100.0,1.0)*100.0;
  hh = ddd(deg,mm,sec);
  if (h < 0) hh = -hh;

  return hh;
 }

void SolarSystem::ssinit()
{
 // initialize solar system data
  ss_update_called = false;
  ss_moon_called = false;
  ss_nutation = false;
  ss_planmat_called = false;
  ss_kepler_stored = false;
  ss_kepler_called = false;

  ss_day = 1;
  ss_month = 1;
  ss_year = 2012;
  ss_hour = 0;
  ss_minute = 0;
  ss_second = 0;
  ss_tzone = 0;
  ss_del_auto = 1;
  ss_del_tdut = DefTdUt(ss_year);
  ss_RT = true;
  ss_epoch = 51544.5;  // J2000.0
  ss_central_body = 4;  
  setCurrentMJD();
  ss_planmat = mxidn();
  getConstEarth();
}

void SolarSystem::DefTime ()  // Get System Time and Date
 {
  time_t tt;
  int hh, mm, ss;
  double jd, hr;

  tt = time(NULL);
  jd = 40587.0 + tt/86400.0; // seconds since 1-JAN-1970

  caldat(jd, hh, mm, ss, hr);
  ss_year = ss;
  ss_month = mm;
  ss_day = hh;

  dms(hr, hh, mm, jd);
  ss_hour = hh;
  ss_minute = mm;
  ss_second = int(jd);
  if (ss_del_auto) ss_del_tdut = DefTdUt(ss_year);
  };

void SolarSystem::setTimezone(double d)
{
  // set the timezone to d (hours difference from UTC) for I/O
  ss_tzone = d;
}

void SolarSystem::setDeltaTAI_UTC(double d)
{
  // c is the difference between TAI and UTC according to the IERS
  // we have to add 32.184 sec to get to the difference TT - UT
  // which is used in the calculations here

  ss_del_tdut = d + 32.184;
  ss_del_auto = 0;
}

void SolarSystem::setAutoTAI_UTC()
{
  // set the difference between TAI and UTC according to the IERS
  ss_del_auto = true;
  ss_del_tdut = DefTdUt(ss_year);
}

void SolarSystem::setCurrentMJD(int year, int month, int day, int hour, int min, double sec)
{
    // set the (MJD-) time currently used for calculations to year, month, day, hour, min, sec
    // corrected for timezone

    double jd;

    jd = ddd(hour, min, sec) - ss_tzone;
    jd = mjd(day, month, year, jd);

    ss_time = jd;
    ss_update_called = false;
    ss_moon_called = false;
    ss_planmat_called = false;
    ss_kepler_called = false;

}

void SolarSystem::setCurrentMJD()
{
  // set the (MJD-) time currently used for calculations to Real Time

    double jd, sec;

    DefTime();
    sec = double(ss_second);
    jd = ddd(ss_hour, ss_minute, sec);
    jd = mjd(ss_day, ss_month, ss_year, jd);

    ss_time = jd;
    ss_update_called = false;
    ss_moon_called = false;
    ss_planmat_called = false;
    ss_kepler_called = false;

}

double SolarSystem::getMJD(int year, int month, int day, int hour, int min, double sec) const
{
    // return the (MJD-) time corresponding to year, month, day, hour, min, sec
    // corrected for timezone

    double jd;

    jd = ddd(hour, min, sec) - ss_tzone;
    jd = mjd(day, month, year, jd);

    return jd;
}

void SolarSystem::getDatefromMJD(double mjd, int &year, int &month, int &day, int &hour, int &min, double &sec) const
{
    // convert times given in Modified Julian Date (MJD) into conventional date and time
    // correct for timezone

    double magn;

    caldat((mjd + ss_tzone/24.0), day, month, year, magn);
    dms (magn,hour,min,sec);
    if (sec>30.0) min++;;
    if (min>59)
     {
      hour++;
      min=0;
     };
}

void SolarSystem::setEpoch (double yr)
{
 int day, month, year;
 double b;

 if (yr == 0) ss_epoch = 0;  // Mean of Date
 else
  {
   year = int(yr);
   b = 12.0 * (yr - double(year));
   month = int(b) + 1;
   day = 1;

   b = 0;
   ss_epoch = mjd(day, month, year, b); 
  }
  ss_update_called = false;
  ss_moon_called = false;
  ss_planmat_called = false;
  ss_kepler_called = false;

}

void SolarSystem::setNutation (bool nut)
{
  ss_nutation = nut;
  ss_update_called = false;
  ss_moon_called = false;
  ss_planmat_called = false;
  ss_kepler_called = false;

}

void SolarSystem::setCentralBody(const char* pname)
{
  ss_central_body = 4;  // default Earth
  getConstEarth();
  if (strncmp("Sun", pname, 3) == 0)
   {
    ss_central_body = 0;
    getConstSun();
   };
  if (strncmp("Moon", pname, 4) == 0)
   {
    ss_central_body = 1;
    getConstMoon();
   };
  if (strncmp("Mercury", pname, 7) == 0)
   {
    ss_central_body = 2;
    getConstMercury();
   };
  if (strncmp("Venus", pname, 5) == 0)
   {
    ss_central_body = 3;
    getConstVenus();
   };
  if (strncmp("Mars", pname, 4) == 0)
   {
    ss_central_body = 5;
    getConstMars();
   };
  if (strncmp("Jupiter", pname, 7) == 0)
   {
    ss_central_body = 6;
    getConstJupiter();
   };
  if (strncmp("Saturn", pname, 6) == 0)
   {
    ss_central_body = 7;
    getConstSaturn();
   };
  if (strncmp("Uranus", pname, 6) == 0)
   {
    ss_central_body = 8;
    getConstUranus();
   };
  if (strncmp("Neptune", pname, 7) == 0)
   {
    ss_central_body = 9;
    getConstNeptune();
   };
  ss_update_called = false;
  ss_moon_called = false;
  ss_planmat_called = false;
  ss_kepler_called = false;

}

void SolarSystem::updateSolar()
{
  // calculate all positions in mean ecliptic of epoch

  const double ae = 23454.77992; // 149597870.0/6378.14 =  1AE -> Earth Radii
  double dt, eps2;
  Sun200 sun;
  Moon200 moon;
  Plan200 pln;
  Vec3 coff;
  Mat3 pmx;

  ss_update_called = true;

  // get positions in ecliptic coordinates of date
  dt = ss_time + ss_del_tdut/86400.0;
  dt = julcent (dt);
  ss_rs = sun.position(dt);
  ss_rm = moon.position(dt)/ae;
  ss_pmer = pln.Mercury(dt);
  ss_pven = pln.Venus(dt);
  ss_pearth[0] = -ss_rs[0];
  ss_pearth[1] = -ss_rs[1];
  ss_pearth[2] = -ss_rs[2];
  ss_pmars = pln.Mars(dt);
  ss_pjup = pln.Jupiter(dt);
  ss_psat = pln.Saturn(dt);
  ss_pura = pln.Uranus(dt);
  ss_pnept = pln.Neptune(dt);

  // refer to selected central body
  coff[0] = 0;
  coff[1] = 0;
  coff[2] = 0;

  if (ss_central_body == 2) coff -= ss_pmer; 
  if (ss_central_body == 3) coff -= ss_pven; 
  if (ss_central_body == 4) coff -= ss_pearth;
  if (ss_central_body == 5) coff -= ss_pmars; 
  if (ss_central_body == 6) coff -= ss_pjup; 
  if (ss_central_body == 7) coff -= ss_psat; 
  if (ss_central_body == 8) coff -= ss_pura; 
  if (ss_central_body == 9) coff -= ss_pnept; 
  if (ss_central_body == 1) coff = coff + ss_rs - ss_rm; 

  ss_pmer += coff;
  ss_pven += coff;
  ss_pearth += coff;
  ss_pmars += coff;
  ss_pjup += coff;
  ss_psat += coff;
  ss_pura += coff;
  ss_pnept += coff;

  ss_rs[0] = coff[0];
  ss_rs[1] = coff[1];
  ss_rs[2] = coff[2];

  // convert into equatorial
  ss_rs = eclequ(dt, ss_rs);
  ss_rm = eclequ(dt, ss_rm);
  ss_pmer = eclequ(dt, ss_pmer);
  ss_pven = eclequ(dt, ss_pven);
  ss_pearth = eclequ(dt, ss_pearth);
  ss_pmars = eclequ(dt, ss_pmars);
  ss_pjup = eclequ(dt, ss_pjup);
  ss_psat = eclequ(dt, ss_psat);
  ss_pura = eclequ(dt, ss_pura);
  ss_pnept = eclequ(dt, ss_pnept);

  // correct for precession
  if (ss_epoch != 0)
   {
    eps2 = julcent(ss_epoch);
    pmx = pmatequ(dt, eps2);
    ss_rs = mxvct(pmx,ss_rs);
    ss_rm = mxvct(pmx,ss_rm);
    ss_pmer = mxvct(pmx,ss_pmer);
    ss_pven = mxvct(pmx,ss_pven);
    ss_pearth = mxvct(pmx,ss_pearth);
    ss_pmars = mxvct(pmx,ss_pmars);
    ss_pjup = mxvct(pmx,ss_pjup);
    ss_psat = mxvct(pmx,ss_psat);
    ss_pura = mxvct(pmx,ss_pura);
    ss_pnept = mxvct(pmx,ss_pnept);
   };

  // correct for nutation
  if (ss_nutation)
   {
    pmx = nutmat(dt, eps2, false);
    ss_rs = mxvct(pmx,ss_rs);
    ss_rm = mxvct(pmx,ss_rm);
    ss_pmer = mxvct(pmx,ss_pmer);
    ss_pven = mxvct(pmx,ss_pven);
    ss_pearth = mxvct(pmx,ss_pearth);
    ss_pmars = mxvct(pmx,ss_pmars);
    ss_pjup = mxvct(pmx,ss_pjup);
    ss_psat = mxvct(pmx,ss_psat);
    ss_pura = mxvct(pmx,ss_pura);
    ss_pnept = mxvct(pmx,ss_pnept);
   };
}

void SolarSystem::getRaDec(Vec3 r1, double& ra, double& decl)
{
  // convert equatorial vector r1 into RA and DEC (in HH.MMSS and DD.MMSS)

  double const degrad = M_PI / 180.0;
  Vec3 r2;

  r2 = carpol(r1);
  decl = r2[2] / degrad;
  ra = r2[1] / degrad;
  ra /= 15.0;
  if (ra < 0) ra += 24.0;
  
  decl = DegFDms(decl);
  ra = DegFDms(ra);

}

void SolarSystem::getSun (double& ra, double& decl)  // RA and Dec for the Sun
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 0)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_rs, ra, decl);
} 

void SolarSystem::getMoon (double& ra, double& decl)  // RA and Dec for the Moon
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body != 4)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_rm, ra, decl);
} 

void SolarSystem::getMercury (double& ra, double& decl)  // RA and Dec for Mercury
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 2)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pmer, ra, decl);
} 

void SolarSystem::getVenus (double& ra, double& decl)  // RA and Dec for Venus
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 3)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pven, ra, decl);
} 

void SolarSystem::getEarth (double& ra, double& decl)  // RA and Dec for Earth
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 4)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pearth, ra, decl);
} 

void SolarSystem::getMars (double& ra, double& decl)  // RA and Dec for Mars
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 5)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pmars, ra, decl);
} 

void SolarSystem::getJupiter (double& ra, double& decl)  // RA and Dec for Jupiter
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 6)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pjup, ra, decl);
} 

void SolarSystem::getSaturn (double& ra, double& decl)  // RA and Dec for Saturn
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 7)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_psat, ra, decl);
} 

void SolarSystem::getUranus (double& ra, double& decl)  // RA and Dec for Uranus
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 8)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pura, ra, decl);
} 

void SolarSystem::getNeptune (double& ra, double& decl)  // RA and Dec for Neptune
{
  if (!ss_update_called) updateSolar();
  if (ss_central_body == 9)
   {
    ra = -100.0;
    decl = 0;
   }
  else getRaDec (ss_pnept, ra, decl);

} 

void SolarSystem::getPhysSun (double &pdiam, double &pmag)
{
 // Apparent diameter for the Sun in radians

  if (ss_central_body == 0)
   {
    pdiam = 0;
    pmag = 0;

    return;
   }; 
  if (!ss_update_called) updateSolar();

  pdiam = 0.00930495 / abs(ss_rs);
  pmag = -26.7 + 5.0 * log10(abs(ss_rs));
} 
 
void SolarSystem::getPhysMercury(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Mercury 

 double ia, cp, cs, ps;

 if (ss_central_body == 2)
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pmer);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pmer);

 pdiam = 3.24831e-05 / cp; // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 ia /= 100.0;
 pmag = -0.36 + 3.80*ia - 2.73*ia*ia + 2.00*ia*ia*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysVenus(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Venus 

 double ia, cp, cs, ps;

 if (ss_central_body == 3) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pven);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pven);

 pdiam = 8.09089e-05 / cp; // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 ia /= 100.0;
 pmag = -4.29 + 0.09*ia + 2.39*ia*ia - 0.65*ia*ia*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysEarth(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Earth 

 double ia, cp, cs, ps;

 if (ss_central_body == 4) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pearth);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pearth);

 pdiam = 8.52705e-05 / cp; // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 ia /= 100.0;
 pmag = -4.0 + 0.09*ia + 2.39*ia*ia - 0.65*ia*ia*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysMars(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Mars 

 double ia, cp, cs, ps;

 if (ss_central_body == 5) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pmars);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pmars);

 pdiam = 4.54178e-05 / cp;  // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 pmag = -1.52 + 0.016*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysJupiter(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Jupiter 

 double ia, cp, cs, ps;

 if (ss_central_body == 6) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pjup);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pjup);

 pdiam = 0.000955789 / cp;  // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 pmag = -9.25 + 0.005*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysSaturn(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Saturn 

 double ia, cp, cs, ps;

 if (ss_central_body == 7) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_psat);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_psat);

 pdiam = 0.000805733 / cp;  // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

// ia = acos(ia) / degrad;
// pmag = -7.19 + 0.044*ia;
 pmag = -10.0;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysUranus(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Uranus 

 double ia, cp, cs, ps;

 if (ss_central_body == 8) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pura);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pura);

 pdiam = 0.000341703 / cp;  // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 ia = acos(ia) / degrad;
 pmag = -7.19 + 0.0228*ia;
 pmag = pmag + 5.0 * log10(ps * cp);
}

void SolarSystem::getPhysNeptune(double &pdiam, double &pmag, double &pphase)
{
 // Physical elements Neptune 

 double ia, cp, cs, ps;

 if (ss_central_body == 9) 
  {
   pdiam = 0;
   pmag = 0;
   pphase = 0;

   return;
  }; 

 if (!ss_update_called) updateSolar();

 cp = abs(ss_pnept);
 cs = abs(ss_rs);
 ps = abs(ss_rs - ss_pnept);

 pdiam = 0.000331074 / cp;  // apparent diameter in radians

 ia = 2.0 * cp * ps;
 if (ia == 0) ia = 1.0; // this should never happen

 ia = (cp*cp + ps*ps - cs*cs) / ia;  // cos of phase angle

 pphase = 0.5 * (1.0 + ia);

 pmag = -6.87;
 pmag = pmag + 5.0 * log10(ps * cp);
}

double SolarSystem::getDiamMoon ()
{
 // Apparent diameter for the Moon

  if (ss_central_body == 1) return 0; 
  if (!ss_update_called) updateSolar();

  return 2.32356e-05 / abs(ss_rm);
} 

void SolarSystem::getLunarLibration (double &lblon, double &lblat, double &termt)
{
 // librations of the Moon and terminator position
  if (!ss_moon_called) MoonDetails();

  lblon = ss_moon_lblon;
  lblat = ss_moon_lblat;
  termt = ss_moon_term;
}

void SolarSystem::getLunarPhase (double &phase, double &ildisk, double &amag)
{
 // phase and mag of Moon
  if (!ss_moon_called) MoonDetails();

  phase = ss_moon_phase;
  ildisk = ss_moon_ildisk;
  amag = ss_moon_mag;
}

Vec3 SolarSystem::SnPos (double &ep2, double &els) const
 {
  // return the apparent position of the Sun
  // and the Nutation ep2 value and the ecliptic longitude of the Sun els

  Sun200 sun;
  Mat3 mx;
  Vec3 rs, s;
  double t;

  t = ss_time + ss_del_tdut/86400.0;
  t = julcent (t);

  rs = sun.position(t);
  s = carpol(rs);
  els = s[1];

  rs = eclequ(t,rs);
  mx = nutmat(t,ep2);   // nutation
  rs = mxvct(mx,rs);    // apparent coordinates
  rs = aberrat(t,rs);

  return rs;
 }

Vec3 SolarSystem::MnPos (double &ep2, double &elm) const
 {
  // return the apparent position of the Moon
  // and the Nutation ep2 value and the ecliptic longitude of the Moon elm
	
  Moon200 moon;
  Mat3 mx;
  Vec3 rm, s;
  double t;

  t = ss_time + ss_del_tdut/86400.0;
  t = julcent (t);

  rm = moon.position(t);
  s = carpol(rm);
  elm = s[1];

  rm = eclequ(t,rm);
  mx = nutmat(t,ep2);   // nutation
  rm = mxvct(mx,rm);

  return rm;
 }

void SolarSystem::MoonLibr (double jd, Vec3 rm, Vec3 sn,
               double &lblon, double &lblat, double &termt)
 {
  /* Calculate the librations angles lblon (longitude) and
     lblat (latitude) for the moon at position rs and time jd.
     Also calculate the selenographic longitude of the terminator.
     rm is the position of the Moon from Earth, sn the position
     of the Sun (also with respect to Earth).
  */
  double t, gam, gmp, l, omg, mln;
  double a, b, c, ic, gn, gp, omp;
  double const degrad = M_PI / 180.0;
  Vec3 v1;
  Mat3 m1, m2;

  t = (jd - 15019.5) / 36525.0;
  gam = 281.2208333 + ((0.33333333e-5*t + 0.45277778e-3)*t + 1.7191750)*t;
  gmp = 334.3295556 + ((-0.125e-4*t - 0.10325e-1)*t + 4069.0340333)*t;
  l = 279.6966778 + (0.3025e-3*t + 36000.768925)*t;
  omg = 259.1832750 + ((0.22222222e-5*t + 0.20777778e-2)*t - 1934.1420083)*t;
  b = 23.45229444 + ((0.50277778e-6*t -0.16388889e-5)*t - 0.130125e-1)*t;
  mln = 270.4341639 + ((0.1888889e-5*t -0.11333e-2)*t + 481267.8831417)*t;
  ic = 1.535*degrad;
  gn = (l - gam)*degrad;
  gp = (mln - gmp)*degrad;
  omp = (gmp - omg)*degrad;
  a = -107.0*cos(gp) + 37.0*cos(gp+2.0*omp) -11.0*cos(2.0*(gp+omp));
  a = a*0.000277778*degrad + ic;
  c = (-109.0*sin(gp) + 37.0*sin(gp+2.0*omp) - 11.0*sin(2.0*(gp+omp)))/sin(ic);
  c = (c*0.000277778 + omg)*degrad;
  gn = -12.0*sin(gp) + 59.0*sin(gn) + 18.0*sin(2.0*omp);
  gn = gn*0.000277778*degrad;  // tau

  b *= degrad;
  gam = cos(a)*cos(b) + sin(a)*sin(b)*cos(c);
  gmp = gam*gam;
  if(gmp > 1.0) gmp = 0;
  else gmp = sqrt(1.0 - gmp);
  gam = atan23(gmp,gam);  // theta
  t = cos(a)*sin(b) - sin(a)*sin(b)*cos(c);
  l = -sin(a)*sin(c);

  gmp = atan23(l,t);  // phi
  t = sin(a)*cos(b) - cos(a)*sin(b)*cos(c);
  l = -sin(b)*sin(c);
  a = atan23(l,t);  // delta
  c = a + mln*degrad + gn - c;   // psi

  // libration rotation matrix from Mean equator to true selenographic
  m1 = zrot(gmp);
  m2 = xrot(gam);
  m1 = m2 * m1;
  m2 = zrot(c);
  m1 = m2 * m1;

  // Earth coordinates
  v1[0] = -rm[0];
  v1[1] = -rm[1];
  v1[2] = -rm[2];

  v1 = mxvct(m1,v1);
  v1 = carpol(v1);
  lblat = v1[2] / degrad;
  lblon = v1[1] / degrad;
  if (lblon > 180.0) lblon = lblon - 360.0;

  // terminator
  v1 = mxvct(m1,sn);
  v1 = carpol(v1);
  termt = v1[1] / degrad;
  if (termt > 180.0) termt = termt - 360.0;
  termt -= 90.0;
  a = 90.0 + lblon;
  b = lblon - 90;
  if (termt > a) termt -= 180.0;
  else
   {
    if (termt < b) termt += 180;
   };
 }

void SolarSystem::MoonDetails ()
 {
  // Calculate specific details about the Moon
  // Libration, Terminator position, Phase and Magnitude

  double jd, t, lblon, lblat, termt, mnmag;
  double dist, ps;
  double els, elm;
  double const degrad = M_PI / 180.0;
  double const ae = 23454.77992; // 149597870.0/6378.14 =  1AE -> Earth Radii
  Vec3 snc, mnc;   // position of the Sun and the Moon
  Vec3 s, rm, rs, s3;
  double ep2;    // correction for Apparent Sideral Time

  if (ss_central_body != 4)   // calculate only for the Earth as reference
   {
    ss_moon_ildisk = 0;
    ss_moon_phase = 0;
    ss_moon_lblon = 0;
    ss_moon_lblat = 0;
    ss_moon_mag = 0;
    ss_moon_term = 0; 

    return;
   };

  if (!ss_update_called) updateSolar();
  ss_moon_called = true;

  jd = ss_time;

  rs = SnPos (ep2, els);
  rs *= ae;
  snc = rs;

  rm = MnPos (ep2, elm);
  mnc = rm;
  s = snc - rm;
  MoonLibr(jd, rm, s, lblon, lblat, termt);  // librations and terminator

  // calculate apparent magnitude of the Moon
  mnmag = abs(rm) / 23454.77992;  // distance moon-observer in AU
  s = vnorm(s);
  s[0] = -s[0];
  s[1] = -s[1];
  s[2] = -s[2];
  s3 = vnorm(rm);

  dist = dot(s, s3);  // cos of phase angle Sun-Moon-Observer
  if (fabs(dist) <= 1.0) dist = acos(dist) / degrad;
  else dist = 180.0;
  if (dist <= 61.0) dist = -3.475256769e-2 + 2.75256769e-2*dist;
  else
   {
    if (dist < 115.0) dist = 0.6962632 * exp(1.48709985e-2*dist);
    else
     {
      if (dist < 155.0) dist = 0.6531068 * exp(1.49213e-2*dist);
      else dist = 1.00779e-9 * pow (dist, 4.486359);
     };
   };
  if (mnmag <= 0) mnmag = 1e-30;  // should never happen.
  mnmag = 0.23 + 5.0*log10(mnmag) + dist;  // moon's mag

  // illuminated fraction
  rs = snc - mnc;
  rs = vnorm(rs);
  rm = vnorm(mnc);
  t = (1.0 - dot(rs,rm)) * 0.5;
  ps = elm - els;
  if (ps < 0) ps += 2*M_PI;
  ps = ps / (2.0*M_PI);

  ss_moon_ildisk = t;
  ss_moon_phase = ps;
  ss_moon_lblon = lblon;
  ss_moon_lblat = lblat;
  ss_moon_mag = mnmag;
  ss_moon_term = termt; 
 }

void SolarSystem::getConstSun()  // Sun constants
{
  ss_J2 = 0;
  ss_R0 = 696000.0;
  ss_flat = 0.0;
  ss_axl0 = 286.13;
  ss_axl1 = 0.0;
  ss_axb0 = 63.87;
  ss_axb1 = 0.0;	
  ss_W = 84.10;
  ss_Wd = 14.1844000;
  ss_GM = 1.32712438e+20;  
}	

void SolarSystem::getConstMoon()  // Moon planetary constants
{
  ss_J2 = 0.2027e-3;
  ss_R0 = 1738.0;
  ss_flat = 0.0;
  ss_axl0 = 0.0;
  ss_axl1 = 0.0;
  ss_axb0 = 90.0;
  ss_axb1 = 0.0;	
  ss_W = 0.0;
  ss_Wd = 13.17635898;
  ss_GM = 4.90279412e+12;  
}	

void SolarSystem::getConstMercury()  // Mercury planetary constants
{ 
  ss_J2 = 0.0;
  ss_R0 = 2439.7;
  ss_flat = 0.0;
  ss_axl0 = 281.01;
  ss_axl1 = -0.033;
  ss_axb0 = 61.45;
  ss_axb1 = -0.005;	
  ss_W = 329.71;
  ss_Wd = 6.1385025;
  ss_GM = 2.20320802e+13;  
}	

void SolarSystem::getConstVenus()  // Venus planetary constants
{ 
  ss_J2 = 0.027e-3;
  ss_R0 = 6051.9;
  ss_flat = 0.0;
  ss_axl0 = 272.72;
  ss_axl1 = 0.0;
  ss_axb0 = 67.15;
  ss_axb1 = 0.0;	
  ss_W = 160.26;
  ss_Wd = -1.4813596;
  ss_GM = 3.24858761e+14;  
}	

void SolarSystem::getConstEarth()  // Earth planetary constants
{ 
  ss_J2 = 1.08263e-3;
  ss_R0 = 6378.140;
  ss_flat = 0.00335364;
  ss_axl0 = 0.0;
  ss_axl1 = 0.0;
  ss_axb0 = 90.0;
  ss_axb1 = 0.0;	
  ss_W = 0;
  ss_Wd = 359.017045833334;
  ss_GM = 3.986005e+14;  
}	

void SolarSystem::getConstMars()  // Mars planetary constants
{
  ss_J2 = 1.964e-3;
  ss_R0 = 3397.2;
  ss_flat = 0.00647630;
  ss_axl0 = 317.681;
  ss_axl1 = -0.108;
  ss_axb0 = 52.886;
  ss_axb1 = -0.061;	
  ss_W = 176.868; // 176.655;
  ss_Wd = 350.8919830;
  ss_GM = 4.282828596416e+13; // 4.282837405582e+13
}	

void SolarSystem::getConstJupiter()  // Jupiter planetary constants
{
  ss_J2 = 0.01475;
  ss_R0 = 71492.0;
  ss_flat = 0.06487;
  ss_axl0 = 268.05;
  ss_axl1 = -0.009;
  ss_axb0 = 64.49;
  ss_axb1 = 0.003;	
  ss_W = 43.3;
  ss_Wd = 870.270;
  ss_GM = 1.2671199164e+17;
}	

void SolarSystem::getConstSaturn()  // Saturn planetary constants
{
  ss_J2 = 0.01645;
  ss_R0 = 60268.0;
  ss_flat = 0.09796;
  ss_axl0 = 40.58;
  ss_axl1 = -0.036;
  ss_axb0 = 83.54;
  ss_axb1 = -0.004;	
  ss_W = 38.90;
  ss_Wd = 810.7939024;
  ss_GM = 3.7934096899e+16;
}	

void SolarSystem::getConstUranus()  // Uranus planetary constants
{
  ss_J2 = 0.012;
  ss_R0 = 25559.0;
  ss_flat = 0.02293;
  ss_axl0 = 257.43;
  ss_axl1 = 0;
  ss_axb0 = -15.10;
  ss_axb1 = 0;	
  ss_W = 261.62;
  ss_Wd = -554.913;
  ss_GM = 5.8031587739e+15;
}	

void SolarSystem::getConstNeptune()  // Neptune planetary constants
{
  ss_J2 = 0.004;
  ss_R0 = 24764.0;
  ss_flat = 0.0171;
  ss_axl0 = 295.33;
  ss_axl1 = 0;
  ss_axb0 = 40.65;
  ss_axb1 = 0;	
  ss_W = 107.21;
  ss_Wd = 468.75;
  ss_GM = 6.8713077560e+15;
}	

Mat3 SolarSystem::getSelenographic () const
{
  // Calculate the Matrix to transform from Mean of J2000 into selenographic
  // coordinates at MJD time ss_time.
  
  double t, gam, gmp, l, omg, mln;
  double a, b, c, ic, gn, gp, omp;
  double const degrad = M_PI / 180.0;
  Mat3 m1, m2;

  t = (ss_time - 15019.5) / 36525.0;
  gam = 281.2208333 + ((0.33333333e-5*t + 0.45277778e-3)*t + 1.7191750)*t;
  gmp = 334.3295556 + ((-0.125e-4*t - 0.10325e-1)*t + 4069.0340333)*t;
  l = 279.6966778 + (0.3025e-3*t + 36000.768925)*t;
  omg = 259.1832750 + ((0.22222222e-5*t + 0.20777778e-2)*t - 1934.1420083)*t;
  b = 23.45229444 + ((0.50277778e-6*t -0.16388889e-5)*t - 0.130125e-1)*t;
  mln = 270.4341639 + ((0.1888889e-5*t -0.11333e-2)*t + 481267.8831417)*t;
  ic = 1.535*degrad;
  gn = (l - gam)*degrad;
  gp = (mln - gmp)*degrad;
  omp = (gmp - omg)*degrad;
  a = -107.0*cos(gp) + 37.0*cos(gp+2.0*omp) -11.0*cos(2.0*(gp+omp));
  a = a*0.000277778*degrad + ic;
  c = (-109.0*sin(gp) + 37.0*sin(gp+2.0*omp) - 11.0*sin(2.0*(gp+omp)))/sin(ic);
  c = (c*0.000277778 + omg)*degrad;
  gn = -12.0*sin(gp) + 59.0*sin(gn) + 18.0*sin(2.0*omp);
  gn = gn*0.000277778*degrad;  // tau

  b *= degrad;
  gam = cos(a)*cos(b) + sin(a)*sin(b)*cos(c);
  gmp = gam*gam;
  if(gmp > 1.0) gmp = 0;
  else gmp = sqrt(1.0 - gmp);
  gam = atan23(gmp,gam);  // theta
  t = cos(a)*sin(b) - sin(a)*sin(b)*cos(c);
  l = -sin(a)*sin(c);

  gmp = atan23(l,t);  // phi
  t = sin(a)*cos(b) - cos(a)*sin(b)*cos(c);
  l = -sin(b)*sin(c);
  a = atan23(l,t);  // delta
  c = a + mln*degrad + gn - c;   // psi

  // libration rotation matrix from Mean equator to true selenographic
  m1 = zrot(gmp);
  m2 = xrot(gam);
  m1 = m2 * m1;
  m2 = zrot(c);
  m1 = m2 * m1;

  t = julcent(ss_time + ss_del_tdut/86400.0);
  m2 = pmatequ(0,t);  // convert from mean of J2000 to mean of epoch
  m1 = m1 * m2;

  return m1;
}

void SolarSystem::getPlanMat()  
{
 // get Matrix to convert from Equatorial into planetary coordinates

 double ag, dt;
 Mat3 mx, m1;
 
 ss_planmat_called = true;

 dt = ss_time + ss_del_tdut/86400.0;
 dt = julcent (dt);
 if (ss_central_body == 1) ss_planmat = getSelenographic();
 else
  {
   if (ss_central_body == 4)  // Earth
    {
     mx = pmatequ(0, dt);
     m1 = nutmat(dt, ag, false);
     mx = m1 * mx;
     m1 = zrot(lsidtim(ss_time, 0, ag)*M_PI/12.0);
    }
   else  // other planets
    {
     ag = (ss_axl0 + ss_axl1 * dt) * M_PI / 180.0; 
     mx = zrot(ag + M_PI / 2.0);
     ag = (ss_axb0 + ss_axb1 * dt) * M_PI / 180.0; 
     m1 = xrot(M_PI / 2.0 - ag);
     mx = m1 * mx;
     m1 = zrot((ss_W + ss_Wd*(ss_time+ss_del_tdut/86400.0-51544.5))*(M_PI/180.0));
    };
   ss_planmat = m1 * mx;
  }; 

 if (ss_epoch != 51544.5)  // correct for precession
  {
    if (ss_epoch == 0) ag = dt;
    else ag = julcent(ss_epoch);
    mx = pmatequ(ag, 0);
    ss_planmat = ss_planmat * mx;
  };

}

Vec3 SolarSystem::getPlanetocentric (double ra, double decl)
{
 // return unity vector which corresponds to planetocentric position of
 // sky point at R.A ra (in HH.MMSS) and Declination decl (in DDD.MMSS)

 double dra, ddec;
 Vec3 ru;

 if (!ss_update_called) updateSolar();
 if (!ss_planmat_called) getPlanMat();

 dra = 15.0*DmsDegF(ra)*degrad;
 ddec = DmsDegF(decl)*degrad;

 ru[0] = 1.0;
 ru[1] = dra;
 ru[2] = ddec;
 ru = polcar(ru);

 ru = mxvct (ss_planmat, ru);

 return ru;
}

void SolarSystem::getPlanetographic (double ra, double decl, double &lng, double &lat)
{
 // get planetogrphic longitude long and latitude lat (in decimal degrees) where
 // sky point at R.A ra (in HH.MMSS) and Declination decl (in DDD.MMSS) is at zenith.

 int j;
 double f, re, b1, b2, b3, c, sh, s, mp2; 
 Vec3 ru, s2;

 ru = getPlanetocentric (ra, decl);

 mp2 = 2.0 * M_PI;
 re = ss_R0;
 ru *= re;
 f = ss_flat;

 s2 = carpol(ru);
 ss_lat = s2[2];   // just preliminary
 ss_lng = s2[1];  // measured with the motion of rotation!
 if (ss_lng > mp2) ss_lng -= mp2;
 if (ss_lng < -M_PI) ss_lng += mp2;
 if (ss_lng > M_PI) ss_lng -= mp2;

 // get height above ellipsoid and geodetic latitude
 if (abs(ru) > 0.1)
  {
   if (f == 0)
    {
     ss_height = abs(ru) - re;
    }
   else
    {
     c = f*(2.0 - f);
     s = ru[0]*ru[0] + ru[1]*ru[1];
     sh = c*ru[2];

     for (j=0; j<4; j++)
      {
       b1 = ru[2] + sh;
       b3 = sqrt(s+b1*b1);
       if (b3 < 1e-5) b3 = sin(ss_lat);  // just in case
       else b3 = b1 / b3;
       b2 = re / sqrt(1.0 - c*b3*b3);
       sh = b2 * c * b3;
      };

     sh = ru[2] + sh;
     ss_lat = atan23(sh,sqrt(s));
     sh = sqrt(s+sh*sh) - b2;
     ss_height = sh;
    };
  } 
 else ss_height = 0;  // this should never happen
  
 ss_lat = ss_lat * 180.0 / M_PI;
 ss_lng = ss_lng * 180.0 / M_PI;

 lat = ss_lat;
 lng = ss_lng;  

}

void SolarSystem::putOrbitElements (double t0, double pdist, double ecc, double ran, double aper, double inc, double eclep)
{
 // store orbit elements for a hyperbolic, parabolic or highly elliptic heliocentric orbit

 ss_kepler_stored = true;
 ss_kepler_called = false;

 ss_t0 = t0;
 ss_m0 = -1.0;
 ss_a = pdist;
 ss_ecc = ecc;
 ss_ran = ran;
 ss_aper = aper;
 ss_inc = inc;
 ss_eclep = eclep;
}

void SolarSystem::putEllipticElements (double t0, double a, double m0, double ecc, double ran, double aper, double inc, double eclep)
{
 // store orbit elements for an elliptic heliocentric orbit

 ss_kepler_stored = true;
 ss_kepler_called = false;

 ss_t0 = t0;
 ss_m0 = m0;
 ss_a = a;
 ss_ecc = ecc;
 ss_ran = ran;
 ss_aper = aper;
 ss_inc = inc;
 ss_eclep = eclep;
}

void SolarSystem::getOrbitPosition (double& ra, double& decl)
{
 // get the position of the object for which the Kepler elements had been stored

 const double gs = 2.959122083e-4;  // gravitation constant of the Sun in AU and d
 double dt;
 int day, month, year;
 double b, eps2, yr;
 Mat3 pmx;

 Vec3 r1, v1;

 if (!ss_kepler_stored)
  {
   ra = -100.0;
   decl = 0;

   return;
  };

 if (!ss_update_called) updateSolar();

 ss_kepler_called = true;

 // calculate Kepler orbit

 dt = ss_time + ss_del_tdut/86400.0;

 kepler (gs, ss_t0, dt, ss_m0, ss_a, ss_ecc, ss_ran, ss_aper, ss_inc, r1, v1);

 // correct for precession
 if (ss_epoch != 0) eps2 = julcent(ss_epoch);
 else eps2 = julcent(dt);

 yr = ss_eclep;
 if (yr == 0) b = eps2;  // Mean of Date
 else
  {
   year = int(yr);
   b = 12.0 * (yr - double(year));
   month = int(b) + 1;
   day = 1;

   b = mjd(day, month, year, 0);
   b  = julcent(b); 
  };

 pmx = pmatecl(b, eps2);
 ss_comet = mxvct(pmx,r1);

 // convert into equatorial
 ss_comet = eclequ(eps2, ss_comet);

 // correct for nutation
 if (ss_nutation)
  {
   dt = julcent(dt);
   pmx = nutmat(dt, eps2, false);
   ss_comet = mxvct(pmx,ss_comet);
  };

 // refer to selected central body
 ss_comet = ss_comet + ss_rs;

 getRaDec (ss_comet, ra, decl);
}

double SolarSystem::getDistance()
{
 // distance in AU of Kepler object

 double ra, decl;

 if (!ss_kepler_stored) return 0;

 if (!ss_kepler_called) getOrbitPosition (ra, decl);

 return abs(ss_comet);
}

double SolarSystem::getCometMag(double g, double k)
{
 // apparent magnitude of comet. g = absolute magnitude, k = comet function

 double ra, decl;

 if (!ss_kepler_stored) return 0;

 if (!ss_kepler_called) getOrbitPosition (ra, decl);

 return g + 5.0 * log10(abs(ss_comet)) + k * log10(abs(ss_comet - ss_rs));

}

double SolarSystem::getAsteroidMag(double h, double g)
{
 // apparent magnitude of asteroid. h = absolute magnitude, g = slope parameter

 double ra, decl, s, t;

 if (!ss_kepler_stored) return 0;

 if (!ss_kepler_called) getOrbitPosition (ra, decl);

 ra = abs(ss_comet);
 decl = abs(ss_comet - ss_rs);
 t = 2.0 * ra * decl;
 s = abs(ss_rs);

 if (t > 0) t = (ra*ra + decl*decl - s*s) / t;
 else t = 0;  // just in case

 t = acos(t) / degrad;
 t /= 10.0;

 ra = h + 5.0 * log10(ra*decl) + g * t;
 
 return ra;  

}
