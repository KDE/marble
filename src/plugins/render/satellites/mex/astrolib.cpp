//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP
//

/* =========================================================================
  Procedures needed for standard astronomy programs.
  The majority of procedures in this unit are taken from Montenbruck,
  Pfleger "Astronomie mit dem Personal Computer", Springer Verlag, 1989
  as well as from the "Explanatory Supplement to the Astronomical Almanac"
  University Science Books, Mill Valley, California, 1992
  and modified correspondingly.

  Linux C++ version
  Copyright : Gerhard HOLTKAMP          11-JAN-2012
  ========================================================================= */

#include <cmath>
using namespace std;

#include "attlib.h"
#include "astrolib.h"

double frac (double f)
 { return fmod(f,1.0); }

/*--------------- Function ddd ------------------------------------------*/

double ddd (int d, int m, double s)
 {
  /*
    Conversion from Degrees, Minutes, Seconds into decimal degrees

    INPUT :
           d : degrees
           m : minutes
           s : seconds (and fractions)

    OUTPUT :
           dd : decimal degrees
   */
  double dd;

  if ( (d < 0) || (m < 0) || (s < 0)) dd = -1.0;
  else dd = 1.0;
  dd = dd * (fabs(double(d)) + fabs(double(m))/60.0 + fabs(s)/3600.0);

  return dd;
 }

/*--------------- Function dms ------------------------------------------*/

void dms (double dd, int &d, int &m, double &s)
 /*
   Conversion from decimal degrees into Degrees, Minutes, Seconds

   INPUT :
          dd : decimal degrees

   OUTPUT :
          d : degrees
          m : minutes
          s : seconds (and fractions)
  */
 {
  double d1;

  d1 = fabs(dd);
  // d = int(floor(d1));
  d = int(d1);
  d1 = (d1 - d) * 60.0;
  // m = int(floor(d1));
  m = int(d1);
  s = (d1 - m) * 60.0;
  if (dd < 0)
   {
    if (d != 0) d = -d;
    else
     {
      if (m != 0) m = -m;
      else s = -s;
     };
   };
 }

 /*------------ FUNCTION mjd ----------------------------------------------*/

double mjd (int day, int month, int year, double hour)
/*
  Modified Julian Date ( MJD = Julian Date - 2400000.5)
  valid for every date
  Julian Calendar up to 4-OCT-1582,
  Gregorian Calendar from 15-OCT-1582.
  */
 {
  double a;
  long int b, c;

  a = 10000.0 * year + 100.0 * month + day;
  if (month <= 2)
   {
    month = month + 12;
    year = year - 1;
   };
  if (a <= 15821004.1)
   {
    b = ((year+4716)/4) - 1181;
    if (year < -4716)
     {
      c = year + 4717;
      c = -c;
      c = c / 4;
      c = -c;
      b = c - 1182;
     };
   }
  else b = (year/400) - (year/100) + (year/4);
  //	 { b = -2 + floor((year+4716)/4) - 1179;}
  // else {b = floor(year/400) - floor(year/100) + floor(year/4);};

  a = 365.0 * year - 679004.0;
  a = a + b + int(30.6001 * (month + 1)) + day + hour / 24.0;

  return a;
 }

/*---------------- Function julcent ---------------------------------------*/

double julcent (double mjuld)
 {
  /*
	 Julian Centuries since J2000.0 of Modified Julian Date mjuld.
  */
  return (mjuld - 51544.5) / 36525.0;
 }

/*---------------- Function caldat -----------------------------------------*/

void caldat (double mjd, int &day, int &month, int &year, double &hour)
  /*
    Calculate the calendar date from the Modified Julian Date

    INPUT :
           mjd : Modified Julian Date (Julian Date - 2400000.5)

    OUTPUT :
           day, month, year : corresponding date
           hour : corresponding hours of the above date
   */
 {
  long int b, c, d, e, f, jd0;

  jd0 = long(mjd +  2400001.0);
  if (jd0 < 2299161) c = jd0 + 1524;    /* Julian calendar */
  else
   {                                /* Gregorian calendar */
    b = long (( jd0 - 1867216.25) / 36524.25);
    c = jd0 + b - (b/4) + 1525;
   };

  if (mjd < -2400001.0)  // special case for year < -4712
   {
    if (mjd == floor(mjd)) jd0 = jd0 + 1;
    c = long((-jd0 - 0.1)/ 365.25);
    c = c + 1;
    year = -4712 - c;
    d = c / 4;
    d = c * 365 + d;  // number of days from JAN 1, -4712
    f = d + jd0;  // day of the year
    if ((c % 4) == 0) e = 61;
    else e = 60;
    if (f == 0)
     {
      year = year - 1;
      month = 12;
      day = 31;
      f = 500;  // set as a marker
     };
    if (f < e)
     {
      if (f < 32)
       {
         month = 1;
         day = f;
       }
      else
       {
         month = 2;
         day = f - 31;
       };
     }
    else
     {
       if (f < 500)
        {
         f = f - e;
         month = long((f + 123.0) / 30.6001);
         day = f - long(month * 30.6001) + 123;
         month = month - 1;
        };
     };
   }
  else   // normal case
   {
    d = long ((c - 122.1) / 365.25);
    e = 365 * d + (d/4);
    f = long ((c - e) / 30.6001);
    day = c - e - long(30.6001 * f);
    month = f - 1 - 12 * (f / 14);
    year = d - 4715 - ((7 + month) / 10);
   };

  hour = 24.0 * (mjd - floor(mjd));
 }

/*---------------- Function DefTdUt --------------------------------------*/
double DefTdUt (int yi)
 {
  /*
     Get a suitable default for value of TDT - UT in year yi in seconds.
   */

  const int td[9] = {55,55,56,56,57,58,58,59,60};
  double t, result;
  int yy, yr;

  yr = yi;

  if (yr > 1899)
   {
    if (yr >= 2000)  // this corrects for observations until DEC 2006
     {
      if (yr > 2006) yr -= 1;
      if (yr > 2006) yr -= 1;  // no leap second at the end of 2007
      if (yr > 2007) yr -= 1;  // no leap second at the end of 2009
      if (yr > 2007) yr -= 1;  // no leap second at the end of 2010
      yr -= 6;
      if (yr < 1999) yr = 1999;
     };
    t = yr - 2000;
    t = t / 100.0;
    result = (((-339.84*t - 516.12)*t -160.22)*t + 92.23)*t + 71.28;
    if (yr > 2013)
     {
      t = yr - 2013;
      t = t / 100.0;
      result = (27.5*t + 75.0)*t + 73.0;
     }
    else if (yr > 1994)
     {
      result -= 6.28;
     }
    else if (yr  > 1985)
     {
      yy = yr - 1986;
      result = td[yy];
     };
   };

  if (yr < 1900)
   {
    t = yr - 1800;
    t = t / 100;
    if (yr > 1649)
     {
      t = t - 0.19;
      result = 5.156 + 13.3066 * t * t;
      if (yr > 1864) result = -0.6*(yr - 1865) + 6;
      if (yr > 1884) result = -0.2*(yr - 1885) - 6;
     }
    else
     {
      if (yr > 947) result = 25.5 * t*t;
      else result = 1360 + (44.3*t + 320.0) * t;
     };
   };

// round to full seconds

  if (result < 0)
   {
    t = -result;
    t += 0.5;
    result = - floor(t);
   }
  else result = floor(result + 0.5);

  result += 0.184;  // because of TT = TAI + 32.184;

  return result;
 }

/*---------------- Function lsidtim -------------------------------------*/

double lsidtim (double jd, double lambda, double ep2)
 {
  /* Calculate the Apparent Local Mean Sidereal Time (in decimal hours) for
	  the Modified Julian Date jd and geographic longitude lambda (in degrees)
	  and with the correction (AST - MST) eps (given in seconds)
	 */
	double lmst;
	double t, ut, gmst;
	int mjd0;

	mjd0 = int(jd);
	ut = (jd - mjd0)*24.0;
	t = (mjd0 - 51544.5) / 36525.0;
	gmst = 6.697374558 + 1.0027379093*ut
			 + (8640184.812866 + (0.093104 - 6.2e-6*t)*t)*t/3600.0;
	lmst = 24.0 * frac((gmst + lambda/15.0) / 24.0);
	lmst = lmst + ep2 / 3600.0;

	return lmst;
 }

/*---------------- Function eps -----------------------------------------*/

double eps (double t)  //   obliquity of ecliptic
 {
  /*
	 Obliquety of ecliptic eps (in radians)
	 at time t (in Julian Centuries from J2000.0)
	 */
	 double tp;

	 tp = 23.43929111 - (46.815+(0.00059-0.001813*t)*t)*t/3600.0;
	 tp = 1.74532925199e-2 * tp;

	 return tp;
  }

/*---------------- Function eclequ -----------------------------------------*/

Vec3 eclequ (double t, Vec3& r1)  //  ecliptic -> equatorial
 {
  /*
	 Convert position vector r1 from ecliptic into equatorial coordinates
	 at t (in Julian Centuries from J2000.0 )
	 */

	 Mat3 m;
	 Vec3 r2;

	 m = xrot (-eps(t));
	 r2 =	mxvct (m, r1);
	 return r2;
  }

/*---------------- Function equecl -----------------------------------------*/

Vec3 equecl (double t, Vec3& r1)    //  equatorial -> ecliptic
 {
  /*
	 Convert position vector r1 from equatorial into ecliptic coordinates
	 at t (in Julian Centuries from J2000.0)
	 */

	 Mat3 m;
	 Vec3 r2;

	 m = xrot (eps(t));
	 r2 =	mxvct (m, r1);
	 return r2;
  }

/*---------------- Function pmatecl -----------------------------------------*/

Mat3 pmatecl (double t1, double t2)  //  ecl. precession
 {
  /*
	 Calculate ecliptic precession matrix a from t1 to t2
	 (times in Julian Centuries from J2000.0)
	 */

        const double  secrad = 4.8481368111e-6;  // arcsec -> radians

        double ppi, pii, pa, dt;
        Mat3   m1, m2, a;

	dt = t2 - t1;
	ppi = 174.876383889 + (((3289.4789+0.60622*t1)*t1) +
			((-869.8089-0.50491*t1) + 0.03536*dt)*dt) / 3600.0;
	ppi = ppi * 1.74532925199e-2;
	pii = ((47.0029-(0.06603-0.000598*t1)*t1) +
		  ((-0.03302+0.000598*t1)+0.00006*dt)*dt)*dt * secrad;
	pa = ((5029.0966+(2.22226-0.000042*t1)*t1) +
			 ((1.11113-0.000042*t1)-0.000006*dt)*dt)*dt * secrad;

	pa = ppi + pa;
	m1 = zrot (-pa);
	m2 = xrot (pii);
	m1 = m1 * m2;
	m2 = zrot (ppi);
	a = m1 * m2;

	 return a;
  }

/*---------------- Function pmatequ --------------------------------------*/

