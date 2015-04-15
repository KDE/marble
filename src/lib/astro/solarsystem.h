//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Gerhard Holtkamp
//

#if !defined(__solarsystem_h)
#define __solarsystem_h

#include "attlib.h"
#include "astrolib_export.h"

class ASTROLIB_EXPORT SolarSystem     // Calculate Positions of Solar System Bodies
{
  public:
    SolarSystem();
    ~SolarSystem();

    void setTimezone(double d);  // set timezone for I/O
    void setDeltaTAI_UTC(double d);  // set IERS Parameter TAI - UTC
    void setAutoTAI_UTC();  // IERS Parameter TAI - UTC to auto
    void setCurrentMJD(int year, int month, int day, int hour, int min, double sec); // set current time
    void setCurrentMJD();  // sets current MJD to R/T 
    double getMJD(int year, int month, int day, int hour, int min, double sec); // get MJD from date 
    void getDatefromMJD(double mjd, int &year, int &month, int &day,
                        int &hour, int &min, double &sec); // convert MJD into date and time
    void setEpoch (double yr);  // set epoch of coordinates
    void setNutation (bool nut);  // specify whether nutation is to be included.
    void setCentralBody (char* pname); // select which planet is central body
    void includeUser (bool uact);  // activate user defined object

    void getSun (double& ra, double& decl);  // RA and Dec for the Sun 
    void getMoon (double& ra, double& decl);  // RA and Dec for the Moon 
    void getMercury (double& ra, double& decl);  // RA and Dec for Mercury 
    void getVenus (double& ra, double& decl);  // RA and Dec for Venus 
    void getEarth (double& ra, double& decl);  // RA and Dec for Earth (with regard to the Sun) 
    void getMars (double& ra, double& decl);  // RA and Dec for Mars 
    void getJupiter (double& ra, double& decl);  // RA and Dec for Jupiter 
    void getSaturn (double& ra, double& decl);  // RA and Dec for Saturn 
    void getUranus (double& ra, double& decl);  // RA and Dec for Uranus 
    void getNeptune (double& ra, double& decl);  // RA and Dec for Neptune
    void getIo (double& ra, double& decl);  // RA and Dec for Io 
    void getEuropa (double& ra, double& decl);  // RA and Dec for Europa 
    void getGanymede (double& ra, double& decl);  // RA and Dec for Ganymede 
    void getCallisto (double& ra, double& decl);  // RA and Dec for Callisto 
    void getRhea (double& ra, double& decl);  // RA and Dec for Rhea 
    void getTitan (double& ra, double& decl);  // RA and Dec for Titan 
    void getMimas (double& ra, double& decl);  // RA and Dec for Mimas 
    void getEnceladus (double& ra, double& decl);  // RA and Dec for Enceladus 
    void getDione (double& ra, double& decl);  // RA and Dec for Dione 
    void getUser (double& ra, double& decl);  // RA and Dec for user defined object 

    void getPhysSun (double &pdiam, double &pmag);  // Physical elements Sun 
    void getPhysMercury(double &pdiam, double &pmag, double &pphase);  // Physical elements Mercury 
    void getPhysVenus(double &pdiam, double &pmag, double &pphase);  // Physical elements Venus 
    void getPhysEarth(double &pdiam, double &pmag, double &pphase);  // Physical elements Earth 
    void getPhysMars(double &pdiam, double &pmag, double &pphase);  // Physical elements Mars 
    void getPhysJupiter(double &pdiam, double &pmag, double &pphase);  // Physical elements Jupiter 
    void getPhysSaturn(double &pdiam, double &pmag, double &pphase);  // Physical elements Saturn 
    void getPhysUranus(double &pdiam, double &pmag, double &pphase);  // Physical elements Uranus 
    void getPhysNeptune(double &pdiam, double &pmag, double &pphase);  // Physical elements Neptune 
    double getDiamMoon (); // Apparent diameter for the Moon 
    void getLunarLibration (double &lblon, double &lblat, double &termt);  // librations of the Moon and terminator position
    void getLunarPhase (double &phase, double &ildisk, double &amag);  // phase and mag of Moon

