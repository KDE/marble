// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Gerhard HOLTKAMP
//

#if !defined(__planetarysats_h)
#define __planetarysats_h

#include "attlib.h"
#include "astrolib_export.h"

class ASTROLIB_EXPORT PlanetarySats     // Calculate spacecraft around other planets
{
  public:
    PlanetarySats();
    ~PlanetarySats();

    void setStepWidth(double s);  // set the step width (seconds)
    void setDeltaTAI_UTC(double d);  // set IERS Parameter TAI - UTC
    void setAutoTAI_UTC();  // IERS Parameter TAI - UTC to auto
    void getTime ();  // Get System Time and Date
    void setDeltaRT(double drt);
    void setMJD(int year, int month, int day, int hour, int min, double sec);// set time
    static void getDatefromMJD(double mjd, int &year, int &month, int &day, int &hour, int &min, double &sec);
    void setSatFile(char* fname);
    void setPlanet(char* pname);
    int selectSat(char* sname);
    void getSatName(char* sname) const;
    void setStateVector(double mjd, double x, double y, double z, double vx, double vy, double vz);
    int getStateVector(int nsat);
    void stateToKepler();
    void getKeplerElements(double &perc, double &apoc, double &inc, double &ecc, double &ra, double &tano, double &m0, double &a, double &n0);
    void getPlanetographic(double &lng, double &lat, double &height) const;
    void getFixedFrame(double &x, double &y, double &z, double &vx, double &vy, double &vz);
    void currentPos();
    void nextStep();
    double getLastMJD() const;

  private:  
    void plsatinit();  // initialize PlanetarySats
    static double atan23 (double y, double x);  // atan without singularity for x,y=0
    void getMercury();
    void getVenus();
    void getMoon();
    void getMars();
    void getSatPos (double t);
    static Mat3 getSelenographic (double jd);

   // data fields
      
      char pls_satelmfl[205];  // name of file for satellite state vectors
      char pls_satname[40];  // name of satellite
      char pls_plntname[40]; // name of planet

      bool pls_moonflg;  // true if Moon, false if other body

      int pls_day;           // date
      int pls_month;
      int pls_year;
      int pls_hour;
      int pls_minute;
      int pls_second;
      double pls_time;     // current time in MJD (UTC)
      double pls_del_tdut;   // TDT - UT in sec
      int pls_del_auto;      // 1 = automatic del_tdut,  0 = manual
      double pls_step;       // stepwidth in sec
      double pls_delta_rt;   // delta time to R/T in hours
      
      double pls_tepoch;  // MJD epoch of state vector (TT)
      Vec3 pls_rep;  // state vector km and km/s
      Vec3 pls_vep;
      
      double pls_ra; // Right Ascension
      double pls_per; // argument of perige center
      double pls_m0;  // mean anomaly
      double pls_ecc; // eccentricity
      double pls_inc; // inclination
      double pls_a;  // semi-major axis (km)
      double pls_n0;  // mean motion  (rev/day)
      
      double pls_GM;  // graviatational constant (m^3/s^2)
      double pls_J2;  // J2 gravitational term
      double pls_R0; // equatorial radius (km)
      double pls_flat;  // flattening factor
      double pls_axl0;  // l-direction of rotation axis
      double pls_axl1;  // delta of axl0
      double pls_axb0;  // b-direction of rotation axis
      double pls_axb1;  // delta of axb0
      double pls_W;    // location of prime meridian
      double pls_Wd;   // daily variation of W.

      Vec3 pls_r;  // current state vector  m and m/s
      Vec3 pls_v;     
      double pls_lat;  // planetary latitude (decimal degrees)
      double pls_lng;  // planetary longitude (decimal degrees)
      double pls_height; // height above reference ellipsoid (km)    
};

#endif         // __planetarysats_h sentry.