Mat3 pmatequ (double t1, double t2)  //  equ. precession
 {
  /*
    Calculate equatorial precession matrix a from t1 to t2
    (times in Julian Centuries from J2000.0)
   */
  const double  secrad = 4.8481368111e-6;  // arcsec -> radians

  double  dt, zeta, z, theta;
  Mat3   m1, m2, a;

  dt = t2 - t1;
  zeta = ((2306.2181+(1.39656-0.000139*t1)*t1) +
              ((0.30188-0.000345*t1)+0.017998*dt)*dt)*dt * secrad;
  z = zeta + ((0.7928+0.000411*t1)+0.000205*dt)*dt*dt * secrad;
  theta = ((2004.3109-(0.8533+0.000217*t1)*t1) -
           ((0.42665+0.000217*t1)+0.041833*dt)*dt)*dt * secrad;

  m1 = zrot (-z);
  m2 = yrot (theta);
  m1 = m1 * m2;
  m2 = zrot (-zeta);
  a = m1 * m2;

  return a;
 }

/*---------------- Function nutmat --------------------------------------*/

Mat3 nutmat (double t, double& ep2, bool hpr)
 {
  /*
    Calculate nutation matrix a from mean to true equatorial coordinates
    at time t (in Julian Centuries from J2000.0)
    Also calculates the correction ep2 for apparent sidereal time in sec

   if hpr is true a high precision is used, otherwise a low precision
    (only the first 50 terms of the nutation theory are used)
	 */
  const int  ntb1 = 15;
  const int tb1[ntb1][5] =
   {
    {  0, 0, 0, 0, 1}, //   1
    {  0, 0, 0, 0, 2}, //   2
    {  0, 0, 2,-2, 2}, //   9
    {  0, 1, 0, 0, 0}, //  10
    {  0, 1, 2,-2, 2}, //  11
    {  0,-1, 2,-2, 2}, //  12
    {  0, 0, 2,-2, 1}, //  13
    {  0, 2, 0, 0, 0}, //  16
    {  0, 2, 2,-2, 2}, //  18
    {  0, 0, 2, 0, 2}, //  31
    {  1, 0, 0, 0, 0}, //  32
    {  0, 0, 2, 0, 1}, //  33
    {  1, 0, 2, 0, 2}, //  34
    {  1, 0, 0, 0, 1}, //  38
    { -1, 0, 0, 0, 1}, //  39
   };

  const int  ntb2 = 35;
  const int tb2[ntb2][5] =
  {
    { -2, 0, 2, 0, 1}, //   3
    {  2, 0,-2, 0, 0}, //   4
    { -2, 0, 2, 0, 2}, //   5
    {  1,-1, 0,-1, 0}, //   6
    {  0,-2, 2,-2, 1}, //   7
    {  2, 0,-2, 0, 1}, //   8
    {  2, 0, 0,-2, 0}, //  14
    {  0, 0, 2,-2, 0}, //  15
    {  0, 1, 0, 0, 1}, //  17
    {  0,-1, 0, 0, 1}, //  19
    { -2, 0, 0, 2, 1}, //  20
    {  0,-1, 2,-2, 1}, //  21
    {  2, 0, 0,-2, 1}, //  22
    {  0, 1, 2,-2, 1}, //  23
    {  1, 0, 0,-1, 0}, //  24
    {  2, 1, 0,-2, 0}, //  25
    {  0, 0,-2, 2, 1}, //  26
    {  0, 1,-2, 2, 0}, //  27
    {  0, 1, 0, 0, 2}, //  28
    { -1, 0, 0, 1, 1}, //  29
    {  0, 1, 2,-2, 0}, //  30
    {  1, 0, 0,-2, 0}, //  35
    { -1, 0, 2, 0, 2}, //  36
    {  0, 0, 0, 2, 0}, //  37
    { -1, 0, 2, 2, 2}, //  40
    {  1, 0, 2, 0, 1}, //  41
    {  0, 0, 2, 2, 2}, //  42
    {  2, 0, 0, 0, 0}, //  43
    {  1, 0, 2,-2, 2}, //  44
    {  2, 0, 2, 0, 2}, //  45
    {  0, 0, 2, 0, 0}, //  46
    { -1, 0, 2, 0, 1}, //  47
    { -1, 0, 0, 2, 1}, //  48
    {  1, 0, 0,-2, 1}, //  49
    { -1, 0, 2, 2, 1}, //  50
   };

  const double tb3[ntb1][4] =
   {
    {-171996.0,-174.2,  92025.0,   8.9 },   //   1
    {   2062.0,   0.2,   -895.0,   0.5 },   //   2
    { -13187.0,  -1.6,   5736.0,  -3.1 },   //   9
    {   1426.0,  -3.4,     54.0,  -0.1 },   //  10
    {   -517.0,   1.2,    224.0,  -0.6 },   //  11
    {    217.0,  -0.5,    -95.0,   0.3 },   //  12
    {    129.0,   0.1,    -70.0,   0.0 },   //  13
    {     17.0,  -0.1,      0.0,   0.0 },   //  16
    {    -16.0,   0.1,      7.0,   0.0 },   //  18
    {  -2274.0,  -0.2,    977.0,  -0.5 },   //  31
    {    712.0,   0.1,     -7.0,   0.0 },   //  32
    {   -386.0,  -0.4,    200.0,   0.0 },   //  33
    {   -301.0,   0.0,    129.0,  -0.1 },   //  34
    {     63.0,   0.1,    -33.0,   0.0 },   //  38
    {    -58.0,  -0.1,     32.0,   0.0 },   //  39
   };

  const double tb4[ntb2][2] =
  {
    { 46.0, -24.0}, //   3
    { 11.0,  0.0 }, //   4
    { -3.0,  1.0 }, //   5
    { -3.0,  0.0 }, //   6
    { -2.0,  1.0 }, //   7
    {  1.0,  0.0 }, //   8
    { 48.0,  1.0 }, //  14
    {-22.0,  0.0 }, //  15
    {-15.0,  9.0 }, //  17
    {-12.0,  6.0 }, //  19
    { -6.0,  3.0 }, //  20
    { -5.0,  3.0 }, //  21
    {  4.0, -2.0 }, //  22
    {  4.0, -2.0 }, //  23
    { -4.0,  0.0 }, //  24
    {  1.0,  0.0 }, //  25
    {  1.0,  0.0 }, //  26
    { -1.0,  0.0 }, //  27
    {  1.0,  0.0 }, //  28
    {  1.0,  0.0 }, //  29
    { -1.0,  0.0 }, //  30
    {-158.0,-1.0 }, //  35
    {123.0,-53.0 }, //  36
    { 63.0, -2.0 }, //  37
    {-59.0, 26.0 }, //  40
    {-51.0, 27.0 }, //  41
    {-38.0, 16.0 }, //  42
    { 29.0, -1.0 }, //  43
    { 29.0,-12.0 }, //  44
    {-31.0, 13.0 }, //  45
    { 26.0, -1.0 }, //  46
    { 21.0,-10.0 }, //  47
    { 16.0, -8.0 }, //  48
    {-13.0,  7.0 }, //  49
    {-10.0,  5.0 }, //  50
   };

   const double  secrad = 4.8481368111e-6;  // arcsec -> radians
   const double  p2 = 2.0 * M_PI;

   double ls, lm, d, f, n, dpsi, deps, ep0;
   int j;
   Mat3   m1, m2, a;

   if (hpr)
    {
     lm = 2.355548393544 +
          (8328.691422883903 + (0.000151795164 + 0.000000310281*t)*t)*t;
     ls = 6.240035939326 +
          (628.301956024185 + (-0.000002797375 - 0.000000058178*t)*t)*t;
     f = 1.627901933972 +
          (8433.466158318464 + (-0.000064271750 + 0.000000053330*t)*t)*t;
     d = 5.198469513580 +
          (7771.377146170650 + (-0.000033408511 + 0.000000092115*t)*t)*t;
     n = 2.182438624361 +
          (-33.757045933754 + (0.000036142860 + 0.000000038785*t)*t)*t;

     lm = fmod(lm,p2);
     ls = fmod(ls,p2);
     f = fmod(f,p2);
     d = fmod(d,p2);
     n = fmod(n,p2);

     dpsi = 0.0;
     deps = 0.0;
     for(j=0; j<ntb1; ++j)
      {
       ep0 =  tb1[j][0]*lm + tb1[j][1]*ls + tb1[j][2]*f + tb1[j][3]*d + tb1[j][4]*n;
       dpsi = dpsi + (tb3[j][0]+tb3[j][1]*t) * sin(ep0);
       deps = deps + (tb3[j][2]+tb3[j][3]*t) * cos(ep0);
      };
     for(j=0; j<ntb2; ++j)
      {
       ep0 =  tb2[j][0]*lm + tb2[j][1]*ls + tb2[j][2]*f + tb2[j][3]*d + tb2[j][4]*n;
       dpsi = dpsi + tb4[j][0] * sin(ep0);
       deps = deps + tb4[j][1] * cos(ep0);
      };
     dpsi = 1.0e-4 * dpsi * secrad;
     deps = 1.0e-4 * deps * secrad;
    }

   else   // low precision
    {
     ls = p2 * frac (0.993133+99.997306*t);  //  mean anomaly sun
		 d = p2 * frac (0.827362+1236.853087*t); //  diff long. moon-sun
		 f = p2 * frac (0.259089+1342.227826*t); //  dist. node
		 n = p2 * frac (0.347346 - 5.372447*t);  //  long. node

		 dpsi = (-17.2*sin(n) - 1.319*sin(2*(f-d+n)) - 0.227*sin(2*(f+n))
					+ 0.206*sin(2*n) + 0.143*sin(ls)) * secrad;
		 deps = (+9.203*cos(n) + 0.574*cos(2*(f-d+n)) + 0.098*cos(2*(f+n))
					-0.09*cos(2*n) ) * secrad;
    };

   ep0 = eps (t);
   ep2 = ep0 + deps;
   m1 = xrot (ep0);
   m2 = zrot (-dpsi);
   m1 *= m2;
   m2 = xrot (-ep2);
   a = m2 * m1;
   ep2 = dpsi * cos (ep2);
   ep2 *= 13750.9870831;   // convert radians into time-seconds

   return a;
 }

/*---------------- Function nutecl --------------------------------------*/

Mat3 nutecl (double t, double& ep2)  //  nutation matrix (ecliptic)
 {
  /*
	 Calculate nutation matrix a from mean to true ecliptic coordinates
	 at time t (in Julian Centuries from J2000.0)
	 Also calculates the correction ep2 for apparent sidereal time in sec
	 */

	 const double secrad = 4.8481368111e-6; //   arcsec -> radians
	 const double p2 = 2.0 * M_PI;

	 double ls, d, f, n, dpsi, deps, ep0;
	 Mat3   m1, m2, a;

	ls = p2 * frac (0.993133+99.997306*t);  //  mean anomaly sun
	d = p2 * frac (0.827362+1236.853087*t); //  diff long. moon-sun
	f = p2 * frac (0.259089+1342.227826*t); //  dist. node
	n = p2 * frac (0.347346 - 5.372447*t);  //  long. node

	dpsi = (-17.2*sin(n) - 1.319*sin(2*(f-d+n)) - 0.227*sin(2*(f+n))
				+ 0.206*sin(2*n) + 0.143*sin(ls)) * secrad;

	deps = (+9.203*cos(n) + 0.574*cos(2*(f-d+n)) + 0.098*cos(2*(f+n))
				-0.09*cos(2*n) ) * secrad;

	ep0 = eps (t);
	ep2 = ep0 + deps;
	m1 = xrot (-deps);
	m2 = zrot (-dpsi);
	a = m1 * m2;
	ep2 = dpsi * cos (ep2);
	ep2 *= 13750.9870831;   // convert radians into time-seconds

        return a;
  }

