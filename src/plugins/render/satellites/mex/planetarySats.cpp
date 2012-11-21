//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP
//

/***************************************************************************
* Calculate Spacecraft around other planets                                *
*                                                                          *
*                                                                          *
* Open Source Code. License: GNU LGPL Version 2+                          *
*                                                                          *
* Author: Gerhard HOLTKAMP,        26-AUG-2012                              *
***************************************************************************/

/*------------ include files and definitions -----------------------------*/
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
using namespace std;

#include "planetarySats.h"
#include "attlib.h"
#include "astrolib.h"

// ################ Planetary Sats Class ####################

PlanetarySats::PlanetarySats()
{
 plsatinit();
}

PlanetarySats::~PlanetarySats()
{

}

double PlanetarySats::atan23 (double y, double x)
 {
  // redefine atan2 so that it does'nt crash when both x and y are 0 
  double result;

  if ((x == 0) && (y == 0)) result = 0;
  else result = atan2 (y, x);

  return result;
 }

void PlanetarySats::plsatinit()
{
 // initialize planetary sat data
  pls_moonflg = false;
  pls_day = 1;
  pls_month = 1;
  pls_year = 2012;
  pls_hour = 0;
  pls_minute = 0;
  pls_second = 0;
  pls_del_auto = 1;
  pls_step = 60.0;
  pls_delta_rt = 0.0;
  getTime();
  getMars();

  strcpy (pls_satelmfl, "./planetarysats.txt");

}

void PlanetarySats::getTime ()  // Get System Time and Date
{
  time_t tt;
  int hh, mm, ss;
  double jd, hr;

  tt = time(NULL);
  jd = 40587.0 + tt/86400.0; // seconds since 1-JAN-1970

  jd = jd + pls_delta_rt / 24.0;
  caldat(jd, hh, mm, ss, hr);
  pls_year = ss;
  pls_month = mm;
  pls_day = hh;

  dms(hr, hh, mm, jd);
  pls_hour = hh;
  pls_minute = mm;
  pls_second = int(jd);
  if (pls_del_auto) pls_del_tdut = DefTdUt(pls_year);
  setMJD(pls_year, pls_month, pls_day, hh, mm, jd);
};

void PlanetarySats::setStepWidth(double s)
{
  // set the step width (in seconds) used for calculations
  if (s < 0.01) pls_step = 0.01;
  else pls_step = s;
}

void PlanetarySats::setDeltaRT(double drt)
{
  pls_delta_rt = drt;
}

void PlanetarySats::setDeltaTAI_UTC(double d)
{
  // c is the difference between TAI and UTC according to the IERS
  // we have to add 32.184 sec to get to the difference TT - UT
  // which is used in the calculations here

  pls_del_tdut = d + 32.184;
  pls_del_auto = 0;
}

void PlanetarySats::setAutoTAI_UTC()
{
  // set the difference between TAI and UTC according to the IERS
  pls_del_auto = true;
  pls_del_tdut = DefTdUt(pls_year);
}

void PlanetarySats::setMJD(int year, int month, int day, int hour, int min, double sec)
{
    // set the (MJD-) time currently used for calculations to year, month, day, hour, min, sec
    double jd;

    pls_year = year;
    pls_month = month;
    pls_day = day;
    pls_hour = hour;
    pls_minute = min;
    pls_second = sec;

    jd = ddd(hour, min, sec);
    jd = mjd(day, month, year, jd);

    pls_time = jd;

    if (pls_del_auto) pls_del_tdut = DefTdUt(pls_year);
    
}

void PlanetarySats::getDatefromMJD(double mjd, int &year, int &month, int &day, int &hour, int &min, double &sec)
{
    // convert times given in Modified Julian Date (MJD) into conventional date and time

    double magn;

    caldat((mjd), day, month, year, magn);
    dms (magn,hour,min,sec);
    if (sec>30.0) min++;;
    if (min>59)
     {
      hour++;
      min=0;
     };
}

