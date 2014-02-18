//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP
//

#if !defined(__eclsolar_h)
#define __eclsolar_h

#include "attlib.h"
#include "astrolib_export.h"

const int GBL_ECLBUF = 12;  // maximum number of eclipses allowed
const int MAXLUN = 15;

typedef double PMJD[MAXLUN];  // array of MJD's of the respective phase

class ASTROLIB_EXPORT EclSolar     // Calculate Solar Eclipses
{
  public:
    EclSolar();
    ~EclSolar();

    int getYear() const;  // the year currently used by EclSolar
    void putYear(int yr);  // set the year to be used by EclSolar
    int getNumberEclYear();  // get the number of eclipses of the currently selected year
    void setLunarEcl(bool lecl);  // include lunar eclipses
    void setStepWidth(double s);  // set the step width (minutes)
    void setTimezone(double d);  // set timezone for I/O
    void setDeltaTAI_UTC(double d);  // set IERS Parameter TAI - UTC
    void setAutoTAI_UTC();  // IERS Parameter TAI - UTC to auto
    void setLocalPos(double lat, double lng, double hgt); // set local geographic coordinates
    void getLocalDetails(char *otxt);  // provide eclipse details for local position
    void setCurrentMJD(int year, int month, int day, int hour, int min, double sec); // set current time
    void getDatefromMJD(double mjd, int &year, int &month, int &day,
                        int &hour, int &min, double &sec) const; // convert MJD into date and time
    int getLocalVisibility(double &mjd_start, double &mjd_stop);  // local start and stop times for eclipse
    int getLocalTotal(double &mjd_start, double &mjd_stop);  // local start and stop times for totality/annularity
    int getLocalMax(double &mjdmax, double &magmax, double &elmax);  // get local (solar) eclipse maximum
    int getPenumbra(double &mjd_start, double &mjd_stop);  // start and stop times for penumbral eclipse of Moon
    int getPartial(double &mjd_start, double &mjd_stop);  // (global) start and stop times for partial phase
    int getTotal(double &mjd_start, double &mjd_stop);   // (global) start and stop times for totality/annularity
    void getEclYearInfo(char* wbuf);  // list of eclipses of the year
    int getEclYearInfo(int k, int &yr, int &month, int &day,
                   int &hour, int &min, double &sec, double &tzone, double &magn);
    int getEclTxt (int j, char* jtxt);  // get text for j-th eclipse
    void putEclSelect(int es);  // select particular eclipse for details
    void nextEcl();  // select the next eclipse for details
    void previousEcl();  // select the previous eclipse for details
    double getLastMJD() const;  // get the MJD last used in calculations
    void getMaxPos(double &lat, double &lng); // get position of maximum eclipse
    int eclPltCentral(bool firstc, double &lat, double &lng);  // calc central eclipse line
    int GNSBound(bool firstc, bool north, double& lat, double& lng); // northern or southern boundary
    int GRSBound(bool firstc, double& lat1, double& lng1, double& lat2, double& lng2);
                     // Rise / Set Boundary
    int centralBound(bool firstc, double& lat1, double& lng1, double& lat2, double& lng2);
    void getShadowCone(double mjd, bool umbra, int numpts, double* lat, double* lng);
    void setPenumbraAngle(double pa, int mode);

  private:  
    void esinit();  // initialize EclSolar
    static double atan23 (double y, double x);  // atan without singularity for x,y=0
    void DefTime ();  // Get System Time and Date
    void calcMaxPos(double &lat, double &lng); // get position of maximum eclipse
    static void GetMonth (int mm, char* mchr);
    static double phmjd (double yearf, double phase, double tdut,
				  int& eph, double& ejd, double& emag);
    void ckphase (double minmjd, double maxmjd, double yr,
              double deltdut, int &mp, PMJD p, double phase);
    static void dtmstr(double jdmoon, char *dts);
    void moonph();  // calculate phases of the Moon
    void eclStart();   // initialize detailed calcs for selected eclipse
    static double getlocmag(double jd, double ep2, double phi, double lamda,
                     double height, Vec3 rs, Vec3 rm, int& totflg);
    static int iscrs(double vrc0, double vrc1, double dpn,
                           double& vrx0, double& vrx1, double& vrx20, double& vrx21);
    void InitBound(); // initialize boundary calcs
    void InRSBound();  // initialize Sunrise/Sunset boundaries
    static double DegFDms (double h);
    int localStart(int j, double *spt, double *ept, int *spp,
                                   int p, char *otxt);
    static double navCourse (double lat1, double lng1, double lat2, double lng2); // navigation course from p1 to p2
    static void navNewPos (double d, double an, double lat1, double lng1, double &lat2, double &lng2);
    static double sunObscure(double l1, double l2, double m);  // get the Obscuration of the Sun