/*---------------- Function pmatequ --------------------------------------*/

Mat3 PoleMx (double xp, double yp)
 {
  /* Returns Polar Motion matrix.
     xp and yp are the coordinates of the Celestial Ephemeris Pole
     with respect to the IERS Reference Pole in arcsec as published
     in the IERS Bulletin B.
  */
  const double arctrd = M_PI / (180.0*3600.0);
  Mat3 res;
  double xr, yr;

  xr = xp * arctrd;
  yr = yp * arctrd;
  res.assign(1.0,0.0,xr,0.0,1.0,-yr,-xr,yr,1.0);

  return res;
 }

/*---------------- Function aberrat --------------------------------------*/

Vec3 aberrat (double t, Vec3& ve)   //  aberration
 {
  /*
	 Correct position vector ve for aberration into new position va
	 at time t (in Julian Centuries from J2000.0)
	*/
        const double p2 = 2.0 * M_PI;

        double l, cl, d0;
        Vec3 va;

	d0 = abs(ve);
	l = p2 * frac(0.27908+100.00214*t);
	cl = cos(l)*d0;
	va[0] = ve[0] - 9.934e-5 * sin(l) * d0;
	va[1] = ve[1] + 9.125e-5 * cl;
	va[2] = ve[2] + 3.927e-5 * cl;

        return va;
  }

/*--------------------- Function GeoPos ----------------------------------*/

Vec3 GeoPos (double jd, double ep2, double lat, double lng, double ht)
 {
  /* Return the geocentric vector (in the equatorial system) of the
	  geographic position given by the latitude lat and longitude lng
	  (in radians) and height ht (in m) at the MJD-time jd (UT).
	  ep2 : correction for apparent sidereal time in sec

	  The length unit of the vector is in terms of the equatorial
	  Earth radius (6378.137 km)
	 */
	 double const e2 = 6.69438499959e-3;
	 double np, h, sp;

	 Vec3 r;

	 sp = sin(lat);
	 h = ht / 6378.137e3;
	 np = 1.0 / (sqrt(1.0 - e2*sp*sp));
	 r[2] = ((1.0 - e2)*np + h)*sp;

	 sp = (np + h) * cos(lat);
	 np = lsidtim(jd, (lng*180.0/M_PI), ep2) * M_PI/12.0;

	 r[0] = sp * cos(np);
	 r[1] = sp * sin(np);

	 return r;
  }

Vec3 GeoPos (double jd, double ep2, double lat, double lng, double ht,
              double xp, double yp)
 {
  /* Return the geocentric vector (in the equatorial system) of the
	  geographic position given by the latitude lat and longitude lng
	  (in radians) and height ht (in m) at the MJD-time jd (UT).

    ep2 : correction for apparent sidereal time in sec
    xp, yp: coordinates of polar motion in arcsec.

    The length unit of the vector is in terms of the equatorial
    Earth radius (6378.137 km)
   */
   double const e2 = 6.69438499959e-3;
   double np, h, sp;
   Mat3 prx;
   Vec3 r;

   sp = sin(lat);
   h = ht / 6378.137e3;
   np = 1.0 / (sqrt(1.0 - e2*sp*sp));
   r[2] = ((1.0 - e2)*np + h)*sp;
   sp = (np + h) * cos(lat);
   r[0] = sp * cos(lng);
   r[1] = sp * sin(lng);

   // correct for polar motion
   if ((xp != 0) || (yp != 0))
    {
     prx = mxtrn(PoleMx(xp, yp));
     r = mxvct(prx, r);
    };

   // convert into equatorial
   np = lsidtim(jd, 0.0, ep2) * M_PI/12.0;
   prx = zrot(-np);
   r = mxvct(prx, r);

   return r;
  }

/*--------------------- Function EquHor ----------------------------------*/

Vec3 EquHor (double jd, double ep2, double lat, double lng, Vec3 r)
 {
  /* convert vector r from the equatorial system into the horizontal system.
	  jd = MJD-time (UT)
	  ep2 : correction for apparent sidereal time in sec
	  lat, lng : geographic latitude and longitude (in radians)
	 */
  double lst;
  Vec3 s;
  Mat3 mx;

  lst = lsidtim(jd, (lng*180.0/M_PI), ep2) * M_PI/12.0;
  mx = zrot(lst);
  s = mxvct(mx, r);
  mx = yrot(M_PI/2.0 - lat);
  s = mxvct(mx, s);

  return s;
 }

/*--------------------- Function HorEqu ----------------------------------*/


Vec3 HorEqu (double jd, double ep2, double lat, double lng, Vec3 r)
 {
  /* convert vector r from the horizontal system into the equatorial system.
	  jd = MJD-time (UT)
	  ep2 : correction for apparent sidereal time in sec
	  lat, lng : geographic latitude and longitude (in radians)
	 */
  double lst;
  Vec3 s;
  Mat3 mx;

  mx = yrot(lat - M_PI/2.0);
  s = mxvct(mx, r);
  lst = -lsidtim(jd, (lng*180.0/M_PI), ep2) * M_PI/12.0;
  mx = zrot(lst);
  s = mxvct(mx, s);

  return s;
 }

/*--------------------- Function AppPos ----------------------------------*/

void AppPos (double jd, double ep2, double lat, double lng, double ht,
				 int solsys, Vec3 r, double& azim, double& elev, double& dist)
 {
  /* get apparent position in the horizontal system
	  jd = MJD-time (UT)
	  ep2 : correction for apparent sidereal time in sec
	  lat, lng : geographic latitude and longitude (in radians)
	  ht : height above normal in meters.
	  solsys : = 1 if object is in solar system and parallax has to be
						taken into account, 0 otherwise.
	  r = vector of celestial object. The unit of lenght of this vector
			has to be in terms of the equatorial Earth radius (6378.14 km)
			if solsys = 1, otherwise it's arbitrary.
	  azim : azimuth in radians (0 is to the North).
	  elev : elevation in radians
	  dist : distance (if solsys = 1; otherwise abs(r))
	 */
  Vec3 s;

  // correct for topocentric position (parallax)
  if (solsys) s = r - GeoPos(jd, ep2, lat, lng, ht);
  else s = r;

  s = EquHor(jd, ep2, lat, lng, s);
  s = carpol(s);
  dist = s[0];
  elev = s[2];
  azim = M_PI - s[1];
 }

/*--------------------- Function AppRADec ----------------------------------*/

void AppRADec (double jd, double ep2, double lat, double lng,
                double azim, double elev, double& ra, double& dec)
 {
  /* get apparent position in the horizontal system
	  jd = MJD-time (UT)
	  ep2 : correction for apparent sidereal time in sec
	  lat, lng : geographic latitude and longitude (in radians)
	  azim : azimuth in radians (0 is to the North).
	  elev : elevation in radians
	  ra : Right Ascension (in radians)
	  dec : Declination (in radians)
    */
	 Vec3 s;

   s[0] = 1.0;
   s[1] = M_PI - azim;
   s[2] = elev;
   s = polcar(s);
   s = HorEqu(jd, ep2, lat, lng, s);
   s = carpol(s);
   dec = s[2];
   ra = s[1];
 }

/*------------------------- Refract ---------------------------------*/

double Refract (double h, double p, double t)
 {
  /* Calculate atmospheric refraction with low precision
	  h = height (in radians) of object
	  p = presure (in millibars)
	  t = temperature (in degrees Celsius)

	  RETURN: refraction angle in radians
   */
  double const raddeg = 180.0 / M_PI;
  double r;

  r = h * raddeg;
  r = (r + 7.31 / (r + 4.4)) / raddeg;
  r = (0.28*p/(t+273.0)) * 0.0167 / tan(r);
  r = r / raddeg;

  return r;
 }

/*---------------- Function eccanom --------------------------------------*/

double eccanom (double man, double ecc)
 {
  /*
	 Solve Kepler equation for eccentric anomaly of elliptical orbits.
	 man : Mean Anomaly (in radians)
	 ecc : eccentricity
	 */
	 const double p2 = 2.0*M_PI;
	 const double eps = 1E-11;
	 const int maxit = 15;

	 double  m, e, f;
	 int i, mi;

	m=man/p2;
	mi = int(m);
	m=p2*(m-mi);
	if (m < 0)  m=m+p2;
	if (ecc < 0.8) e=m;
	else e=M_PI;
	f = e - ecc*sin(e) - m;
	i=0;

	while ((fabs(f) > eps) && (i < maxit))
	 {
	  e = e - f / (1.0 - ecc*cos(e));
	  f = e - ecc*sin(e) - m;
	  i = i + 1;
	 }

        return  e;
 }


/*---------------- Function hypanom --------------------------------------*/

double hypanom (double mh, double ecc)
 {
  /*
	 Solve Kepler equation for eccentric anomaly of hyperbolic orbits.
	 mh : Mean Anomaly
	 ecc : eccentricity
	 */
	 const double eps = 1E-11;
	 const int maxit = 15;

	 double  h, f;
	 int i;

	h = log (2.0*fabs(mh)/ecc+1.8);
	if (mh < 0.0) h = -h;
	f = ecc * sinh(h) - h - mh;
	i = 0;

	while ((fabs(f) > eps*(1.0+fabs(h+mh))) && (i < maxit))
	 {
	  h = h - f / (ecc*cosh(h) - 1.0);
	  f = ecc*sinh(h) - h - mh;
	  i = i + 1;
	 }

        return h;
  }


/*------------------ Function ellip ------------------------------------*/

void ellip (double gm, double t0, double t, double a, double ecc,
				double m0, Vec3& r1, Vec3& v1)
 {
  /*
	 Calculate position r1 and velocity v1 of for elliptic orbit at time t.
	 gm : Gravitational Constant
	 t0 : epoch
	 a : semim-ajor axis
	 ecc : eccentricity
	 m0 : Mean Anomaly at epoch (in radians).
	 The units must be consistent with each other.
	 */
	 double m, e, fac, k, c, s;

	 if (fabs(a) < 1e-60) a = 1e-60;
	 k = gm / a;
	 if (k >= 0) k = sqrt(k);
	 else k = 0;    // just in case

	 m = k * (t - t0) / a + m0;
	 e = eccanom(m, ecc);
	 fac = sqrt(1.0 - ecc*ecc);
	 c = cos(e);
	 s = sin(e);
	 r1.assign (a*(c - ecc), a*fac*s, 0.0);
	 m = 1.0 - ecc*c;
	 v1.assign (-k*s/m, k*fac*c/m, 0.0);
  }

/*---------------- Function hyperb --------------------------------------*/