void PlanetarySats::setSatFile(char* fname)
{
  strcpy (pls_satelmfl, fname);
  
}

void PlanetarySats::setStateVector(double mjd, double x, double y, double z, double vx, double vy, double vz)
{
    pls_rep[0] = x;
    pls_rep[1] = y;
    pls_rep[2] = z;
    pls_vep[0] = vx;
    pls_vep[1] = vy;
    pls_vep[2] = vz;

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    double sec = 0;
    getDatefromMJD(mjd, year, month, day, hour, min, sec);
    setMJD(year, month, day, hour, min, sec);
    pls_tepoch = pls_time;
    //pls_tepoch = pls_time + pls_del_tdut / 86400.0;  // epoch in TT
}

int PlanetarySats::getStateVector(int nsat)
{
 // read the state vector from the planetary sat file
 // nsat = number of eligible sat to select (1 if first or only sat)
 // RETURN number of eligible sat selected, 0 if no suitable sats of file problems
 
 int fsc, j, k, nst, utc;
 int yr, month, day, hour, min;
 double sec;
 bool searching;
 ifstream cfle;
 char satname[40];  // name of satellite
 char plntname[40]; // name of planet
 
 strcpy(satname, "");
 strcpy(plntname, "");
 nst = 0;

 searching = true;
 fsc = 0;

 cfle.open(pls_satelmfl, ios::in);
 if (!cfle)
 {
  searching = false;
  cfle.close();
 };
 if(searching)
 {
  while (searching)
  {
    fsc = 1;

    if (!cfle.getline(satname,40)) fsc = 0; 
    else
    {
     k = strlen(satname);
     if ((k>1) && (satname[0] == '#'))
     {
      for (j=1; j<k; j++)
      {
       pls_satname[j-1] = satname[j]; 
       if(pls_satname[j-1] == '\n') pls_satname[j-1] = '\0';
      };
      pls_satname[k-1] = '\0';
     }
     else fsc = 0;
    };
  
    if (cfle.eof())
    {
     fsc = 0;
     searching = false;
    };

    if (fsc)
    {
     if (!cfle.getline(plntname, 40)) fsc = 0;
     else
     {
      k = strlen(plntname);
      if (k>0)
      {
       if(plntname[k-1] == '\n') plntname[k-1] = '\0';
       if((k>1) && (plntname[k-2] == '\r')) plntname[k-2] = '\0';
      };
     };
    };

    if (fsc)
    {
     cfle >> yr >> month >> day >> hour >> min >> sec >> utc;
     if (cfle.bad()) fsc = 0;
     if (cfle.eof())
     {
       fsc = 0;
       searching = false;
      };
    };

    if (fsc)
    {
     cfle >> pls_rep[0] >> pls_rep[1] >> pls_rep[2];
     if (cfle.bad()) fsc = 0;
     if (cfle.eof())
     {
       fsc = 0;
       searching = false;
      };
    };

    if (fsc)
    {
     cfle >> pls_vep[0] >> pls_vep[1] >> pls_vep[2];
     if (cfle.bad()) fsc = 0;
    };

    if (fsc)
    {
      if (strncmp(pls_plntname, plntname, 4) == 0) nst++;
      if (nst == nsat)
      {
        searching = false;

        setMJD(yr, month, day, hour, min, sec);
        pls_tepoch = pls_time;
        if (utc) pls_tepoch = pls_tepoch + pls_del_tdut/86400.0;  // epoch in TT
      };
    };
  };
  cfle.close(); 
 };

 if (fsc == 0) nst = 0;

 return nst;		 		
}	

void PlanetarySats::setPlanet(char* pname)
{
  pls_moonflg = false;
  strcpy(pls_plntname, pname);
  if (strncmp("Mars", pname, 4) == 0) getMars();
  if (strncmp("Venus", pname, 4) == 0) getVenus();
  if (strncmp("Mercury", pname, 4) == 0) getMercury();
  if (strncmp("Moon", pname, 4) == 0) getMoon();
}
	