    void getPhysIo(double &pdiam, double &pmag, double &pphase);  // Physical elements Io 
    void getPhysEuropa(double &pdiam, double &pmag, double &pphase);  // Physical elements Europa 
    void getPhysGanymede(double &pdiam, double &pmag, double &pphase);  // Physical elements Ganymede 
    void getPhysCallisto(double &pdiam, double &pmag, double &pphase);  // Physical elements Callisto 
    void getPhysRhea(double &pdiam, double &pmag, double &pphase);  // Physical elements Rhea 
    void getPhysTitan(double &pdiam, double &pmag, double &pphase);  // Physical elements Titan 
    void getPhysMimas(double &pdiam, double &pmag, double &pphase);  // Physical elements Mimas 
    void getPhysEnceladus(double &pdiam, double &pmag, double &pphase);  // Physical elements Enceladus 
    void getPhysDione(double &pdiam, double &pmag, double &pphase);  // Physical elements Dione 
    void getPhysUser(double &pdiam, double &pmag, double &pphase);  // Physical elements user defined object

    Vec3 getPlanetocentric (double ra, double decl);  // planetocentric position
    void getPlanetographic (double ra, double decl, double &lng, double &lat);
    void getSkyRotAngles (double &raz1, double &rax, double &raz2);

    void putOrbitElements (double t0, double pdist, double ecc, double ran, double aper, double inc, double eclep);
    void putEllipticElements (double t0, double a, double m0, double ecc, double ran, double aper, double inc, double eclep);
    void getOrbitPosition (double& ra, double& decl);
    double getDistance();  // distance in AU of Kepler object
    double getCometMag(double g, double k);  // apparent magnitude of comet
    double getAsteroidMag(double h, double g);   // apparent magnitude of asteroid
    void putOrbitUser (double t0, double pdist, double ecc, double ran, double aper, double inc, double eclep);
    void putEllipticUser (double t0, double a, double m0, double ecc, double ran, double aper, double inc, double eclep);
    void putConstUser(double j2, double r0, double flat, double axl0, double axl1, double axb0, double axb1, double w, double wd, double gm);  // store physical user constants
    
    double DmsDegF (double h); // conversion from Format DDD.MMSS into d.fff
    double DegFDms (double h); // conversion from Format d.fff into DDD.MMSS


  private:  
    void ssinit();  // initialize SolarSystem
    double atan23 (double y, double x);  // atan without singularity for x,y=0
    void DefTime ();  // Get System Time and Date
    void getRaDec (Vec3 r1, double& ra, double& decl); // convert r1 into RA and DEC
    void updateSolar();  // update all positions of planets according to current MJD.
   
    void MoonDetails();
    void MoonLibr (double jd, Vec3 rm, Vec3 sn, double &lblon, double &lblat, double &termt);
    Vec3 SnPos (double &ep2, double &els);
    Vec3 MnPos (double &ep2, double &els);
    Vec3 PosUser(double dt);  // position of User defined object
    Mat3 getSelenographic();
    void getConstSun();  // Sun constants
    void getConstMoon();  // Moon planetary constants
    void getConstMercury();  // Mercury planetary constants
    void getConstVenus();  // Venus planetary constants
    void getConstEarth();  // Earth planetary constants
    void getConstMars();  // Mars planetary constants
    void getConstJupiter(); // Jupiter planerary constants
    void getConstSaturn(); // Saturn planerary constants
    void getConstUranus(); // Uranus planerary constants
    void getConstNeptune(); // Neptune planerary constants
    void getConstIo(); // Io planerary constants
    void getConstEuropa(); // Europa planerary constants
    void getConstGanymede(); // Ganymede planerary constants
    void getConstCallisto(); // Callisto planerary constants
    void getConstRhea(); // Rhea planerary constants
    void getConstTitan(); // Titan planerary constants
    void getConstMimas(); // Mimas planerary constants
    void getConstEnceladus(); // Enceladus planerary constants
    void getConstDione(); // Dione planerary constants
    void getConstUser(); // User planerary constants

    void getPlanMat();  // get Matrix to convert from J2000.0 into planetary coordinates

   // data fields

      bool ss_update_called;  // true if updateSolar() has already been called
      bool ss_moon_called;    // true if special Moon calculation called
      bool ss_planmat_called; // true if matrix for planetary coordintes called
      bool ss_kepler_stored;  // true if Kepler elements for comet or asteroid stored
      bool ss_kepler_called;  // true if comet or asteroid position calculated
      bool ss_user_stored;    // true if orbit elements for user defined object stored
      bool ss_user_active;    // true if user defined object activated