void hyperb (double gm, double t0, double t, double a, double ecc,
				 Vec3& r1, Vec3& v1)
 {
  /*
	 Calculate position r1 and velocity v1 of for hyperbolic orbit at time t.
	 gm : Gravitational Constant
	 t0 : time of perihelion passage
	 a : semi-major axis
	 ecc : eccentricity
	 The units must be consistent with each other.
	 */
	 double  mh, h, fac, k, c, s;

	 a = fabs(a);
	 if (a < 1e-60) a = 1e-60;
	 k = gm / a;
	 if (k >= 0) k = sqrt(k);
	 else k = 0;    // just in case

	 mh = k * (t - t0) / a;
	 h = hypanom (mh, ecc);
	 fac = sqrt(ecc*ecc-1.0);
	 c = cosh(h);
	 s = sinh(h);
	 r1.assign (a*(ecc-c), a*fac*s, 0.0);
	 mh = ecc*c - 1.0;
	 v1.assign (-k*s/mh, k*fac*c/mh, 0.0);
 }

/*---------------- Function parab --------------------------------------*/

 void stumpff (double e2, double& c1, double& c2, double& c3)
  {
    /*
      Calculation of Stumpff functions c1=sin(e)/c,
      c2=(1-cos(e))/e^2 and c3=(e-sin(e))/e^3 for the
      argument e2=e^2  (e: eccentric anomaly)
     */
  const double  eps=1E-12;
  double  n, add;

   c1=0.0; c2=0.0; c3=0.0; add=1.0; n=1.0;
   do
    {
     c1=c1+add; add=add/(2.0*n);
     c2=c2+add; add=add/(2.0*n+1);
     c3=c3+add; add=-e2*add;
     n=n+1.0;
    } while (abs(add)>eps);
  }

void parab (double gm, double t0, double t, double q, double ecc,
                       Vec3& r1, Vec3& v1)
 {
  /*
    Calculate position r1 and velocity v1 of for parabolic
    (or near parabolic) orbit at time t using Stumpff's method.
    gm : Gravitational Constant
    t0 : time of perihelion passage
    q : perihelion distance
    ecc : eccentricity
    The units must be consistent with each other.
   */
  const double  eps = 1E-9;
  const int  maxit = 15;

  double e2, e20, fac, c1, c2, c3, k, tau, a, u, u2;
  double x, y;
  int i, fle;

  ecc = fabs(ecc);
  e2=0.0; fac=0.5*ecc; i=0;

  q = fabs(q);
  if (q < 1e-40) q = 1e-40;
  k = gm / (q*(1+ecc));

  if (k >= 0) k = sqrt(k);
  else k = 0;    // just in case

  tau = gm / (q*q*q);
  if (tau >= 0) tau= 1.5*sqrt(tau)*(t-t0);
  else tau = 0;

  do
   {
    i = i + 1;
    e20 = e2;
    if (fac < 0.0) a = -1.0;
    else a = tau * sqrt(fac);
    a = sqrt(a*a+1.0)+a;
    if (a > 0.0) a = exp(log(a)/3.0);
    if (a == 0.0) u = 0.0;
    else u = a - 1.0/a;
    u2 = u*u;
    if (fac != 0) e2 = u2*(1.0-ecc)/fac;
    else e2 = 1;
    stumpff (e2, c1, c2, c3);
    fac = 3.0*ecc*c3;
    fle = (abs(e2-e20) < eps) || (i > maxit);
   } while (!fle);

  if (fac != 0)
   {
    tau = q*(1.0+u2*c2*ecc/fac);
    x = q*(1.0-u2*c2/fac);
    y = (1.0+ecc)/fac;
    if (y >= 0) y = q*sqrt(y)*u*c1;
    else y = 0;
    r1.assign (x, y, 0.0);
    v1.assign (-k*y/tau, k*(x/tau+ecc), 0.0);
   }
  else  // just in case
   {
    r1.assign (0, 0, 0);
    v1.assign (0, 0, 0);
   };
 }

/*---------------- Function kepler --------------------------------------*/

void kepler (double gm, double t0, double t, double m0, double a, double ecc,
                        double ran, double aper, double inc, Vec3& r1, Vec3& v1)
 {
  /*
    Calculate position r1 and velocity v1 of body at time t as an
    undisturbed 2-body Kepler problem.

    gm : Gravitational Constant
    t0 : time of perihelion passage (hyperbolic or near-parabolic)
              epoch of m0 for elliptical orbits.
    m0 : Mean Anomaly at epoch for elliptical orbits in degrees in the
              range 0 to 360. If m0 < 0 the calculations will be done as
              near-parabolic. Set m0 = 0 for hyperbolic orbits.
    a : semi-major axis for elliptical (positive) or hyperbolic orbits
              (negative). If m0 < 0, a signifies perihelion distance q instead
    ecc : eccentricity
    ran : Right Ascension of Ascending Node (in degrees)
    aper : Argument of Perihelion (in degrees)
    inc : Inclination (in degrees)

    The units must be consistent with each other.

    NOTE : If ecc = 1 the orbit will always be calculated as a parabolic one.
           If ecc < 1 (elliptical orbits) two possibilities exist:
           If m0 >= 0 the calculation will be done in the classical
             way with m0 signifying the mean anomaly at time t0 and
             a the semi-major axis.
           If m0 < 0 the orbit will be calculated as a near-parabolic
             orbit with a signifying the perihelion distance at time t0.
             (This latter method will be useful for high eccentricity
              comet orbits for which typically q is given instead of a)
           If ecc > 1 (hyperbolic orbits) two possibilities exist:
           If m0 >= 0 (the value is ignored for hyperbolic orbits)
               a classical hyperbolic calculation is performed with a
               signifying the semi-major axis (negative for hyperbolas).
           If m0 < 0 the orbit will be calculated as a near-parabolic
               orbit with a signifying the perihelion distance. (This can
               be useful for comet orbits which would rarely have an
               eccentricity >> 1)
               In either case t0 signifies the time of perihelion passage.
   */
  const double  dgrd = M_PI / 180.0;
  enum {ell, par, hyp} kepc;
  Mat3 m1,m2;

  kepc = ell;  // just to keep the compiler happy

  // convert into radians
  m0 *= dgrd;
  ran *= dgrd;
  aper *= dgrd;
  inc *= dgrd;

  // calculate the position and velocity within the orbit plane
  if (ecc == 1.0) kepc = par;
  if (ecc < 1.0)
   {
    if (m0 < 0.0) kepc = par;
    else kepc = ell;
   }
  if (ecc > 1.0)
   {
    if (m0 < 0.0) kepc = par;
    else kepc = hyp;
   }

  switch (kepc)
   {
    case ell : ellip (gm, t0, t, a, ecc, m0, r1, v1); break;
    case par : parab (gm, t0, t, a, ecc, r1, v1); break;
    case hyp : hyperb (gm, t0, t, a, ecc, r1, v1); break;
   }

  // convert into reference plane
  m1 = zrot (-aper);
  m2 = xrot (-inc);
  m1 *= m2;
  m2 = zrot (-ran);
  m2 = m2 * m1;

  r1 = mxvct (m2, r1);
  v1 = mxvct (m2, v1);
 }

/*---------------- Function oscelm --------------------------------------*/

void oscelm (double gm, double t, Vec3& r1, Vec3& v1,
             double& t0, double& m0, double& a, double& ecc,
             double& ran, double& aper, double& inc)
 {
  /*
	 Get the osculating Kepler elements at epoch t from position r1 and
	 velocity v1 of body.

	 gm : Gravitational Constant. If set negative, its absolute value will
			be taken as gm and the perihelion distance will be returned
			instead of the semimajor axis.
	 t0 : time of perihelion passage
	 m0 : Mean Anomaly at epoch for elliptical orbits in degrees in the
			range 0 to 360. Set m0 = 0 for hyperbolic orbits.
			m0 will be set to -1 if perihelion distance is to be returned
			instead of a.
	 a : semi-major axis for elliptical (positive) or hyperbolic orbits
		  (negative). If m0 < 0, a signifies perihelion distance q instead.
		  If gm is negative, the perihelion distance q will be returned
	 ecc : eccentricity
	 ran : Right Ascension of Ascending Node (in degrees)
	 aper : Argument of Perihelion (in degrees)
	 inc : Inclination (in degrees)

	 The units must be consistent with each other.
	*/
	 const double rddg = 180.0/M_PI;
	 const double p2 = 2.0*M_PI;

	 Vec3 c;
	 double cabs, u, cu, su, p, r;
	 int pflg;  // 1, if perihelion distance to be returned

		pflg = 0;
		if (gm < 0.0)
		 {
		  gm = -gm;
		  pflg = 1;
		 };

		if (gm < 1e-60) gm = 1e-60;
		c = r1 * v1;
		cabs = abs(c);
		if (fabs(cabs) < 1e-40) cabs = 1e-40;
		ran = atan20 (c[0], -c[1]);
		inc = c[2]/cabs;
		if (fabs(inc) <= 1.0) inc = acos (inc);
		else inc = 0;

		r = abs(r1);
		if (fabs(r) < 1e-40) r = 1e-40;
		su = sin(inc);
		if (su != 0.0) su = r1[2]/su;
		cu = r1[0]*cos(ran)+r1[1]*sin(ran);
		u = atan20 (su, cu);   // argument of latitude

		a = abs(v1);
		a = 2.0/r - a*a/gm;
		if (fabs(a) < 1.0E-30) ecc = 1.0;  // parabolic
		else
		 {
		  a = 1.0/a;         // semimajor axis
		  ecc = 0;
		 };

		p = cabs*cabs/gm;  // semilatum rectum

		if (ecc == 1.0)
		 {
			p = p / 2.0;  // perihelion distance
			a = 2*p;
		  }
		else
		 {
		  ecc = 1.0 - p/a;
		  if (ecc >= 0) ecc = sqrt(ecc);
		  else ecc = 0;
		  p = p / (1.0 + ecc);  // perihelion distance
		 }

		if (fabs(a) > 1e-60) cu = (1.0 - r/a);
		else cu = 0;   // just in case
		su = dot(r1,v1) / sqrt(fabs(a)*gm);
		if (ecc < 1.0)
		 {
		  m0 = atan20(su,cu);  // eccentric anomaly
		  su = sin(m0);
		  cu = cos(m0);
		  aper = 1.0-ecc*ecc;
		  if (aper >= 0) aper = atan20(sqrt(aper)*su,(cu-ecc)); // true anomaly
		  m0 = m0 - ecc*su;   // mean anomaly
		 }
		else if (ecc > 1.0)
		 {
		  su = su / ecc;
		  m0 = su + sqrt(su*su + 1.0);
		  if (m0 >= 0) m0 = log(m0);  // = asinh (su); hyperbolic anomaly
		  aper = (ecc+1.0)/(ecc-1.0);
		  if (aper >= 0) aper = 2.0 * atan(sqrt(aper)*tanh(m0/2.0));
		  m0 = ecc*su-m0;
		 }

		if (ecc != 1.0)
		 {
		  aper = u - aper;    // argument of perihelion
		  u = fabs(a);
		  t0 = u/gm;
		  if (t0 >= 0) t0 = t - u*sqrt(t0)*m0;  // time of perihelion transit
		  else t0 = t;
		 }
		else     // parabolic
		 {
		  pflg = 1;

		  aper = 2.0 * atan(su);   // true anomaly
		  aper = u - aper;    // argument of perihelion
		  t0 = 2.0*p*p*p/gm;
		  if (t0 >= 0) t0 = t - sqrt(t0) * (su*su*su/3.0 + su);
		  else t0 = t;
		 }

		if (m0 < 0.0) m0 = m0 + p2;
		if (ran < 0.0) ran = ran + p2;
		if (aper < 0.0) aper = aper + p2;
		m0 = rddg * m0;
		ran = rddg * ran;
		aper = rddg * aper;
		inc = rddg * inc;

		if (ecc > 1.0) m0 = 0.0;
		if (pflg)
		 {
			 a = p;
			 m0 = -1.0;
		 }
 }