void PlanetarySats::stateToKepler()
{
 // convert state vector (mean equatorial J2000.0) into planetary Kepler elements
 
 double t, dt, ag, gm, re, j2;
 double n, c, w, a, ecc, inc;
 Vec3 r1, v1;
 Mat3 mx;
 
 dt = (pls_tepoch - 51544.5) / 36525.0;
 gm = pls_GM * 7.4649600000; // convert from m^3/s^2 into km^3/d^2
 re = pls_R0;
 j2 = pls_J2;
    
 // convert into planet equatorial reference frame
 if (pls_moonflg)
 {
   mx = mxidn();
   r1 = mxvct (mx, pls_rep);
   v1 = mxvct (mx, pls_vep);

 }
 else
 {
   ag = (pls_axl0 + pls_axl1 * dt) * M_PI / 180.0; 
   mx = zrot(ag + M_PI / 2.0);
   r1 = mxvct (mx, pls_rep);
   v1 = mxvct (mx, pls_vep);
   
   ag = (pls_axb0 + pls_axb1 * dt) * M_PI / 180.0; 
   mx = xrot(M_PI / 2.0 - ag);
   r1 = mxvct (mx, r1);
   v1 = mxvct (mx, v1);
 };

 v1 *= 86400.0;  // convert into km / day
 
 oscelm(gm, pls_tepoch, r1, v1, t, pls_m0, pls_a, pls_ecc, pls_ra, pls_per, pls_inc);

 // now the mean motion
 a = pls_a;
 ecc = pls_ecc;
 inc = pls_inc;
      
 //  preliminary n
 if (a == 0) a = 1.0; // just in case
 if (a < 0) a = -a;   // just in case
 n = sqrt (gm / (a*a*a));

 // correct for J2 term
 w = 1.0 - ecc*ecc;
 if (w > 0)
 {
  w = pow (w, -1.5);
  c = sin (inc*M_PI/180.0);
  n = n*(1.0 + 1.5*j2*re*re/(a*a) * w * (1.0 - 1.5*c*c));
 }
 else n = 1.0;   // do something to avoid a domain error

 n = n / (2.0*M_PI);
 if (n > 1000.0) n = 1000.0;  // avoid possible errors

 pls_n0 = n;
  	 	 	 		 	 	 	
}
	
void PlanetarySats::getKeplerElements(double &perc, double &apoc, double &inc, double &ecc, double &ra, double &tano, double &m0, double &a, double &n0)
{
  // get Kepler elements of orbit with regard to the planetary equator and prime meridian.

  double t, gm;
  Vec3 r1, v1;
  Mat3 mx;


  if (pls_moonflg)  // for the Moon we normally work in J2000. Now get it into planetary
  {
    gm = pls_GM * 7.4649600000; // convert from m^3/s^2 into km^3/d^2
    
    mx = getSelenographic(pls_tepoch);
    r1 = mxvct (mx, pls_rep);
    v1 = mxvct (mx, pls_vep);
    v1 *= 86400.0;  // convert into km / day
 
    oscelm(gm, pls_tepoch, r1, v1, t, m0, a, ecc, ra, tano, inc);

    // now the mean motion
    if (a == 0) a = 1.0; // just in case
    if (a < 0) a = -a;   // just in case
    n0 = sqrt (gm / (a*a*a));
    n0 = n0 / (2.0*M_PI);
  }
  else
  {
    a = pls_a;
    n0 = pls_n0;
    m0 = pls_m0;
    tano = pls_per;
    ra = pls_ra;
    ecc = pls_ecc;
    inc = pls_inc;
  };

  perc = pls_a * (1.0 - pls_ecc) - pls_R0;
  apoc = pls_a * (1.0 + pls_ecc) - pls_R0;  

}

