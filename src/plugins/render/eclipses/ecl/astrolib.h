//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP
//

#if !defined(__astrolib_h)
#define __astrolib_h

#include "attlib.h"

/***********************************************************************
   Definitions of Astrolib Functions

   Author: Gerhard HOLTKAMP               6-JAN-2012
 ***********************************************************************/

double ddd (int d, int m, double s); // deg, min, sec -> decimal degrees
void dms (double dd, int &d, int &m, double &s); // dec deg -> deg, min, sec
double mjd (int day, int month, int year, double hour); // modified Julian date
double julcent (double mjuld);                // Julian centuries since 2000.0
void caldat (double mjd, int &day, int &month, int &year, double &hour);
double DefTdUt (int yr);   // default value for TDT - UT in seconds
double lsidtim (double jd, double lambda, double ep2);  // Sidereal Time

double eps (double t);  //   obliquity of ecliptic
Vec3 eclequ (double t, Vec3& r1);  //  ecliptic -> equatorial
Vec3 equecl (double t, Vec3& r1);  //  equatorial -> ecliptic
Mat3 pmatecl (double t1, double t2);  // ecl. precession
Mat3 pmatequ (double t1, double t2);  // equ. precession
Mat3 nutmat (double t, double& ep2, bool hipr = false); // nutation (equatorial)
Mat3 nutecl (double t, double& ep2);  // nutation matrix (ecliptic)
Mat3 PoleMx (double xp, double yp);   // Polar motion matrix
Vec3 aberrat (double t, Vec3& ve);   //  aberration

Vec3 GeoPos (double jd, double ep2, double lat, double lng, double ht);
Vec3 GeoPos (double jd, double ep2, double lat, double lng, double ht,
             double xp, double yp);
Vec3 EquHor (double jd, double ep2, double lat, double lng, Vec3 r);
Vec3 HorEqu (double jd, double ep2, double lat, double lng, Vec3 r);
void AppPos (double jd, double ep2, double lat, double lng, double ht,
             int solsys, Vec3 r, double& azim, double& elev, double& dist);
void AppRADec (double jd, double ep2, double lat, double lng,
               double azim, double elev, double& ra, double& dec);
double Refract (double h, double p = 1015.0, double t = 15.0); // refraction  

double eccanom (double man, double ecc);  // eccentric anomaly
double hypanom (double mh, double ecc);   // hyperbolic anomaly
void ellip (double gm, double t0, double t, double a, double ecc,
            double m0, Vec3& r1, Vec3& v1); // elliptic state vector
void hyperb (double gm, double t0, double t, double a, double ecc,
             Vec3& r1, Vec3& v1);  // hyperbolic state vector
void parab (double gm, double t0, double t, double q, double ecc,
            Vec3& r1, Vec3& v1);  // elliptic state vector
void kepler (double gm, double t0, double t, double m0, double a, double ecc,
             double ran, double aper, double inc, Vec3& r1, Vec3& v1);
void oscelm (double gm, double t, Vec3& r1, Vec3& v1,
             double& t0, double& m0, double& a, double& ecc,
             double& ran, double& aper, double& inc);

Vec3 QuickSun (double t);   // low precision position of the Sun at time t

class Sun200      // Calculating the Sun in epoch J2000.0 coordinates
 {
  public:
   Sun200();
   Vec3 position (double t);   // position of the Sun
   void state (double t, Vec3& rs, Vec3& vs);  // State vector of the Sun

  private:
   double c3[9], s3[9];
   double c[9], s[9];
   double m2, m3, m4, m5, m6;
   double d, a, uu, tt;
   double cl, sl, cb, sb;
   double u, v, dl, dr, db;
   void addthe (double c1, double s1, double c2, double s2,
                double& cc, double& ss);
   void term (int i1, int i, int it, double dlc, double dls, double drc,
              double drs, double dbc, double dbs);
   void pertven();
   void pertmar();
   void pertjup();
   void pertsat();
   void pertmoo();
 };

class Moon200     // Calculating the position of the Moon in J2000.0
 {
  public:
   Moon200();
   Vec3 position (double t);   // position of the Moon

  private:
   double dgam, dlam, n, gam1c, sinpi;
   double l0, l, ls, f, d, s;
   double dl0, dl, dls, df, dd, ds;
   double co[13][4];
   double si[13][4];
   void addthe (double c1, double s1, double c2, double s2,
                double& c, double& s);
   double sinus (double phi);
   void long_periodic (double t);
   void minit(double t);
   void term (int p, int q, int r, int s, double& x, double& y);
   void addsol(double coeffl, double coeffs, double coeffg,
               double coeffp, int p, int q, int r, int s);
   void solar1();
   void solar2();
   void solar3();
   void addn (double coeffn, int p, int q, int r, int s,
              double& n, double&x, double& y);
   void solarn (double& n);
   void planetary (double t);
 };

class Eclipse      // Eclipse Calculations
 {
  public: 
   Eclipse();
   int solar (double jd, double tdut, double& phi, double& lamda);
   void maxpos (double jd, double tdut, double& phi, double& lamda);
   void penumd (double jd, double tdut, Vec3& vrm, Vec3& ves, 
                double& dpn, double& pang);
   void umbra (double jd, double tdut, Vec3& vrm, Vec3& ves,
                double& dpn, double& pang);
   double duration (double jd, double tdut, double& width);
   Vec3 GetRSun ();    // get Earth - Sun vector in Earth radii
   Vec3 GetRMoon ();   // get Earth - Moon vector in Earth radii
   double GetEp2 ();   // get the ep2 value
   int lunar (double jd, double tdut);

  private: 
   Sun200 sun;
   Moon200 moon;
   Vec3 rs, rm;   // position of the Sun and the Moon
   Vec3 eshadow;  // unit vector in direction of shadow
   Vec3 rint;     // intersection shadow axis - Earth surface
   double t;      // time in Julian Centuries
   double ep2;    // correction for Apparent Sideral Time
   double d_umbra; // diameter of umbra in Earth Radii
   double d_penumbra; // diameter of penumbra in Earth Radii
   void equ_sun_moon(double jd, double tdut);
 };

#endif         // __astrolib_h sentry.