/*-------------------- QuickSun --------------------------------------*/

Vec3 QuickSun (double t)   // low precision position of the Sun at time t
 {
  /* Low precision position of the Sun at time t given in
	  Julian centuries since J2000.
	  Valid only between 1950 and 2050.

	  Returns the position vector (in A.U.) in ecliptic coordinates

  */
  double n, g, l;
  Vec3 rs;

  n = 36525.0 * t;
  l = 280.460 + 0.9856474*n;   // mean longitude
  g = 357.528 + 0.9856003*n;   // mean anomaly
  l = 2.0*M_PI * frac(l/360.0);
  g = 2.0*M_PI * frac(g/360.0);
  l = l + (1.915*sin(g) + 0.020*sin(2.0*g)) * 1.74532925199e-2; // ecl.long.
  g = 1.00014 - 0.01671*cos(g) - 0.00014*cos(2.0*g);  // radius
  rs[0] = g * cos(l);
  rs[1] = g * sin(l);
  rs[2] = 0;

  return rs;
 }

/*-------------------- Class Sun200 --------------------------------------*/
 /*
	 Ecliptic coordinates (in A.U.) and velocity (in A.U./day)
	 of the sun for Equinox of Date given in Julian centuries since J2000.
	 ======================================================================
  */
Sun200::Sun200 ()
  { }

Vec3 Sun200::position (double t)   // position of the Sun at time t
 {
  Vec3 rs, vs;

  state (t, rs, vs);

  return rs;
 }

void Sun200::state (double t, Vec3& rs, Vec3& vs)
 {
  /* State vector rs (position) and vs (velocity) of the Sun in
	  ecliptic of date coordinates at time t (in Julian Centruries
	  since J2000).
	 */
	 const double  p2 = 2.0 * M_PI;

	 double l, b, r;
	 int	i;

	 tt = t;

	 dl = 0.0; dr = 0.0; db = 0.0;
	 m2 = p2 * frac(0.1387306 + 162.5485917*t);
	 m3 = p2 * frac(0.9931266 + 99.9973604*t);
	 m4 = p2 * frac(0.0543250 + 53.1666028*t);
	 m5 = p2 * frac(0.0551750 + 8.4293972*t);
	 m6 = p2 * frac(0.8816500 + 3.3938722*t);
	 d = p2 * frac(0.8274 + 1236.8531*t);
	 a= p2 * frac(0.3749 + 1325.5524*t);
	 uu = p2 * frac(0.2591 + 1342.2278*t);

	 c3[1] = 1.0; s3[1] = 0.0;
	 c3[2] = cos(m3); s3[2] = sin(m3);
	 c3[0] = c3[2]; s3[0] = -s3[2];

	 for (i=3; i<9; ++i)
		addthe(c3[i-1],s3[i-1],c3[2],s3[2],c3[i],s3[i]);
	 pertven(); pertmar(); pertjup(); pertsat(); pertmoo();

	 dl = dl + 6.4 * sin(p2*(0.6983 + 0.0561*t))
				 + 1.87 * sin(p2*(0.5764 + 0.4174*t))
				 + 0.27 * sin(p2*(0.4189 + 0.3306*t))
				 + 0.20 * sin(p2*(0.3581 + 2.4814*t));
	 l = p2 * frac(0.7859453 + m3/p2 +((6191.2+1.1*t)*t+dl)/1296.0E3);
	 r = 1.0001398 - 0.0000007*t + dr*1E-6;
	 b = db * 4.8481368111E-6;

	 cl= cos(l); sl=sin(l); cb=cos(b); sb=sin(b);
	 rs[0]=r*cl*cb; rs[1]=r*sl*cb; rs[2]=r*sb;

  /* velocity calculation
		 gms=2.9591202986e-4 AU^3/d^2
		 e=0.0167086
		 sqrt(gms/a)=1.7202085e-2 AU/d
		 sqrt(gms/a)*sqrt(1-e^2)=1.71996836e-2 AU/d
		 nu=M + 2e*sin(M) = M + 0.0334172 * sin(M)  */

	 uu = m3 + 0.0334172*sin(m3);  // eccentric Anomaly E
	 d = cos(uu);
	 uu = sin(uu);
	 a = 1.0 - 0.0167086*d;
	 vs[0] = -1.7202085e-2*uu/a;   // velocity in orbit plane
	 vs[1] = 1.71996836e-2*d/a;
	 uu = atan2 (0.9998604*uu, (d-0.0167086));  // true anomaly
	 d = cos(uu);
	 uu = sin(uu);
	 dr = d*vs[0]+uu*vs[1];
	 dl = (d*vs[1]-uu*vs[0]) / r;

	 vs[0] = dr*cl*cb - dl*r*sl*cb;
	 vs[1] = dr*sl*cb + dl*r*cl*cb;
	 vs[2] = dr*sb;
 }

void Sun200::addthe (double c1, double s1, double c2, double s2,
							double& cc, double& ss)
 {
	cc=c1*c2-s1*s2;
	ss=s1*c2+c1*s2;
 }

void Sun200::term (int i1, int i, int it, double dlc, double dls, double drc,
			  double drs, double dbc, double dbs)
 {
	 if (it == 0) addthe (c3[i1+1],s3[i1+1],c[i+8],s[i+8],u,v);
	 else
	  {
		u=u*tt;
		v=v*tt;
	  }
	 dl = dl + dlc*u + dls*v;
	 dr = dr + drc*u + drs*v;
	 db = db + dbc*u + dbs*v;
 }

void Sun200::pertven()   // Kepler terms and perturbations by Venus
 {
	int i;

	  c[8]=1.0; s[8]=0.0; c[7]=cos(m2); s[7]=-sin(m2);
	  for (i=7; i>2; i--)
		 addthe(c[i],s[i],c[7],s[7],c[i-1],s[i-1]);

	  term (1, 0, 0, -0.22, 6892.76, -16707.37, -0.54, 0.0, 0.0);
	  term (1, 0, 1, -0.06, -17.35, 42.04, -0.15, 0.0, 0.0);
	  term (1, 0, 2, -0.01, -0.05, 0.13, -0.02, 0.0, 0.0);
	  term (2, 0, 0, 0.0, 71.98, -139.57, 0.0, 0.0, 0.0);
	  term (2, 0, 1, 0.0, -0.36, 0.7, 0.0, 0.0, 0.0);
	  term (3, 0, 0, 0.0, 1.04, -1.75, 0.0, 0.0, 0.0);
	  term (0, -1, 0, 0.03, -0.07, -0.16, -0.07, 0.02, -0.02);
	  term (1, -1, 0, 2.35, -4.23, -4.75, -2.64, 0.0, 0.0);
	  term (1, -2, 0, -0.1, 0.06, 0.12, 0.2, 0.02, 0.0);
	  term (2, -1, 0, -0.06, -0.03, 0.2, -0.01, 0.01, -0.09);
	  term (2, -2, 0, -4.7, 2.9, 8.28, 13.42, 0.01, -0.01);
	  term (3, -2, 0, 1.8, -1.74, -1.44, -1.57, 0.04, -0.06);
	  term (3, -3, 0, -0.67, 0.03, 0.11, 2.43, 0.01, 0.0);
	  term (4, -2, 0, 0.03, -0.03, 0.1, 0.09, 0.01, -0.01);
	  term (4, -3, 0, 1.51, -0.4, -0.88, -3.36, 0.18, -0.1);
	  term (4, -4, 0, -0.19, -0.09, -0.38, 0.77, 0.0, 0.0);
	  term (5, -3, 0, 0.76, -0.68, 0.3, 0.37, 0.01, 0.0);
	  term (5, -4, 0, -0.14, -0.04, -0.11, 0.43, -0.03, 0.0);
	  term (5, -5, 0, -0.05, -0.07, -0.31, 0.21, 0.0, 0.0);
	  term (6, -4, 0, 0.15, -0.04, -0.06, -0.21, 0.01, 0.0);
	  term (6, -5, 0, -0.03, -0.03, -0.09, 0.09, -0.01, 0.0);
	  term (6, -6, 0, 0.0, -0.04, -0.18, 0.02, 0.0, 0.0);
	  term (7, -5, 0, -0.12, -0.03, -0.08, 0.31, -0.02, -0.01);
 }

void Sun200::pertmar()    // Kepler terms and perturbations by Mars
 {
	int i;

	  c[7] = cos(m4); s[7] = -sin(m4);
	  for (i=7; i>0; i--)
		  addthe(c[i],s[i],c[7],s[7],c[i-1],s[i-1]);
	  term (1, -1, 0, -0.22, 0.17, -0.21, -0.27, 0.0, 0.0);
	  term (1, -2, 0, -1.66, 0.62, 0.16, 0.28, 0.0, 0.0);
	  term (2, -2, 0, 1.96, 0.57, -1.32, 4.55, 0.0, 0.01);
	  term (2, -3, 0, 0.4, 0.15, -0.17, 0.46, 0.0, 0.0);
	  term (2, -4, 0, 0.53, 0.26, 0.09, -0.22, 0.0, 0.0);
	  term (3, -3, 0, 0.05, 0.12, -0.35, 0.15, 0.0, 0.0);
	  term (3, -4, 0, -0.13, -0.48, 1.06, -0.29, 0.01, 0.0);
	  term (3, -5, 0, -0.04, -0.2, 0.2, -0.04, 0.0, 0.0);
	  term (4, -4, 0, 0.0, -0.03, 0.1, 0.04, 0.0, 0.0);
	  term (4, -5, 0, 0.05, -0.07, 0.2, 0.14, 0.0, 00);
	  term (4, -6, 0, -0.1, 0.11, -0.23, -0.22, 0.0, 0.0);
	  term (5, -7, 0, -0.05, 0.0, 0.01, -0.14,  0.0, 0.0);
	  term (5, -8, 0, 0.05, 0.01, -0.02, 0.1, 0.0, 0.0);
 }