int PlanetarySats::selectSat(char* sname)
{
  // select specified satellite
  // RETURN 1 if successful, 0 if no suitable satellite found

  int nst, res, sl; 
  bool searching;

  searching = true;
  nst = 1;
  sl = strlen(sname);
  
  while (searching)
  {
    res = getStateVector(nst);
    if (res)
    {
      if (strncmp(pls_satname, sname, sl) == 0) searching = false;
    }
    else searching = false;
    nst++; 
  }; 
  
  return res;
}
	
void PlanetarySats::getSatName(char* sname)
{
  strcpy (sname, pls_satname);
}
	
void PlanetarySats::currentPos()
{
  getSatPos(pls_time);
}
	
void PlanetarySats::nextStep()
{
  pls_time = pls_time + pls_step / 86400.0;
  getSatPos(pls_time);
}

double PlanetarySats::getLastMJD()
{
  return pls_time;
}

void PlanetarySats::getPlanetographic(double &lng, double &lat, double &height)
{
  // planetographic coordinates from current state vector
  
  lng = pls_lng;
  lat = pls_lat;
  height = pls_height;
  	
}

void PlanetarySats::getFixedFrame(double &x, double &y, double &z, double &vx, double &vy, double &vz)
{
  // last state vector coordinates in planetary fixed frame

  x = pls_r[0];
  y = pls_r[1];
  z = pls_r[2];
  vx = pls_v[0];
  vy = pls_v[1];
  vz = pls_v[2];
}

void PlanetarySats::getSatPos (double tutc)
{
  // Get Position of Satellite at MJD-time t (UTC) 

  const double mp2 = 2.0*M_PI;

  double t, dt, m0, ran, aper, inc, a, ecc, n0, re;
  double f, e, c, s, k, sh, j2, gm, fac, b1, b2, b3;
  int j;

  Vec3 r1, v1, rg1, s2;
  Mat3 m1, m2;

  // prepare orbit calculation

  t = tutc + pls_del_tdut/86400.0;
  dt = t - pls_tepoch;

  ecc = pls_ecc;
  if (ecc >= 1.0) ecc = 0.999;  // to avoid crashes
  a = pls_a;
  n0 = mp2 * pls_n0;
  if (a < 1.0) a = 1.0;  // avoid possible crashes later on
  re = pls_R0;
  f = pls_flat;
  j2 = pls_J2;
  gm = pls_GM * 7.4649600000; // convert from m^3/s^2 into km^3/d^2

  // get current orbit elements ready
  aper = (re / a) / (1.0 - ecc*ecc);
  aper = 1.5 * j2 * aper*aper * n0;
  m0 = pls_inc*M_PI/180.0;
  ran = -aper * cos(m0) * dt;
  m0 = sin (m0);
  aper = aper * (2.0 - 2.5*m0*m0) * dt;
  ran = pls_ra * M_PI/180.0 + ran;
  aper = pls_per * M_PI/180.0 + aper;
  m0 = pls_m0*M_PI/180.0 + n0*dt;
  inc = pls_inc * M_PI/180.0;

  // solve Kepler equation
  if (a < 1.0) a = 1.0;  // avoid possible crashes later on
  e = eccanom(m0, ecc);
  fac = sqrt(1.0 - ecc*ecc);
  c = cos(e);
  s = sin(e);
  r1.assign (a*(c - ecc), a*fac*s, 0.0);

  m0 = 1.0 - ecc*c;
  k = sqrt(gm/a);
  v1.assign (-k*s/m0, k*fac*c/m0, 0.0);

  // convert into reference plane 
  m1 = zrot (-aper);
  m2 = xrot (-inc);
  m1 *= m2;
  m2 = zrot (-ran);
  m2 = m2 * m1;
  r1 = mxvct (m2, r1);
  v1 = mxvct (m2, v1);
  v1 /= 86400.0;

  // save state vector in planet-fixed frame

  if (pls_moonflg) m1 = getSelenographic(t);
  else m1 = zrot((pls_W + pls_Wd*(t-51544.5))*(M_PI/180.0));
  pls_r = mxvct(m1,r1);
  pls_v = mxvct(m1,v1);
  pls_r *= 1000.0;
  pls_v *= 1000.0;

  // get the groundtrack coordinates

  rg1 = mxvct(m1,r1);

  s2 = carpol(rg1);
  pls_lat = s2[2];   // just preliminary
  pls_lng = s2[1];  // measured with the motion of rotation!
  if (pls_lng > mp2) pls_lng -= mp2;
  if (pls_lng < -M_PI) pls_lng += mp2;
  if (pls_lng > M_PI) pls_lng -= mp2;

  // get height above ellipsoid and geodetic latitude
  if (abs(r1) > 0.1)
  {
   if (f == 0) pls_height = abs(r1) - re;
   else
   {
    c = f*(2.0 - f);
    s = r1[0]*r1[0] + r1[1]*r1[1];
    sh = c*r1[2];

    for (j=0; j<4; j++)
     {
      b1 = r1[2] + sh;
      b3 = sqrt(s+b1*b1);
      if (b3 < 1e-5) b3 = sin(pls_lat);  // just in case
      else b3 = b1 / b3;
      b2 = re / sqrt(1.0 - c*b3*b3);
      sh = b2 * c * b3;
     };

    sh = r1[2] + sh;
    pls_lat = atan20(sh,sqrt(s));
    sh = sqrt(s+sh*sh) - b2;
    pls_height = sh;
   }
  } 
  else pls_height = 0;  // this should never happen
  
  pls_lat = pls_lat * 180.0 / M_PI;
  pls_lng = pls_lng * 180.0 / M_PI;

 }
	