   // data fields

      bool eb_start_called;  // true if eclStart() has already been called for eclipse
      bool eb_moonph_called; // true if moonph has already been called for year
      bool eb_lunecl;        // true if lunar eclipses are to be included
      bool eb_lunactive;     // true if current eclipse is a lunar eclipse
      bool eb_local_called;  // true if local circumstances calculated

      int eb_day;           // date
      int eb_month;
      int eb_year;
      int eb_hour;
      int eb_minute;
      int eb_second;
      double eb_time;
      double eb_tzone;      // timezone in hours
      double eb_del_tdut;   // TDT - UT in sec
      int eb_del_auto;      // 1 = automatic del_tdut,  0 = manual
      double eb_geolat;     // geographic coordinates (angles in degrees)
      double eb_geolong;
      double eb_geoheight;  // in meters
      int eb_lstcall;  // 1 = last call moonph; 2 = caldisp; 0 = no call 
      int eb_locecl;   // 1 = local eclipse call; 0 = normal (global) call

      bool eb_finished;  // true if the loop run on consecutive calls is finished
      bool eb_finished2;
      int eb_numecl;   // number of eclipses of the year;
      int eb_eclselect; // eclipse selected for detailed calculation
      int eb_lastyear;   // last year for which moon phases were calculated
      double eb_lasttz;  // last used timezone
      double eb_lastdlt; // last delta-t used
      int eb_cstep;   // step width in minutes used for central eclipse output
      double eb_eclmjd [GBL_ECLBUF];  // the MJD's of the middle of the eclipses
      double eb_magnitude [GBL_ECLBUF];   // magnitude of respective eclipse
      int eb_phase [GBL_ECLBUF];    // phase of the eclipse. 0 if no eclipse,
                                    // 1 partial Sun, 2 non-central annular, 3 non-central total,
                                    // 4 annular, 5 total, 6 annual/total  Sun
                                    // -1 partial penumbral Moon, -2 penumbral Moon,
                                    // -3 partial Moon, -4 total Moon.
      int eb_nphase;   // number of phases for eclipse details
      int eb_spp[4];     // kind of eclipse phase i
      double eb_spt[4];  // start time in MJD for phase i
      double eb_ept[4];  // end time in MJD for phase i
      int eb_lccnt; // = 0 not visible, 1,2 = first rise/set; 3,4 second r/s
      double eb_lcb1, eb_lcb2, eb_lce1, eb_lce2;  // rise and set times (up to 2 possible)
      double eb_jdmaxps; // MJD of maximum phase
      double eb_maxps;  // value of maximum phase
      double eb_maxelv; // elevation at maximum phase
      double eb_ltotb;  // begin of totality/annularity at local position
      double eb_ltote;  // end of totality/annularity at local position
      int eb_cphs;   // phase of the central eclipse for center line plotting
      double eb_clat; // current latitude of center line (in decimal degrees)
      double eb_clng; // current longitude of center line (in decimal degrees)
      double eb_cmxlat; // latitude of maximum eclipse (in decimal degrees)
      double eb_cmxlng; // longitude of maximum eclipse (in decimal degrees) 
      double eb_jdstart; // (global) start jd of eclipse
      double eb_jdstop;  // (global) end jd of eclipse
      double eb_lastjd;  // last used MJD for eclipse calcs
      double eb_dpb;   // Base value for diameter of penumbra
      double eb_dpd;   // delta value for diameter of penumbra
      double eb_penangle; // factor for multiplying the penumbra angle (1.0 for normal borders)
      int eb_penamode;  // mode for using eb_penangle
      Vec3 eb_ubm;  // Penumbra (or Umbra or Moon) base vector
      Vec3 eb_ube;  // Shadow base vector for upper boundary
      Vec3 eb_udm;  // Penumbra base delta vector
      Vec3 eb_ude;  // Shadow delta vector for upper boundary
      Vec3 eb_lbe;  // Shadod base vector for lower boundary
      Vec3 eb_lde;  // Shadow delta vector for lower boundary

};

#endif         // __eclsolar_h sentry.