void Sun200::pertjup()    // Kepler terms and perturbations by Jupiter
 {
	int i;

	  c[7] = cos(m5); s[7] = -sin(m5);
	  for (i=7; i>4; i--)
		  addthe(c[i],s[i],c[7],s[7],c[i-1],s[i-1]);
	  term (1, -1, 0, 0.01, 0.07, 0.18, -0.02, 0.0, -0.02);
	  term (0, -1, 0, -0.31, 2.58, 0.52, 0.34, 0.02, 0.0);
	  term (1, -1, 0, -7.21, -0.06, 0.13, -16.27, 0.0, -0.02);
	  term (1, -2, 0, -0.54, -1.52, 3.09, -1.12, 0.01, -0.17);
	  term (1, -3, 0, -0.03, -0.21, 0.38, -0.06, 0.0, -0.02);
	  term (2, -1, 0, -0.16, 0.05, -0.18, -0.31, 0.01, 0.0);
	  term (2, -2, 0, 0.14, -2.73, 9.23, 0.48, 0.0, 0.0);
	  term (2, -3, 0, 0.07, -0.55, 1.83, 0.25, 0.01,  0.0);
	  term (2, -4, 0, 0.02, -0.08, 0.25, 0.06, 0.0, 0.0);
	  term (3, -2, 0, 0.01, -0.07, 0.16, 0.04, 0.0, 0.0);
	  term (3, -3, 0, -0.16, -0.03, 0.08, -0.64, 0.0, 0.0);
	  term (3, -4, 0, -0.04, -0.01, 0.03, -0.17, 0.0, 0.0);
  }

void Sun200::pertsat()  // Kepler terms and perturbations by Saturn
 {
  c[7] = cos(m6); s[7] = -sin(m6);
  addthe(c[7],s[7],c[7],s[7],c[6],s[6]);
  term (0, -1, 0, 0.0, 0.32, 0.01, 0.0, 0.0, 0.0);
  term (1, -1, 0, -0.08, -0.41, 0.97, -0.18, 0.0, -0.01);
  term (1, -2, 0, 0.04, 0.1, -0.23, 0.1, 0.0, 0.0);
  term (2, -2, 0, 0.04, 0.1, -0.35, 0.13, 0.0, 0.0);
 }

void Sun200::pertmoo()   // corrections for Earth-Moon center of gravity
 {
  dl = dl + 6.45*sin(d) - 0.42*sin(d-a) + 0.18*sin(d+a) + 0.17*sin(d-m3) - 0.06*sin(d+m3);
  dr = dr + 30.76*cos(d) - 3.06*cos(d-a) + 0.85*cos(d+a) - 0.58*cos(d+m3) + 0.57*cos(d-m3);
  db = db + 0.576*sin(uu);
 }

/*--------------------- Class moon200 ----------------------------------*/

	/*
	 Position vector (in Earth radii) of the Moon referred to Ecliptic
	 for Equinox of Date.
	 t is the time in Julian centuries since J2000.
	*/

Moon200::Moon200 ()
  { }

Vec3 Moon200::position (double t)   // position of the Moon at time t
 {
  Vec3 rm;

	 const double arc = 206264.81;    // 3600*180/pi = ''/r
	 const double pi2 = M_PI * 2.0;

	 double lambda, beta, r, fac;

	minit(t);
	solar1(); solar2(); solar3(); solarn(n); planetary(t);

	lambda =  pi2 * frac ((l0+dlam/arc) / pi2);
	if (lambda < 0) lambda = lambda + pi2;
	s = f + ds / arc;

	fac = 1.000002708 + 139.978 * dgam;
	beta = (fac*(18518.511+1.189+gam1c)*sin(s)-6.24*sin(3*s)+n);
	beta = beta * 4.8481368111e-6;
	sinpi = sinpi * 0.999953253;
	r = arc / sinpi;

	rm.assign (r, lambda, beta);
	rm = polcar(rm);

        return rm;
 }


void Moon200::addthe (double c1, double s1, double c2, double s2,
							 double& c, double& s)
 {
	c=c1*c2-s1*s2;
	s=s1*c2+c1*s2;
 }

double Moon200::sinus (double phi)
 {
	 /* sin(phi) in units of 1r = 360ø */
	return sin (2.0*M_PI * frac(phi));
 }

void Moon200::long_periodic (double t)
 {
   /* long periodic changes of mean elements */

  double  s1, s2, s3, s4, s5, s6, s7;

  s1 = sinus (0.19833 + 0.05611*t);
  s2 = sinus (0.27869 + 0.04508*t);
  s3 = sinus (0.16827 - 0.36903*t);
  s4 = sinus (0.34734 - 5.37261*t);
  s5 = sinus (0.10498 - 5.37899*t);
  s6 = sinus (0.42681 - 0.41855*t);
  s7 = sinus (0.14943 - 5.37511*t);
  dl0 = 0.84*s1 + 0.31*s2 + 14.27*s3 + 7.26*s4 + 0.28*s5 + 0.24*s6;
  dl = 2.94*s1 + 0.31*s2 + 14.27*s3 + 9.34*s4 + 1.12*s5 + 0.83*s6;
  dls = -6.4*s1 - 1.89*s6;
  df = 0.21*s1+0.31*s2+14.27*s3-88.7*s4-15.3*s5+0.24*s6-1.86*s7;
  dd = dl0 - dls;
  dgam = -3332E-9 * sinus (0.59734 - 5.37261*t)
                    -539E-9 * sinus (0.35498 - 5.37899*t)
                    -64E-9 * sinus (0.39943 - 5.37511*t);
 }

void Moon200::minit(double t)
 {
  /* calculate mean elements l (moon), F (node distance)
		l' (sun) and D (moon's elongation) */

  const double arc = 206264.81;    // 3600*180/pi = ''/r
  const double pi2 = M_PI * 2.0;

  int i, j, max;
  double t2, arg, fac;

  max = 0; // just to keep the compiler happy
  arg = 0;
  fac = 0;

  t2 = t * t;
  dlam = 0; ds = 0; gam1c = 0; sinpi = 3422.7;
  long_periodic (t);
  l0 = pi2*frac(0.60643382+1336.85522467*t-0.00000313*t2) + dl0/arc;
  l = pi2*frac(0.37489701+1325.55240982*t+0.00002565*t2)  + dl/arc;
  ls = pi2*frac(0.99312619+99.99735956*t-0.00000044*t2)   + dls/arc;
  f = pi2*frac(0.25909118+1342.22782980*t-0.00000892*t2)  + df/arc;
  d = pi2*frac(0.82736186+1236.85308708*t-0.00000397*t2)  + dd/arc;
  for (i=0; i<4; ++i)
   {
    switch (i)
     {
      case 0: arg=l; max=4; fac=1.000002208; break;
      case 1: arg=ls; max=3; fac=0.997504612-0.002495388*t; break;
      case 2: arg=f; max=4; fac=1.000002708+139.978*dgam; break;
      case 3: arg=d; max=6; fac=1.0; break;
     }
    co[6][i] = 1.0; co[7][i] = cos (arg) * fac;
    si[6][i] = 0.0; si[7][i] = sin (arg) * fac;
    for (j = 2; j <= max; ++j)
           addthe(co[j+5][i],si[j+5][i],co[7][i],si[7][i],co[j+6][i],si[j+6][i]);
    for (j = 1; j <= max; ++j)
     {
      co[6-j][i]=co[j+6][i];
      si[6-j][i]=-si[j+6][i];
     }
   }
 }

void Moon200::term (int p, int q, int r, int s, double& x, double& y)
 {
  // calculate x=cos(p*arg1+q*arg2...); y=sin(p*arg1+q*arg2...)
  int i[4];
  int k;

  i[0]=p; i[1]=q; i[2]=r; i[3]=s; x=1.0; y=0.0;
  for (k=0; k<4; k++)
   if (i[k]!=0) addthe(x,y,co[i[k]+6][k],si[i[k]+6][k],x,y);
 }

void Moon200::addsol(double coeffl, double coeffs, double coeffg,
                     double coeffp, int p, int q, int r, int s)
 {
  double x, y;
  term (p,q,r,s,x,y);
  dlam = dlam + coeffl*y; ds = ds + coeffs*y;
  gam1c = gam1c + coeffg*x; sinpi = sinpi + coeffp*x;
 }

void Moon200::solar1()
 {
	  addsol ( 13.902, 14.06, -0.001, 0.2607, 0, 0, 0, 4);
	  addsol ( 0.403, -4.01, 0.394, 0.0023, 0, 0, 0, 3);
	  addsol ( 2369.912, 2373.36, 0.601, 28.2333, 0, 0, 0, 2);
	  addsol ( -125.154, -112.79, -0.725, -0.9781, 0, 0, 0, 1);
	  addsol ( 1.979, 6.98, -0.445, 0.0433, 1, 0, 0, 4);
	  addsol (191.953, 192.72, 0.029, 3.0861, 1, 0, 0, 2);
	  addsol (-8.466, -13.51, 0.455, -0.1093, 1, 0, 0, 1);
	  addsol (22639.5, 22609.07, 0.079, 186.5398, 1, 0, 0, 0);
	  addsol (18.609, 3.59, -0.094, 0.0118, 1, 0, 0, -1);
	  addsol (-4586.465, -4578.13, -0.077, 34.3117, 1, 0, 0, -2);
	  addsol (3.215, 5.44, 0.192, -0.0386, 1, 0, 0, -3);
	  addsol (-38.428, -38.64, 0.001, 0.6008, 1, 0, 0, -4);
	  addsol (-0.393, -1.43, -0.092, 0.0086, 1, 0, 0, -6);
	  addsol (-0.289, -1.59, 0.123, -0.0053, 0, 1, 0, 4);
	  addsol (-24.420, -25.1, 0.04, -0.3, 0, 1, 0, 2);
	  addsol (18.023, 17.93, 0.007, 0.1494, 0, 1, 0, 1);
	  addsol (-668.146, -126.98, -1.302, -0.3997, 0, 1, 0, 0);
	  addsol (0.56, 0.32, -0.001, -0.0037, 0, 1, 0, -1);
	  addsol (-165.145, -165.06, 0.054, 1.9178, 0, 1, 0, -2);
	  addsol (-1.877, -6.46, -0.416, 0.0339, 0, 1, 0, -4);
	  addsol (0.213, 1.02, -0.074, 0.0054, 2, 0, 0, 4);
	  addsol (14.387, 14.78, -0.017, 0.2833, 2, 0, 0, 2);
	  addsol (-0.586, -1.2, 0.054, -0.01, 2, 0, 0, 1);
	  addsol (769.016, 767.96, 0.107, 10.1657, 2, 0, 0, 0);
	  addsol (1.75, 2.01, -0.018, 0.0155, 2, 0, 0, -1);
	  addsol (-211.656, -152.53, 5.679, -0.3039, 2, 0, 0, -2);
	  addsol (1.225, 0.91, -0.03, -0.0088, 2, 0, 0, -3);
	  addsol (-30.773, -34.07, -0.308, 0.3722, 2, 0, 0, -4);
	  addsol (-0.57, -1.4, -0.074, 0.0109, 2, 0, 0, -6);
	  addsol (-2.921, -11.75, 0.787, -0.0484, 1, 1, 0, 2);
	  addsol (1.267, 1.52, -0.022, 0.0164, 1, 1, 0, 1);
	  addsol (-109.673, -115.18, 0.461, -0.949, 1, 1, 0, 0);
	  addsol (-205.962, -182.36, 2.056, 1.4437, 1, 1, 0, -2);
	  addsol (0.233, 0.36, 0.012, -0.0025, 1, 1, 0, -3);
	  addsol (-4.391, -9.66, -0.471, 0.0673, 1, 1, 0, -4);
 }