      int ss_day;           // date
      int ss_month;
      int ss_year;
      int ss_hour;
      int ss_minute;
      int ss_second;
      double ss_time;       // currently used MJD
      double ss_tzone;      // timezone in hours
      double ss_del_tdut;   // TDT - UT in sec
      int ss_del_auto;      // 1 = automatic del_tdut,  0 = manual
      bool ss_RT;   // true if calculations in Real Time, false if time manually set to MJD.
      bool ss_nutation;  // true if position corrected for nutation
      double ss_epoch;  // epoch of coordinates in MJD
      int ss_central_body;  // body defining the coordinate origin (0=sun,1=moon,2=mercury etc.; -1=user)

     Vec3 ss_rm, ss_rs;  // position of Moon and Sun in ecliptic of date (A.U.)
     Vec3 ss_pmer, ss_pven, ss_pearth, ss_pmars, ss_pjup, ss_psat, ss_pura, ss_pnept; // positions of planets
     Vec3 ss_pio, ss_peuropa, ss_pganymede, ss_pcallisto;  // positions of Jupiter moons
     Vec3 ss_prhea, ss_ptitan, ss_pmimas, ss_penceladus, ss_pdione; // positions of Saturn moons
     Vec3 ss_user;  // position of user defined object

     double ss_moon_mag;  // apparent magnitude of the Moon
     double ss_moon_lblon;  // longitude of lunar libration
     double ss_moon_lblat;  // latitude of lunar libration
     double ss_moon_term;   // position of lunar terminator
     double ss_moon_ildisk; // fraction of Moon's illuminated disk
     double ss_moon_phase;  // phase of Moon

     double ss_GM;  // graviatational constant (m^3/s^2)
     double ss_J2;  // J2 gravitational term
     double ss_R0; // equatorial radius (km)
     double ss_flat;  // flattening factor
     double ss_axl0;  // l-direction of rotation axis
     double ss_axl1;  // delta of axl0
     double ss_axb0;  // b-direction of rotation axis
     double ss_axb1;  // delta of axb0
     double ss_W;    // location of prime meridian
     double ss_Wd;   // daily variation of W.

     // data for user defined object:
     double ss_user_GM;  // graviatational constant (m^3/s^2)
     double ss_user_J2;  // J2 gravitational term
     double ss_user_R0; // equatorial radius (km)
     double ss_user_flat;  // flattening factor
     double ss_user_axl0;  // l-direction of rotation axis
     double ss_user_axl1;  // delta of axl0
     double ss_user_axb0;  // b-direction of rotation axis
     double ss_user_axb1;  // delta of axb0
     double ss_user_W;    // location of prime meridian
     double ss_user_Wd;   // daily variation of W.
     double ss_user_t0;  // time of perihelion passage or epoch of elliptical elements (MJD
     double ss_user_m0;  // Mean Anomaly in degrees
     double ss_user_a;   // semi-major axis in AU
     double ss_user_ecc; // eccentricity
     double ss_user_ran; // right ascension of ascending node in degrees
     double ss_user_aper; // argument of perihelion
     double ss_user_inc;  // inclination in degrees
     double ss_user_eclep; // epoch of ecliptic and equator for these elements

     Mat3 ss_planmat; // matrix to convert from J2000.0 into planetary coordinates
     double ss_lat;  // planetary latitude (decimal degrees)
     double ss_lng;  // planetary longitude (decimal degrees)
     double ss_height; // height above reference ellipsoid (km) 

     double ss_t0;  // time of perihelion passage or epoch of elliptical elements (MJD)
     double ss_m0;  // Mean Anomaly in degrees
     double ss_a;   // semi-major axis in AU
     double ss_ecc; // eccentricity
     double ss_ran; // right ascension of ascending node in degrees
     double ss_aper; // argument of perihelion
     double ss_inc;  // inclination in degrees
     double ss_eclep; // epoch of ecliptic and equator for these elements
     Vec3 ss_comet;  // position vector of comet or asteroid in AU

};

#endif         // __solarsystem_h sentry.