void PlanetarySats::getMars()  // Mars planetary constants
{
  pls_J2 = 1.964e-3;
  pls_R0 = 3397.2;
  pls_flat = 0.00647630;
  pls_axl0 = 317.681;
  pls_axl1 = -0.108;
  pls_axb0 = 52.886;
  pls_axb1 = -0.061;	
  pls_W = 176.868;
  pls_Wd = 350.8919830;
  pls_GM = 4.282828596416e+13; // 4.282837405582e+13
}	

void PlanetarySats::getVenus()  // Venus planetary constants
{ 
  pls_J2 = 0.027e-3;
  pls_R0 = 6051.9;
  pls_flat = 0.0;
  pls_axl0 = 272.72;
  pls_axl1 = 0.0;
  pls_axb0 = 67.15;
  pls_axb1 = 0.0;	
  pls_W = 160.26;
  pls_Wd = -1.4813596;
  pls_GM = 3.24858761e+14;  
}	

void PlanetarySats::getMercury()  // Mercury planetary constants
{ 
  pls_J2 = 0.0;
  pls_R0 = 2439.7;
  pls_flat = 0.0;
  pls_axl0 = 281.01;
  pls_axl1 = -0.033;
  pls_axb0 = 61.45;
  pls_axb1 = -0.005;	
  pls_W = 329.71;
  pls_Wd = 6.1385025;
  pls_GM = 2.20320802e+13;  
}	

void PlanetarySats::getMoon()  // Moon planetary constants
{
  pls_moonflg = true; 
  pls_J2 = 0.2027e-3;
  pls_R0 = 1738.0;
  pls_flat = 0.0;
  pls_axl0 = 0.0;
  pls_axl1 = 0.0;
  pls_axb0 = 90.0;
  pls_axb1 = 0.0;	
  pls_W = 0.0;
  pls_Wd = 13.17635898;
  pls_GM = 4.90279412e+12;  
}	

Mat3 PlanetarySats::getSelenographic (double jd)
{
  // Calculate the Matrix to transform from Mean of J2000 into selenographic
  // coordinates at MJD time jd.
  
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

  t = julcent(jd);
  m2 = pmatequ(0,t);  // convert from mean of J2000 to mean of epoch
  m1 = m1 * m2;

  return m1;
}