void Moon200::solar2()
 {
	  addsol (0.283, 1.53, -0.111, 0.006, 1, -1, 0, 4);
	  addsol (14.577, 31.7, -1.54, 0.2302, 1, -1, 0, 2);
	  addsol (147.687, 138.76, 0.679, 1.1528, 1, -1, 0, 0);
	  addsol (-1.089, 0.55, 0.021, 0.0, 1, -1, 0, -1);
	  addsol (28.475, 23.59, -0.443, -0.2257, 1, -1, 0, -2);
	  addsol (-0.276, -0.38, -0.006, -0.0036, 1, -1, 0, -3);
	  addsol (0.636, 2.27, 0.146, -0.0102, 1, -1, 0, -4);
	  addsol (-0.189, -1.68, 0.131, -0.0028, 0, 2, 0, 2);
	  addsol (-7.486, -0.66, -0.037, -0.0086, 0, 2, 0, 0);
	  addsol (-8.096, -16.35, -0.74, 0.0918, 0, 2, 0, -2);
	  addsol (-5.741, -0.04, 0.0, -0.0009, 0, 0, 2, 2);
	  addsol (0.255, 0.0, 0.0, 0.0, 0, 0, 2, 1);
	  addsol (-411.608, -0.2, 0.0, -0.0124, 0, 0, 2, 0);
	  addsol (0.584, 0.84, 0.0, 0.0071, 0, 0, 2, -1);
	  addsol (-55.173, -52.14, 0.0, -0.1052, 0, 0, 2, -2);
	  addsol (0.254, 0.25, 0.0, -0.0017, 0, 0, 2, -3);
	  addsol (0.025, -1.67, 0.0, 0.0031, 0, 0, 2, -4);
	  addsol (1.06, 2.96, -0.166, 0.0243, 3, 0, 0, 2);
	  addsol (36.124, 50.64, -1.3, 0.6215, 3, 0, 0, 0);
	  addsol (-13.193, -16.4, 0.258, -0.1187, 3, 0, 0, -2);
	  addsol (-1.187, -0.74, 0.042, 0.0074, 3, 0, 0, -4);
	  addsol (-0.293, -0.31, -0.002, 0.0046, 3, 0, 0, -6);
	  addsol (-0.29, -1.45, 0.116, -0.0051, 2, 1, 0, 2);
	  addsol (-7.649, -10.56, 0.259, -0.1038, 2, 1, 0, 0);
	  addsol (-8.627, -7.59, 0.078, -0.0192, 2, 1, 0, -2);
	  addsol (-2.74, -2.54, 0.022, 0.0324, 2, 1, 0, -4);
	  addsol (1.181, 3.32, -0.212, 0.0213, 2, -1, 0, 2);
	  addsol (9.703, 11.67, -0.151, 0.1268, 2, -1, 0, 0);
	  addsol (-0.352, -0.37, 0.001, -0.0028, 2, -1, 0, -1);
	  addsol (-2.494, -1.17, -0.003, -0.0017, 2, -1, 0, -2);
	  addsol (0.36, 0.2, -0.012, -0.0043, 2, -1, 0, -4);
	  addsol (-1.167, -1.25, 0.008, -0.0106, 1, 2, 0, 0);
	  addsol (-7.412, -6.12, 0.117, 0.0484, 1, 2, 0, -2);
	  addsol (-0.311, -0.65, -0.032, 0.0044, 1, 2, 0, -4);
	  addsol (0.757, 1.82, -0.105, 0.0112, 1, -2, 0, 2);
	  addsol (2.58, 2.32, 0.027, 0.0196, 1, -2, 0, 0);
	  addsol (2.533, 2.4, -0.014, -0.0212, 1, -2, 0, -2);
	  addsol (-0.344, -0.57, -0.025, 0.0036, 0, 3, 0, -2);
	  addsol (-0.992, -0.02, 0.0, 0.0, 1, 0, 2, 2);
	  addsol (-45.099, -0.02, 0.0, -0.0010, 1, 0, 2, 0);
	  addsol (-0.179, -9.52, 0.0, -0.0833, 1, 0, 2, -2);
	  addsol (-0.301, -0.33, 0.0, 0.0014, 1, 0, 2, -4);
	  addsol (-6.382, -3.37, 0.0, -0.0481, 1, 0, -2, 2);
	  addsol (39.528, 85.13, 0.0, -0.7136, 1, 0, -2, 0);
	  addsol (9.366, 0.71, 0.0, -0.0112, 1, 0, -2, -2);
	  addsol (0.202, 0.02, 0.0, 0.0, 1, 0, -2, -4);
 }

void Moon200::solar3()
 {
	  addsol (0.415, 0.1, 0.0, 0.0013, 0, 1, 2, 0);
	  addsol (-2.152, -2.26, 0.0, -0.0066, 0, 1, 2, -2);
	  addsol (-1.44, -1.3, 0.0, 0.0014, 0, 1, -2, 2);
	  addsol (0.384, -0.04, 0.0, 0.0, 0, 1, -2, -2);
	  addsol (1.938, 3.6, -0.145, 0.0401, 4, 0, 0, 0);
	  addsol (-0.952, -1.58, 0.052, -0.0130, 4, 0, 0, -2);
	  addsol (-0.551, -0.94, 0.032, -0.0097, 3, 1, 0, 0);
	  addsol (-0.482, -0.57, 0.005, -0.0045, 3, 1, 0, -2);
	  addsol (0.681, 0.96, -0.026, 0.0115, 3, -1, 0, 0);
	  addsol (-0.297, -0.27, 0.002, -0.0009, 2, 2, 0, -2);
	  addsol (0.254, 0.21, -0.003, 0.0, 2, -2, 0, -2);
	  addsol (-0.25, -0.22, 0.004, 0.0014, 1, 3, 0, -2);
	  addsol (-3.996, 0.0, 0.0, 0.0004, 2, 0, 2, 0);
	  addsol (0.557, -0.75, 0.0, -0.009, 2, 0, 2, -2);
	  addsol (-0.459, -0.38, 0.0, -0.0053, 2, 0, -2, 2);
	  addsol (-1.298, 0.74, 0.0, 0.0004, 2, 0, -2, 0);
	  addsol (0.538, 1.14, 0.0, -0.0141, 2, 0, -2, -2);
	  addsol (0.263, 0.02, 0.0, 0.0, 1, 1, 2, 0);
	  addsol (0.426, 0.07, 0.0, -0.0006, 1, 1, -2, -2);
	  addsol (-0.304, 0.03, 0.0, 0.0003, 1, -1, 2, 0);
	  addsol (-0.372, -0.19, 0.0, -0.0027, 1, -1, -2, 2);
	  addsol (0.418, 0.0, 0.0, 0.0, 0, 0, 4, 0);
	  addsol (-0.330, -0.04, 0.0, 0.0, 3, 0, 2, 0);
 }

void Moon200::addn (double coeffn, int p, int q, int r, int s,
                    double& n, double&x, double& y)
 {
  term (p,q,r,s,x,y);
  n=n+coeffn*y;
 }

void Moon200::solarn (double& n)
 {
  // perturbation N of ecliptic latitude
  double x, y;

  n = 0.0;
  addn (-526.069, 0, 0, 1, -2, n, x, y);
  addn (-3.352, 0, 0, 1, -4, n, x, y);
  addn (44.297, 1, 0, 1, -2,  n, x, y);
  addn (-6.0, 1, 0, 1, -4, n, x, y);
  addn (20.599, -1, 0, 1, 0, n, x, y);
  addn (-30.598, -1, 0, 1, -2, n, x, y);
  addn (-24.649, -2, 0, 1, 0, n, x, y);
  addn (-2.0, -2, 0, 1, -2, n, x, y);
  addn (-22.571, 0, 1, 1, -2, n, x, y);
  addn (10.985, 0, -1, 1, -2, n, x, y);
 }

void Moon200::planetary (double t)
 {
  // perturbations of the ecliptic longitude by Venus and Jupiter

  dlam = dlam
            + 0.82*sinus(0.7736 -62.5512*t)+0.31*sinus(0.0466-125.1025*t)
            + 0.35*sinus(0.5785-25.1042*t)+0.66*sinus(0.4591+1335.8075*t)
            + 0.64*sinus(0.313-91.568*t)+1.14*sinus(0.148+1331.2898*t)
            + 0.21*sinus(0.5918+1056.5859*t)+0.44*sinus(0.5784+1322.8595*t)
            + 0.24*sinus(0.2275-5.7374*t)+0.28*sinus(0.2965+2.6929*t)
            + 0.33*sinus(0.3132+6.3368*t);
 }

/*-------------------- Class Eclipse --------------------------------------*/
/*
	Calculalations for Solar and Lunar Eclipses
  */

Eclipse::Eclipse()
 {
  // some assignments just in case
  rs.assign(1,0,0);
  rm.assign(1,0,0);
  eshadow.assign (1,0,0);
  rint.assign (1,0,0);
  d_umbra = 0;
  ep2 = 0;
  }

int Eclipse::solar (double jd, double tdut, double& phi, double& lamda)
 {
  /* Calculates various items about a solar eclipse.
	  INPUT:
	  jd = Modified Julian Date (UT)
	  tdut = TDT - UT in sec

	  OUTPUT:
	  phi, lamda: Geographic latitude and longitude (in radians)
					  of center of shadow if central eclipse

	  RETURN:
	  0 : No eclipse
	  1 : Partial eclipse
	  2 : Non-central annular eclipse
	  3 : Non-central total eclipse
	  4 : Annular eclipse
	  5 : Total eclipse
	 */

	const double flat = 0.996633;  // flatting of the Earth
	const double ds = 218.245445;  // diameter of Sun in Earth radii
	const double dm = 0.544986;   // diameter of Moon in Earth radii
	double s0, s, dlt, r2, r0;
	int phase;
	Vec3 ve;

	// get the apparent equatorial coordinates of the sun and the moon
	equ_sun_moon(jd, tdut);

	rs[2]/=flat;  // adjust for flatting of the Earth
	rm[2]/=flat;
	rint.assign ();
	lamda = 0;
	phi = 0;

	// intersect shadow axis with Earth
	eshadow = rm - rs;
	eshadow = vnorm(eshadow);    // direction vector of shadow

	s0 = - dot(rm, eshadow);   // distance Moon - fundamental plane
	r2 = dot (rm,rm);
	dlt = s0*s0 + 1.0 - r2;
	r0 = 1.0 - dlt;
	if (r0 > 0) r0 = sqrt (r0);
	else r0 = 0;      // distance center of Earth - shadow axis

	r2 = abs(rs - rm);
	d_umbra = (ds - dm) * s0 / r2 - dm;// diameter of umbra at fundamental plane
	d_penumbra = (ds + dm) * s0 / r2 + dm;

	// get phase of eclipse
	if (r0 < 1.0)
	 {
	  if (dlt > 0) dlt = sqrt(dlt);
	  else dlt = 0;
	  s = s0 - dlt;  // distance Moon - fundamental plane
	  d_umbra = (ds - dm) * s / r2 - dm; // diameter of umbra at surface
	  rint = rm + s * eshadow;
	  rint[2] *= flat;    // vector to intersection
	  ve = carpol(rint);
	  lamda = ve[1] - lsidtim(jd,0,ep2)*0.261799387799; // geographic coordinates
	  if (lamda > M_PI) lamda -= 2.0*M_PI;
	  if (lamda < (-M_PI)) lamda += 2.0*M_PI;
	  phi = sqrt(rint[0]*rint[0] + rint[1]*rint[1])*0.993305615;
	  phi = atan2(rint[2],phi);

	  if (d_umbra > 0) phase = 4;  // central annular eclipse
	  else phase = 5;              // central total eclipse
	 }
	else
	 {
	  if (r0 < (1.0 + 0.5 * fabs(d_umbra)))
		 {
		  if (d_umbra > 0) phase = 2;  // non-central annular eclipse
		  else phase = 3;     // non-central total eclipse
		 }
	  else
		 {
		  if (r0 < (1.0 + 0.5*d_penumbra)) phase = 1;   // partial eclipse
		  else phase = 0;   // no eclipse
		 }
	  }

	rs[2]*=flat;  // restore from flatting of the Earth
	rm[2]*=flat;

	return phase;
 }

void Eclipse::maxpos (double jd, double tdut, double& phi, double& lamda)
 {
  /* Calculates the geographic position of the place of maximum eclipse
	  at the time jd for a non-central eclipse. (NOTE that in case of
	  a central eclipse the maximum position will be calculated by
	  function solar. Do not use this function in that case as the
	  result would be incorrect! No check is being done in this routine
	  whether an eclipse is visible at all. Use maxpos only in case of a
	  confirmed partial or non-central eclipse.

	  INPUT:
	  jd = Modified Julian Date (UT)
	  tdut = TDT - UT in sec

	  OUTPUT:
	  phi, lamda: Geographic latitude and longitude (in radians)
					  of maximum eclipse at the time
	 */

	const double flat = 0.996633;  // flatting of the Earth
	double s0;
	Vec3 ve;

	// get the apparent equatorial coordinates of the sun and the moon
	equ_sun_moon(jd, tdut);

	rs[2]/=flat;  // adjust for flatting of the Earth
	rm[2]/=flat;
	rint.assign ();
	lamda = 0;
	phi = 0;

	// intersect shadow axis with Earth
	eshadow = rm - rs;
	eshadow = vnorm(eshadow);    // direction vector of shadow

	s0 = - dot(rm, eshadow);   // distance Moon - fundamental plane
	rint = rm + s0 * eshadow;
	rint = vnorm(rint); // normalize to 1 Earth radius
	rint[2] *= flat;    // vector to position of maximum eclipse
	ve = carpol(rint);

	lamda = ve[1] - lsidtim(jd,0,ep2)*0.261799387799; // geographic coordinates
	if (lamda > M_PI) lamda -= 2.0*M_PI;
	if (lamda < (-M_PI)) lamda += 2.0*M_PI;
	phi = sqrt(rint[0]*rint[0] + rint[1]*rint[1])*0.993305615;
	phi = atan2(rint[2],phi);

	rs[2]*=flat;  // restore from flatting of the Earth
	rm[2]*=flat;
 }

void Eclipse::penumd (double jd, double tdut, Vec3& vrm, Vec3& ves,
							 double& dpn, double& pang)
 {
  /* Calculates various items needed for finding out the northern and
	  southern border of the penumbra during a solar eclipse.

	  INPUT:
	  jd = Modified Julian Date (UT)
	  tdut = TDT - UT in sec

	  OUTPUT:
	  vrm = position vector of Moon adjusted for flattening
	  ves = unit vector pointing into the direction of the center of the shadow
	  dpn = diameter of the penumbra at the fundamental plane
	  pang = angle of penumbral half-cone in radians
	 */

	const double flat = 0.996633;  // flatting of the Earth
	const double ds = 218.245445;  // diameter of Sun in Earth radii
	const double dm = 0.544986;   // diameter of Moon in Earth radii
	double s0, r2;

	// get the apparent equatorial coordinates of the sun and the moon
	equ_sun_moon(jd, tdut);
	rs[2]/=flat;  // adjust for flatting of the Earth
	rm[2]/=flat;

	// intersect shadow axis with Earth
	eshadow = rm - rs;
	pang = abs(eshadow);
	eshadow = vnorm(eshadow);    // direction vector of shadow
	ves = eshadow;
	vrm = rm;

	s0 = - dot(rm, eshadow);   // distance Moon - fundamental plane

	r2 = abs(rs - rm);
	dpn = (ds + dm) * s0 / r2 + dm;

	// penumbral angle
	pang = asin((dm + ds) / (2.0 * pang));

	rs[2]*=flat;  // restore from flatting of the Earth
	rm[2]*=flat;
 }

void Eclipse::umbra (double jd, double tdut, Vec3& vrm, Vec3& ves,
                                                         double& dpn, double& pang)
 {
  /* Calculates various items needed for finding out the northern and
          southern border of the umbra during a solar eclipse.

          INPUT:
          jd = Modified Julian Date (UT)
          tdut = TDT - UT in sec

          OUTPUT:
          vrm = position vector of Moon adjusted for flattening
          ves = unit vector pointing into the direction of the center of the shadow
          dpn = diameter of the umbra at the fundamental plane
          pang = angle of umbral half-cone in radians
         */

        const double flat = 0.996633;  // flatting of the Earth
        const double ds = 218.245445;  // diameter of Sun in Earth radii
        const double dm = 0.544986;   // diameter of Moon in Earth radii
        double s0, r2;

        // get the apparent equatorial coordinates of the sun and the moon
        equ_sun_moon(jd, tdut);
        rs[2]/=flat;  // adjust for flatting of the Earth
        rm[2]/=flat;

        // intersect shadow axis with Earth
        eshadow = rm - rs;
        pang = abs(eshadow);
        eshadow = vnorm(eshadow);    // direction vector of shadow
        ves = eshadow;
        vrm = rm;

        s0 = - dot(rm, eshadow);   // distance Moon - fundamental plane

        r2 = abs(rs - rm);
        dpn = (ds - dm) * s0 / r2 - dm;

        // umbral angle
        pang = asin((ds - dm) / (2.0 * pang));

        rs[2]*=flat;  // restore from flatting of the Earth
        rm[2]*=flat;
 }

void Eclipse::equ_sun_moon(double jd, double tdut)
 {
  /* Get the equatorial coordinates of the Sun and the Moon and
	  store them in rs and rm.
	  Also store the time t in Julian Centuries and the correction
	  ep2 for the Apparent Sidereal Time.
	  jd = Modified Julian Date (UT)
	  tdut = TDT - UT in sec
	*/
	double ae = 23454.77992; // 149597870.0/6378.14 =  1AE -> Earth Radii
	Mat3 mx;

	t = julcent (jd) + tdut / 3.15576e9;  // =(86400.0 * 36525.0);
	rs = sun.position(t);
	rm = moon.position(t);
	rs = eclequ(t,rs);
	rm = eclequ(t,rm);

	// correct Moon coordinates for center of figure
	mx = zrot(-2.4240684e-6); // +0.5" in longitude
	rm = mxvct(mx,rm);
	mx = yrot(-1.2120342e-6); // -0.25" in latitude
	rm = mxvct(mx,rm);
	mx = nutmat(t,ep2);   // nutation
	rs = mxvct(mx,rs);    // apparent coordinates
	rs = aberrat(t,rs);
	rs *= ae;
	rm = mxvct(mx,rm);
 }

double Eclipse::duration (double jd, double tdut, double& width)
 {
  /* Get the duration of a central eclipse at the center point
	  for MJD jd and TDT-UT tdut in seconds.
	  Also return the width of the umbra in km.
	  A call to solar with the respective jd must have been done
	  prior to calling this function !!!
	 */
  const double omega = 4.3755e-3;  // radial velocity of Earth in rad/min.

  double dur, lm, pa, umbold;
  Vec3 rold, eold, rsold, rmold;
  Mat3 mx;

  // save old values for jd
  rold = rint;
  eold = eshadow;
  umbold = d_umbra;
  rsold = rs;
  rmold = rm;

  dur = 0.1;  // 0.1 min
  if (solar(jd+dur/1440.0,tdut, lm, pa) > 3)
	{
	 mx = zrot(dur*omega);
	 rint = mxvct(mx,rint);
	 rint -= rold;
	 pa = dot (rint, eold);
	 lm = dot (rint, rint) - pa*pa;
	 if (lm > 0) lm = sqrt(lm);
	 else lm = 0;
	 if (lm > 0) dur = fabs(umbold) / lm * dur * 60.0;
	 else dur = 0;
	}

  else dur = -1;

  // restore old values for jd
  d_umbra = umbold;
  eshadow = eold;
  eold = rold*rint;  // direction perpendicular of apparent shadow movement
  rint = rold;
  rs = rsold;
  rm = rmold;

  // get width of umbra at center location
  rold = vnorm (eold);
  pa = dot(rold,eshadow);
  if (pa > 1.0) pa = 1.0;
  if (pa < -1.0) pa = -1.0;
  pa = fabs(sin(acos(pa)));
  if (pa < 0.00001) pa = 0.00001;
  width = d_umbra / pa * 6378.14;  // allow for projection
  width = fabs(width);

  return dur;
 }

Vec3 Eclipse::GetRSun ()    // get Earth - Sun vector in Earth radii
 {
  return rs;
 }

Vec3 Eclipse::GetRMoon ()   // get Earth - Moon vector in Earth radii
 {
  return rm;
 }

double Eclipse::GetEp2 ()   // get the ep2 value
 {
  return ep2;
 }

int Eclipse::lunar (double jd, double tdut)
 {
  /* check whether lunar eclipse is in progress at
	  Modified Julian Date jd (UT).
	  tdut = TDT - UT in sec

	  RETURN:
	  0 : No eclipse
	  1 : Partial Penumbral Eclipse
	  2 : Total Penumbral Eclipse
	  3 : Partial Umbral Eclipse
	  4 : Total Umbral Eclipse
	 */

	const double dm = 0.544986;   // diameter of Moon in Earth radii
	const double ds = 218.245445;  // diameter of Sun in Earth radii

	double umbra, penumbra;
	double r2, s0, sep;
	int phase;
	Vec3 v1, v2;

	// get position of Sun and Moon
	equ_sun_moon(jd, tdut);

	// get radius of umbra and penumbra
	r2 = abs(rs);
	s0 = abs (rm);
	umbra = 1.02*fabs((ds - 2.0) * s0 / r2 - 2.0)*0.5; // radius of umbra
	penumbra = 1.02*fabs((ds + 2.0) * s0 / r2 + 2.0)*0.5;//radius of penumbra
	/* (the factor 1.02 allows for enlargment of shadow due to
		 Earth's atmosphere) */

	// get angular seperation of center of shadow and Moon
	r2 = abs(rm);
	sep = dot(rs,rm)/(abs(rs)*r2);
	if (fabs(sep) > 1.0) sep = 1.0;
	sep = acos(sep);  // in radians
	sep = fabs(tan(sep)*r2);  // distance of Moon and shadow in Earth radii

	// Now check the kind of eclipse
	if (sep < (umbra - dm/2.0)) phase = 4;
	else if (sep < (umbra + dm/2.0)) phase = 3;
	else if (sep < (penumbra - dm/2.0)) phase = 2;
	else if (sep < (penumbra + dm/2.0)) phase = 1;
	else phase = 0;

	return phase;
 }


