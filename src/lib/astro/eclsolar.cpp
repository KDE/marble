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
* Calculate Solar Eclipses                                                 *
*                                                                          *
*                                                                          *
* The algorithm for the phases of the Moon and of the dates for the        *
* equinoxes and solstices of the Sun as well as the dates of eclipses      *
* are based on Jean Meeus "Astronomical Formulae for Calculators",         *
* Willman-Bell, Inc., Richmond, Virginia, 1988                             *
*                                                                          *
* The algorithm for the Modified Julian Date and the calendar as well as   *
* the detailed eclipse calculations are based on                           *
* O.Montebruck and T.Pfleger, "Astronomy with a PC",                       *
* Springer Verlag, Berlin, Heidelberg, New York, 1989                      *
* and on the                                                               *
* "Explanatory Supplement to the Astronomical Almanac"                     *
* University Science Books, Mill Valley, CA, 1992                          *
*                                                                          *
* Open Source Code. License: GNU LGPL Version 2+                          *
*                                                                          *
* Author: Gerhard HOLTKAMP,        28-JAN-2013                              *
***************************************************************************/

/*------------ include files and definitions -----------------------------*/
#include "eclsolar.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
using namespace std;

#include "astrolib.h"

/* const double PI = 3.14159265359; */
const double degrad = M_PI / 180.0;

// ################ Solar Eclipse Class ####################

EclSolar::EclSolar()
{
 esinit();
}

EclSolar::~EclSolar()
{

}

double EclSolar::atan23 (double y, double x)
 {
  /* redefine atan2 so that it does'nt crash when both x and y are 0 */
  double result;

  if ((x == 0) && (y == 0)) result = 0;
  else result = atan2 (y, x);

  return result;
 }

void EclSolar::esinit()
{
 // initialize eclipse data
  eb_start_called = false;
  eb_moonph_called = false;
  eb_lunecl = true;
  eb_lunactive = false;
  eb_local_called = false;

  eb_day = 1;
  eb_month = 1;
  eb_year = 2012;
  eb_hour = 0;
  eb_minute = 0;
  eb_second = 0;
  eb_tzone = 0;
  eb_del_auto = 1;
  DefTime();
  eb_time = 0;
  eb_del_tdut = DefTdUt(eb_year);  
  eb_geolat = 0;
  eb_geolong = 0;
  eb_geoheight = 0;
  eb_lstcall = 0;
  eb_locecl = 0;
  eb_lastyear = -9999;
  eb_numecl = 0;
  eb_lasttz = eb_tzone;
  eb_lastdlt = eb_del_tdut;
  eb_cstep = 1.0;
  eb_cmxlat = 0;
  eb_cmxlng = 0;
  eb_penangle = 1.0;
  eb_penamode = 0;
}

void EclSolar::DefTime ()  // Get System Time and Date
 {
  time_t tt;
  int hh, mm, ss;
  double jd, hr;

  tt = time(NULL);
  jd = 40587.0 + tt/86400.0; // seconds since 1-JAN-1970

  caldat(jd, hh, mm, ss, hr);
  eb_year = ss;
  eb_month = mm;
  eb_day = hh;

  dms(hr, hh, mm, jd);
  eb_hour = hh;
  eb_minute = mm;
  eb_second = int(jd);
  if (eb_del_auto) eb_del_tdut = DefTdUt(eb_year);
  };

int EclSolar::getYear() const
{
  return eb_year;
}

void EclSolar::putYear(int yr)
{
  eb_start_called = false;
  eb_moonph_called = false;
  eb_local_called = false;
  eb_lunactive = false;

  eb_year = yr;
  if (eb_del_auto)  eb_del_tdut = DefTdUt(eb_year);
  moonph();

}  
  
int EclSolar::getNumberEclYear()
{
 // RETURN the number of eclipses of the currently selected year

    int j, k;

    if (!eb_moonph_called) moonph();  // calculate the eclipses of the year

    if (eb_lunecl) return eb_numecl;

    k = 0;
    for (j=0; j<eb_numecl; j++)
    {
        if (eb_phase[j] > 0) k++;
    }

    return k;
}
void EclSolar::setLunarEcl(bool lecl)
{
  // if lecl = true include lunar eclipses in addition to solar ones
  if (lecl) eb_lunecl = true;
  else eb_lunecl = false;
  eb_start_called = false;
  eb_local_called = false;
}

void EclSolar::setStepWidth(double s)
{
  // set the step width (in minutes) used for calculations
  if (s < 0.01) eb_cstep = 0.01;
  else eb_cstep = s;
}

void EclSolar::setTimezone(double d)
{
  // set the timezone to d (hours difference from UTC) for I/O
  eb_tzone = d;
}

void EclSolar::setDeltaTAI_UTC(double d)
{
  // c is the difference between TAI and UTC according to the IERS
  // we have to add 32.184 sec to get to the difference TT - UT
  // which is used in the calculations here

  eb_del_tdut = d + 32.184;
  eb_del_auto = 0;
}

void EclSolar::setAutoTAI_UTC()
{
  // set the difference between TAI and UTC according to the IERS
  eb_del_auto = true;
  eb_del_tdut = DefTdUt(eb_year);
}

void EclSolar::setLocalPos(double lat, double lng, double hgt)
{
  // set the geographic coordinates for the position of the local info
  // latitude lat, longitude lng in decimal degrees
  // height hgt in meters

  eb_geolat = lat;
  eb_geolong = lng;
  eb_geoheight = hgt;

  eb_local_called = false;
}

int EclSolar::getLocalVisibility(double &mjd_start, double &mjd_stop)
{
    // local start and stop times (MJD) for eclipse
    // RETURN 0 if not locally visible

    char wbuf[700];

    if (!eb_local_called) getLocalDetails(wbuf);

    mjd_start = eb_lcb1;
    mjd_stop = eb_lce1;

    return eb_lccnt;
}

int EclSolar::getLocalTotal(double &mjd_start, double &mjd_stop)
{
    // local start and stop times (MJD) for totality/annularity
    // RETURN 0 if not locally visible

    int k, j;
    char wbuf[700];

    if (!eb_local_called) getLocalDetails(wbuf);

    mjd_start = 0;
    mjd_stop = 0;

    if(eb_lccnt == 0) return 0;

    k = 0;
    if(eb_lunactive)
    {
        for (j=0; j<eb_nphase; j++)
        {
           if ((k==0) && (eb_spp[j] > 3))
           {
               mjd_start = eb_spt[j];
               mjd_stop  = eb_ept[j];
               k = 1;
           }
        }

        if(mjd_start < eb_lcb1) mjd_start = eb_lcb1;
        if(mjd_start > eb_lce1) k = 0;
        if(mjd_stop > eb_lce1) mjd_stop = eb_lce1;
        if(mjd_stop < eb_lcb1) k = 0;

        eb_ltotb = mjd_start;
        eb_ltote = mjd_stop;

    }
    else k = eb_lccnt;

    mjd_start = eb_ltotb;
    mjd_stop = eb_ltote;

    return k;
}

int EclSolar::getLocalMax(double &mjdmax, double &magmax, double &elmax)
{
// get local (solar) eclipse maximum
// mjdmax : Modified Julian Date of maximum eclipse
// magmax : maximum (local) magnitude of eclipse
// elmax : local Sun elevation at maximum

// RETURN : 0 if eclipse not visible

int k;

    mjdmax = 0;
    magmax = 0;
    elmax = 0;

    if (eb_lunactive) return 0;

    k = getLocalVisibility(mjdmax, magmax);

    if (k != 0)
    {
     mjdmax = eb_jdmaxps;
     magmax = eb_maxps;
     elmax = eb_maxelv;
    }

    return k;
}

int EclSolar::getPenumbra(double &mjd_start, double &mjd_stop)
{
    // start and stop times (MJD) for penumbral eclipse of Moon
    // RETURN 0 if no lunar eclipse

    int j, k;

    if (!eb_start_called) eclStart();

    mjd_start = 0;
    mjd_stop = 0;

    if (!eb_lunactive) return 0;

    if (eb_nphase <= 0) return 0;

    k = 0 ;
    for (j=0; j<eb_nphase; j++)
    {
        if (eb_spp[j] == 1)
        {
            mjd_start = eb_spt[j];
            mjd_stop  =eb_ept[j];
            k = 1;
        }
    }

    return k;
}

int EclSolar::getPartial(double &mjd_start, double &mjd_stop)
{
    // (global) start and stop times (MJD) for partial phase
    // RETURN 0 if no lunar eclipse

    int j, k;

    if (!eb_start_called) eclStart();

    mjd_start = 0;
    mjd_stop = 0;

    if (eb_nphase <= 0) return 0;

    k = 0;

    if(eb_lunactive)
    {
        for (j=0; j<eb_nphase; j++)
        {
           if ((k==0) && (eb_spp[j] == 3))
           {
               mjd_start = eb_spt[j];
               mjd_stop  =eb_ept[j];
               k = 1;
           }
        }
    }
    else
    {
     for (j=0; j<eb_nphase; j++)
     {
        if ((k==0) && (eb_spp[j] == 1))
        {
            mjd_start = eb_spt[j];
            mjd_stop  =eb_ept[j];
            k = 1;
        }
     }
    }

    return k;

}

int EclSolar::getTotal(double &mjd_start, double &mjd_stop)
{
    // (global) start and stop times (MJD) for totality/annularity
    // RETURN 0 if no lunar eclipse

    int j, k;

    if (!eb_start_called) eclStart();

    mjd_start = 0;
    mjd_stop = 0;

    if (eb_nphase <= 0) return 0;

    k = 0;

    if(eb_lunactive)
    {
        for (j=0; j<eb_nphase; j++)
        {
           if ((k==0) && (eb_spp[j] > 3))
           {
               mjd_start = eb_spt[j];
               mjd_stop  =eb_ept[j];
               k = 1;
           }
        }
    }
    else
    {
     for (j=0; j<eb_nphase; j++)
     {
        if ((k==0) && (eb_spp[j] > 1))
        {
            mjd_start = eb_spt[j];
            mjd_stop  =eb_ept[j];
            k = 1;
        }
     }
    }

    return k;

}

void EclSolar::setCurrentMJD(int year, int month, int day, int hour, int min, double sec)
{
    // set the (MJD-) time currently used for calculations to year, month, day, hour, min, sec
    // corrected for timezone

    double jd;

    jd = ddd(hour, min, sec) - eb_tzone;
    jd = mjd(day, month, year, jd);

    eb_lastjd = jd;

}

void EclSolar::getDatefromMJD(double mjd, int &year, int &month, int &day, int &hour, int &min, double &sec) const
{
    // convert times given in Modified Julian Date (MJD) into conventional date and time
    // correct for timezone

    double magn;

    caldat((mjd + eb_tzone/24.0), day, month, year, magn);
    dms (magn,hour,min,sec);
    if (sec>30.0) min++;;
    if (min>59)
     {
      hour++;
      min=0;
     };

}

//---------------------- getEclYearInfo--------------------------------

void EclSolar::getEclYearInfo(char* wbuf)
 {
  // output the eclipse dates in buffer wbuf accurate to the minute.

  char dts[13];
  char outbuf[127];
  char magbuf[30];
  int j, p, kecl;

  if (!eb_moonph_called) moonph();  // calculate the eclipses of the year

  sprintf(wbuf,"Solar Eclipses for %4i  UTC +%4.1f", eb_year, eb_tzone);

  kecl = 1;
  for (j=0; j<eb_numecl; j++)
   {
    sprintf(dts,"%1i : ", kecl);
    strcpy (outbuf,dts);
    dtmstr((eb_eclmjd[j] + eb_tzone/24.0),dts);
    dts[12] = '\0';
    strcat (outbuf,dts);
    p = eb_phase[j];

    switch (p)
     {
      case 1: strcat(outbuf,"\t Partial Sun");
              sprintf(magbuf,"  (magnitude:%5.2f)",eb_magnitude[j]);
              strcat(outbuf,magbuf);
              break;

      case 2: strcat(outbuf,"\t non-central Annular Sun");
              break;
      case 4: strcat(outbuf,"\t Annular Sun");
              break;

      case 3: strcat(outbuf, "\t non-central Total Sun");
              break;
      case 5: strcat(outbuf, "\t Total Sun");
              break;

      case 6: strcat(outbuf, "\t Annular/Total Sun");
              break;

      case -1:
      case -2: strcat(outbuf, "\t Penumbral Moon");
               sprintf(magbuf,"  (magnitude:%5.2f)",eb_magnitude[j]);
               strcat(outbuf,magbuf);
               break;

      case -3: strcat(outbuf, "\t Partial Moon");
               sprintf(magbuf,"  (magnitude:%5.2f)",eb_magnitude[j]);
               strcat(outbuf,magbuf);
               break;

      case -4: strcat(outbuf, "\t Total Moon");
               sprintf(magbuf,"  (magnitude:%5.2f)",eb_magnitude[j]);
               strcat(outbuf,magbuf);
               break;
     };

    if ((p > 0) || eb_lunecl)  // solar eclipse only or also lunar eclipses
     {
      strcat (wbuf, "\n");
      strcat (wbuf, outbuf);
      kecl++;
     };
   }
 }

int EclSolar::getEclYearInfo(int k, int &yr, int &month, int &day,
                             int &hour, int &min, double &sec, double &tzone, double &magn)
{
  /* output the eclipse info for k-th eclipse

     year, month, day, hour, minutes, seconds and timezone
     magn = magnitude of eclipse

   RETURN: phase of eclipse. 0 if no k-th eclipse

      1: Partial Sun
      2: non-central Annular Sun
      3: non-central Total Sun
      4: Annular Sun
      5: Total Sun
      6: Annular/Total Sun

     -1, -2: Penumbral Moon
     -3: Partial Moon
     -4: Total Moon

 */
  bool nls;
  int j, p, kecl;

  nls = true;

  if (!eb_moonph_called) moonph();  // calculate the eclipses of the year

  if (k < 1)
  {
      k = eb_eclselect;  // select current eclipse as default
      nls = false;
  };
  if ((k < 1) && (k > eb_numecl)) return 0;

  p = k - 1;
  if (nls && (!eb_lunecl))
  {
   kecl = 0;
   p = -1;
   for(j=0; j<eb_numecl; j++)
   {
       if (eb_phase[j] > 0)
       {
           kecl++;
           if (kecl == k) p = j;
       };
   };
  };

  if (p < 0) return 0;

  j = p;

  // convert MJD into corresponding date and time
  caldat((eb_eclmjd[j] + eb_tzone/24.0), day, month, yr, magn);
  dms (magn,hour,min,sec);
  if (sec>30.0) min++;;
  if (min>59)
   {
    hour++;
    min=0;
   };

  magn = eb_magnitude[j];
  tzone = eb_tzone;

  return eb_phase[j];
}

int EclSolar::getEclTxt (int k, char* jtxt)
 {
  // get the data / kind of eclipse info for the j-th eclipse
  // and place it into jtxt

    // RETURN : the phase of the eclipse. 0 if no j-th eclipse

  int p, j;
  char dts[13];

  if (!eb_moonph_called) moonph();  // calculate the eclipses of the year

  if (k < 1) k = eb_eclselect;  // select current eclipse as default
  if ((k < 1) && (k > eb_numecl)) return 0;

  j = k-1;

  sprintf(jtxt, "%2i :", (j+1));
  sprintf(dts, "%5i ", eb_year);
  strcat (jtxt, dts);
  dtmstr((eb_eclmjd[j]+eb_tzone/24.0),dts);
  dts[6] = '\0';
  strcat (jtxt,dts);

  p = eb_phase[j];
  switch (p)
   {
    case 1: strcat(jtxt," Par.Sun");
            break;
    case 2: strcat(jtxt, " non-centr.Ann.Sun");
            break;
    case 4: strcat(jtxt," Ann.Sun");
            break;
    case 3: strcat(jtxt," non-centr.Tot.Sun");
            break;
    case 5: strcat(jtxt," Tot.Sun");
            break;
    case 6: strcat(jtxt," Ann/Tot.");
            break;

    case -1:
    case -2: strcat(jtxt," Pen.Moon");
             break;
    case -3: strcat(jtxt," Par.Moon");
             break;
    case -4: strcat(jtxt," Tot.Moon");
             break;
   };

  return p;
 }

// ----------------------------- Select Eclipse -------------------------

void EclSolar::putEclSelect(int es)
{
 // store the number of the eclipse selected for detailed calculations
 int k, j;

 if (!eb_moonph_called) moonph();  // calculate the eclipses of the year

 k = 0;

 eb_lunactive = false;
 eb_eclselect = 1;
 for (j=0; j<eb_numecl; j++)
  {
   if ((eb_phase[j] > 0) || eb_lunecl)  // only solar eclipses if set
    {
     k++;
     if (k == es)
     {
         eb_eclselect = j + 1;
         if (eb_phase[j] < 0) eb_lunactive = true;
     };
    };
  };
 eb_start_called = false;
}

void EclSolar::nextEcl()
{
 // select the next eclipse for detailed calculations
 int k, j, es;

 if (!eb_moonph_called) moonph();  // calculate the eclipses of the year
 eb_start_called = false;

 k = eb_eclselect + 1;
 if (k > eb_numecl)
 {
     k = eb_year + 1;
     putYear (k);
     k = 1;
     putEclSelect(k);

     return;
 };

 if (eb_lunecl)  // easier when lunar eclipses are included
 {
     putEclSelect(k);

     return;
 };

 eb_lunactive = false;

 es = eb_eclselect;
 k = 0;
 for (j=es; j<eb_numecl; j++)
 {
    if ((k == 0) && (eb_phase[j] > 0)) k = j + 1;
 };

 if(k > 0)
 {
    eb_eclselect = k;
    return;
 }

 k = eb_year + 1;
 putYear (k);
 es = 1;
 putEclSelect(es);

}

void EclSolar::previousEcl()
{
 // select the prevoius eclipse for detailed calculations
 int k, j, es;

 if (!eb_moonph_called) moonph();  // calculate the eclipses of the year
 eb_start_called = false;

 k = eb_eclselect - 1;

 if (k <= 0)
 {
     k = eb_year - 1;
     putYear (k);
     k = eb_numecl;
 };

 if (eb_lunecl)  // easier when lunar eclipses are included
 {
     putEclSelect(k);

     return;
 };

 eb_lunactive = false;

 es = 0;
 k--;
 for (j=k; j>=0; j--)
 {
     if ((es == 0) && (eb_phase[j] > 0)) es = j + 1;
 };

 if(es > 0) eb_eclselect = es;
 else putEclSelect(0);  // this case should never happen!

}

double EclSolar::getLastMJD() const
{
 // RETURN the MJD last used in calculations

    return eb_lastjd;
}

void EclSolar::setPenumbraAngle(double pa, int mode)
{
    /* set the Penumbra Angle
       if mode == 0 the angle will be set to pa-times the maximum angle
       (pa < 1.0). Set pa = 1 for the normal penumbra boundaries

       if mode == 1 the angle will be set such that the penumbra line
       markes magnitude pa. pa == 0 will mark normal penumbra boundaries

       if mode == 2 the angle will be set such that the penumbra line
       markes the obscuration pa. pa == 0.5 will mean that 50% of the Sun's
       disk is covered by the Moon etc.
    */

    double mjd, dpn1, dpn2, pan1, pan2;
    double mag, m1, m2, s, ps;
    int j;
    Vec3 ubm, ube;
    Eclipse eclp;

    if (mode == 0)
    {
        eb_penangle = pa;
        eb_penamode = 0;
        if (pa > 1.0) eb_penangle = 1.0;
        if (pa < 0) eb_penangle = 1.0;

        return;
    }

    if (!eb_start_called) eclStart();

    if (mode == 1)
    {
        eb_penamode = 1;
        mjd = eb_eclmjd[eb_eclselect-1];
        eclp.umbra (mjd, eb_del_tdut, ubm, ube, dpn1, pan1);
        eclp.penumd (mjd, eb_del_tdut, ubm, ube, dpn2, pan2);

        if(dpn2 > 0)
        {
            eb_penangle = fabs(pa);
            if(eb_penangle > 1.0) eb_penangle = 1.0;
            eb_penangle = 1.0 - eb_penangle * (1.0 + dpn1 / dpn2);
        }
        else eb_penangle = 1.0;

        return;
    }

    if (mode == 2)
    {
        eb_penamode = 2;
        mjd = eb_eclmjd[eb_eclselect-1];
        eclp.umbra (mjd, eb_del_tdut, ubm, ube, dpn1, pan1);
        eclp.penumd (mjd, eb_del_tdut, ubm, ube, dpn2, pan2);

        ps = pa;  // find the magnitude that corresponds to the obscuration
        if (ps > 1.0) ps = 1.0;
        if (ps < 0) ps = 0;
        for (j=1; j<11; j++)
        {
            mag = double(j) * 0.1;
            s = sunObscure(dpn2, dpn1, mag);
            if (s >= ps) break;
        }

        m1 = mag - 0.1;
        m2 = mag;
        for (j=0; j<8; j++)  // 8 iterations should be plenty
        {
           mag = (m2 + m1) * 0.5;
           s = sunObscure(dpn2, dpn1, mag);
           if (s > ps) m2 = mag;
           else m1 = mag;
        }

        if(dpn2 > 0)
        {
            eb_penangle = fabs(mag);
            if(eb_penangle > 1.0) eb_penangle = 1.0;
            eb_penangle = 1.0 - eb_penangle * (1.0 + dpn1 / dpn2);
        }
        else eb_penangle = 1.0;

        return;
    }

    eb_penamode = 0;
    eb_penangle = 1.0;

}

// -------- auxilliary functions --------------------------------
void EclSolar::GetMonth (int mm, char* mchr)
 {
  // get three letter designation of month
  // mm is the number of the month
  // mchr is a char[4] array to receive the three letters of the month

  switch (mm)
   {
    case 1 : strcpy(mchr,"JAN"); break;
    case 2 : strcpy(mchr,"FEB"); break;
    case 3 : strcpy(mchr,"MAR"); break;
    case 4 : strcpy(mchr,"APR"); break;
    case 5 : strcpy(mchr,"MAY"); break;
    case 6 : strcpy(mchr,"JUN"); break;
    case 7 : strcpy(mchr,"JUL"); break;
    case 8 : strcpy(mchr,"AUG"); break;
    case 9 : strcpy(mchr,"SEP"); break;
    case 10 : strcpy(mchr,"OCT"); break;
    case 11 : strcpy(mchr,"NOV"); break;
    case 12 : strcpy(mchr,"DEC"); break;
    default: strcpy(mchr,"ERR");
   };
}

double EclSolar::phmjd (double yearf, double phase, double tdut,
				  int& eph, double& ejd, double& emag)
  /*
    Calculate the Modified Julian Date of the occurrence of the specified
    phase of the Moon and check for possible eclipses.
    yearf : year.fraction of date close to the Moon phase
    phase : 0    for New Moon,
            0.25 for First Quarter,
            0.5  for Full Moon,
            0.75 for Last Quarter.
    tdut = TDT - UT in sec

    RETURN: the MJD of the lunar phase event

    OUTPUT:
    eph : phase of the eclipse on that date
          0 = no eclipse, 1 = partial solar eclipse,
          2 = non-central annular, 3 = non-central total
          4 = annular solar, 5 = total solar,
          6 = annular/total solar, -1 =partial penumbral lunar,
          -2 = total penumbral lunar, -3 = partial umbral lunar,
          -4 = total umbral lunar
    ejd : Modified Julian Date of the maximum of the eclipse (if any)
    emag : magnitude of the eclipse (if any)
  */
 {
  double tt, jd, k, m, p, f;
  double s, c, gam, u;
  int tst;
  Eclipse eclp;

  // preliminary (modified) Julian Date 
  k = floor ((yearf - 1900.0) * 12.3685) + phase;
  tt = k / 1236.85;
  jd = 166.56 + (132.87 - 0.009173*tt)*tt;
  jd = 15020.25933 + 29.53058868*k
                   + (0.0001178 - 0.000000155*tt)*tt*tt
                   + 0.00033 * sin (degrad*jd);

  eph = 0;   // assume no eclipse

  // Sun's mean anomaly 
  m = degrad * (359.2242 + 29.10535608*k
               - (0.0000333 - 0.00000347*tt)*tt*tt);
  // Moon's mean anomaly 
  p = degrad * (306.0253 + 385.81691808*k
              + (0.0107306 + 0.00001236*tt)*tt*tt);
  // 2* Moon's argument of latitude 
  f = 2.0 * degrad * (21.2964 + 390.67050646*k
                   - (0.0016528 - 0.00000239*tt)*tt*tt);

  if ((phase == 0) || (phase == 0.5))  // for Full and New Moon 
   { 
    k = (0.1734 - 0.000393*tt) * sin (m)
        + 0.0021 * sin (2.0 * m)
        - 0.4068 * sin (p)
        + 0.0161 * sin (2.0 * p)
        - 0.0004 * sin (3.0 * p)
        + 0.0104 * sin (f)
        - 0.0051 * sin (m + p)
        - 0.0074 * sin (m - p)
        + 0.0004 * sin (f + m)
        - 0.0004 * sin (f - m)
        - 0.0006 * sin (f + p)
        + 0.0010 * sin (f - p)
        + 0.0005 * sin (m + 2.0*p);

    // check for possible eclipses.
    f = f / 2.0;
    if (fabs(sin(f)) <= 0.36)   // eclipses are possible
     {
      ejd = (0.1734 - 0.000393*tt) * sin(m)
            + 0.0021 * sin(2.0*m)
            - 0.4068 * sin(p)
            + 0.0161 * sin(2.0*p)
            - 0.0051 * sin(m+p)
            - 0.0074 * sin(m-p)
            - 0.0104 * sin(2.0*f);
      ejd += jd;

      s = 5.19595 - 0.0048*cos(m) + 0.0020*cos(2.0*m) - 0.3283*cos(p)
                  - 0.0060*cos(m+p) + 0.0041*cos(m-p);

      c = 0.2070*sin(m) + 0.0024*sin(2.0*m) - 0.0390*sin(p)
                        + 0.0115*sin(2.0*p) - 0.0073*sin(m+p)
                        - 0.0067*sin(m-p) + 0.0117*sin(2.0*f);

      gam = s*sin(f) + c*cos(f);
      u = 0.0059 + 0.0046*cos(m) - 0.0182*cos(p)
                 + 0.0004*cos(2.0*p) - 0.0005*cos(m+p);

      if (phase == 0)          // check for solar eclipse
       {
        if (fabs(gam) <= (1.5432+u))
         {
          if (fabs(gam) < 0.9972)    // central eclipse
           {
            emag = 1.0;
            if (u < 0) eph = 5;  // total eclipse
            else
             {
              if (u > 0.0047) eph = 4;  // annular eclipse
              else
               {
                if (u < (0.00464*cos(asin(gam)))) eph = 6; // annular/total
                else eph = 4;  // annular
               }; // end if u>0.0047
             }; // end if u<0
           }  // end if fabs(gam) if
          else      // non-central eclipse
           {
            eph = 1;   // assume partial eclipse
            if (fabs(gam) < (0.9972+fabs(u))) // non-central annular or total
             {
              eph = eclp.solar(ejd,tdut,s,c);
              emag = 1.0;
             };
            if (eph == 1)     // get magnitude of partial eclipse
                emag = (1.5432 + u - fabs(gam)) / (0.5460 + 2.0*u);
            if (emag < 0.025) // check if low mag eclipse is OK
             {
              eph = 0;
              u = 1.0 / 720; // 2 min steps
              for (int j=0; j < 288; j++)
               {
                tt = ejd - 0.2 + double(j)*u;
                tst = eclp.solar(tt,tdut,s,c);
                if (tst > 0) eph = tst;
               };
             };
           }
         }
       } // end of solar eclipse check

      if (phase == 0.5)        // check for lunar eclipse
       {
        c = (1.5572 + u - fabs(gam)) / 0.5450;
        if (c > 0)
         {
          s = (1.0129 - u - fabs(gam)) / 0.5450;
          if (s < 0)   // penumbral eclipse
           {
            emag = c;
            if (emag > 1) eph = -2;
            else eph = -1;
           }
          else         // umbral eclipse
           {
            emag = s;
            if (emag > 1) eph = -4;
            else eph = -3;
           }
         }
       }
     }
   };

  if ((phase == 0.25) || (phase == 0.75)) // for first and last quarter 
   { 
    k = (0.1721 - 0.0004*tt) * sin (m)
        + 0.0021 * sin (2.0 * m)
        - 0.6280 * sin (p)
        + 0.0089 * sin (2.0 * p)
        - 0.0004 * sin (3.0 * p)
        + 0.0079 * sin (f)
        - 0.0119 * sin (m + p)
        - 0.0047 * sin (m - p)
        + 0.0003 * sin (f + m)
        - 0.0004 * sin (f - m)
        - 0.0006 * sin (f + p)
        + 0.0021 * sin (f - p)
        + 0.0003 * sin (m + 2.0*p)
        + 0.0004 * sin (m - 2.0*p)
        - 0.0003 * sin (2.0*m + p);
    if (phase == 0.25)
     { 
      k += 0.0028 - 0.0004*cos(m) + 0.0003*cos(p);
     };
    if (phase == 0.75)
     { 
      k += -0.0028 + 0.0004*cos(m) - 0.0003*cos(p);
     };
   };

  jd = jd + k;

  return jd;
 }

void EclSolar::ckphase (double minmjd, double maxmjd, double yr,
              double deltdut, int &mp, PMJD p, double phase)
  /* calculate the respective phase for one (MJD-) date and check whether
     this date is within the limits given by minmjd and maxmjd.
     Use yr as the year.fraction close to the desired date.
     If the date is within the limits store the result in the
     respective array.
     Also check for possible occurrences of eclipses and store the
     respective data in an array
  */
 {
  double td, ejd, emag;
  int j, eph;

  td = phmjd (yr, phase, deltdut*86400.0, eph, ejd, emag);
  // correct difference between UT and TDT 
  td = td - deltdut;  // correct for difference between TDT and UT.

  // check whether the date is within the respective year 

  if ((td >= minmjd) &&  (td <= maxmjd) && (mp < MAXLUN))
   {
    if (mp==0)
     {
      p[0]=td;
      mp=1;
     }
    else
     {
      if (p[mp-1] < (td - 0.1))
       {
        p[mp]=td;
        mp=mp+1;
       };
     };
   };

  // now the eclipses 
  if (eph != 0)
   {
    td = ejd;
    td = td - deltdut;  // correct for difference between TDT and UT.

    // check whether the date is within the respective year 
    if ((td >= minmjd)&&(td <= maxmjd)&&(eb_numecl<GBL_ECLBUF))
     {
      if (eb_numecl == 0)
       {
        eb_eclmjd[0] = td;
        eb_magnitude[0] = emag;
        eb_phase[0] = eph;
        eb_numecl = 1;
       }
      else
       {
        for (j=0; j<eb_numecl; j++)
         {          // check whether the date is already stored
          if (fabs(eb_eclmjd[j]- td) < 0.01) eph = 0;
         };
        if (eph != 0)
         {
          j = eb_numecl;
          eb_eclmjd[j] = td;
          eb_magnitude[j] = emag;
          eb_phase[j] = eph;
          eb_numecl += 1;
         };
       }
     }
   }
 }

void EclSolar::dtmstr(double jdmoon, char *dts)
//  Convert the Modified Julian Date jdmoon into a corresponding string
//  *dts which has the format "MMM DD HH:MM"
{
  int dd, mm, yy, deg, mnt;
  double hh, sec;
  char mchr[4];

  // convert MJD into corresponding date and time 
  caldat(jdmoon, dd, mm, yy, hh);
  dms (hh,deg,mnt,sec);
  if (sec>30.0) {mnt++;};
  if (mnt>59)
   {
    deg++;
    mnt=0;
   };

  // store data in their positions 
  GetMonth (mm, mchr);
  dts[0]=mchr[0];
  dts[1]=mchr[1];
  dts[2]=mchr[2];
  dts[3]=' ';
  sprintf(&dts[4],"%2i %2i:%02i", dd, deg, mnt);
  dts[12]=' ';
}

//------------------------- moonph ---------------------------------

void EclSolar::moonph()
{
  int mp0, mp25, mp5, mp75;  // counter of respective phase entries 
  PMJD p0, p25, p5, p75;
  int day, month, year, j;
  double yr, yx, hour, deltdut;
  double minmjd, maxmjd;
  double glatsv, glongsv, gheightsv, lat, lng;
  char wbuf[700];

  eb_moonph_called = true;

// assign input data from Window Input Structure
  year = eb_year;
  deltdut = eb_del_tdut / 86400.0;  // difference TDT - UT in days
  eb_lastyear = year;
  eb_lasttz = eb_tzone;
  eb_lastdlt = eb_del_tdut;

// initializing counters
  eb_numecl = 0;
  mp0 = 0;
  mp25 = 0;
  mp5 = 0;
  mp75 = 0;

  yr = year - 0.2;
  yx = year + 1.2;
  day = 1;
  month = 1;
  hour = 0;
  minmjd = mjd(day, month, year, hour);
  day = 31;
  month = 12;
  hour = 24.0;
  maxmjd = mjd(day, month, year, hour);

  /* As the function phmjd finds the respective phases of the Moon
     only to within a few weeks of the time given by the input
     parameter (year.faction) the following loop starts well ahead of
     the beginning of the year in question and ends well after.
     The time step of two weeks utilized to increment the year.fraction
     parameter assures that we catch all the different lunations */

  while (yr < yx)
   {                      // calculate and check the phases 
    ckphase(minmjd,maxmjd,yr,deltdut, mp0, p0, 0.0);
    ckphase(minmjd,maxmjd,yr,deltdut, mp25, p25, 0.25);
    ckphase(minmjd,maxmjd,yr,deltdut, mp5, p5, 0.5);
    ckphase(minmjd,maxmjd,yr,deltdut, mp75, p75, 0.75);
    yr+=0.02;  // incease by 1/50th of a year (about two weeks) 
   };

  for (j=0; j<eb_numecl; j++)
  {
      if( (eb_phase[j] == 1) && (eb_magnitude[j] > 0.98))  // check for possible non-central eclipse
      {
          glatsv = eb_geolat;
          glongsv = eb_geolong;
          gheightsv = eb_geoheight;
          eb_eclselect = j+1;
          eb_start_called = false;
          eb_local_called = false;

          getMaxPos (lat, lng);
          eb_geolat = lat;
          eb_geolong = lng;
          eb_geoheight = 0;

          getLocalDetails(wbuf);

          if ((eb_spp[0] == 2) || (eb_spp[1] == 2)) eb_phase[j] = 2;
          if ((eb_spp[0] == 3) || (eb_spp[1] == 3)) eb_phase[j] = 3;

          eb_geolat = glatsv;
          eb_geolong = glongsv;
          eb_geoheight = gheightsv;
          eb_start_called = false;
          eb_local_called = false;

      }

  };

  eb_eclselect = 0;
}

double EclSolar::getlocmag(double jd, double ep2, double phi, double lamda,
                 double height, Vec3 rs, Vec3 rm, int& totflg)
 {
  /* get magnitude of solar eclipse at local position.
     jd : MJD (UT) of event
     ep2 : correction for apparent sidereal time (in sec)
     phi, lamda : latitude and longitude of observer in radians
     height : height of observer in m
     rs, rm : geocentric position vector of the sun and the moon
     totflg : 1 if total or annular, 0 otherwise

     RETURN: The magnitude of the eclipse (0 if no eclipse)
  */ 

  const double ds = 218.245445;  // diameter of Sun in Earth radii
  const double dm = 0.544986;   // diameter of Moon in Earth radii
  Vec3  gm, gs, s;
  double dsun, dmoon, asep;
  double elev;

  gm = GeoPos(jd, ep2, phi, lamda, height);
  gs = rs - gm;
  gm = rm - gm;

  // correct for refraction
  s = EquHor(jd, ep2, phi, lamda, gs);
  s = carpol(s);
  elev = s[2];

  if (elev > -3.5e-2)   // cutoff of -2 deg for calculating refraction
   {
    elev = Refract(elev);
    s[2] = s[2] + elev;
    s = polcar (s);
    gs = HorEqu(jd, ep2, phi, lamda, s);
   };

  s = EquHor(jd, ep2, phi, lamda, gm);
  s = carpol(s);
  elev = s[2];
  if (elev > -3.5e-2)   // cutoff of -2 deg for calculating refraction
   {
    elev = Refract(elev);
    s[2] = s[2] + elev;
    s = polcar (s);
    gm = HorEqu(jd, ep2, phi, lamda, s);
   };

  dsun = atan(0.5*ds/abs(gs)); // apparent radius of the Sun
  dmoon = atan(0.5*dm/abs(gm)); // apparent radius of the Moon

  gs = vnorm(gs);
  gm = vnorm(gm);
  asep = fabs(dot(gs, gm));
  if (asep > 1.0) asep = 1.0;
  asep = acos(asep);    // apparent distance between Sun and Moon

  totflg = 0;
  if ((dsun+dmoon) > asep)  // we have an eclipse
   {
    if (fabs(dsun-dmoon) > asep) totflg = 1;
    asep = fabs(dsun + dmoon - asep) / (2.0 * dsun);
   }
  else asep = 0;

  return asep;
 }

//------------------------- eclStart ---------------------------------

void EclSolar::eclStart()
{
  /* get start and end times of the various phases of the eclipse
     j = index of the eclipse
     eb_spt[i] = start time in MJD for phase i
     eb_ept[i] = end time in MJD for phase i
     eb_spp[i] = kind of phase i

     Also set the global eb_jdstart and eb_jdstop
     to the (global jd times) of eclipse start and end.

   */
  int nump, eflg, pcur, pold, maxp, i, j, npflg, p;
  double jd, step, jd2, jdf, d1, d2;
  double azim, elev, dist, phi, lamda;
  bool eclstarted;
  Vec3 gx;
  Eclipse eclp;

  if (!eb_moonph_called)  // just in case - this should never happen!
  {
      moonph();
      putEclSelect(1);
  };

  eb_local_called = false;
  eb_start_called = true;

  j = eb_eclselect-1;

  eclstarted = false;
  nump = 0;
  maxp = 0;
  eflg = 0;  // end flag
  pold = 0;
  elev = 0;
  eb_maxps = -1.0;
  eb_maxelv = -1.0;
  p = eb_phase[j];
  jd = eb_eclmjd[j] - 0.5;  // start 12 hours before maximum
  jdf = jd + 1.5;  // emergency stop if something goes wrong with the loop
  step = eb_cstep / (24.0*60.0);  // stepwidth (best set to 1 minute)

  do
   {
     if (eb_lunactive) pcur = eclp.lunar(jd, eb_del_tdut);
     else pcur = eclp.solar (jd, eb_del_tdut,d1, d2);
    // now check the start and stop times for the phases
    if (pcur > pold)
     {
      eclstarted = true;
      if(nump < 4)
       {
        eb_spt[nump] = jd;
        eb_spp[nump] = pcur;
        nump++;
        maxp++;
        pold = pcur;
        // get time accurate to the second
        jd2 = jd - 1.0/86400.0;  // go in seconds steps
        for (i=0; i<60; i++)
        {
            if (eb_lunactive) pcur = eclp.lunar(jd, eb_del_tdut);
            else pcur = eclp.solar (jd2, eb_del_tdut,d1, d2);

            if (pcur == pold) eb_spt[nump] = jd2;
            else break;
            jd2 = jd2 - 1.0/86400.0;  // go in seconds steps
        };
       }
     }
    else if (eclstarted && (pcur < pold))
     {
        pold = pcur;
        // get time accurate to the second
        jd2 = jd - 1.0/86400.0;  // go in seconds steps
        for (i=0; i<60; i++)
        {
            if (eb_lunactive) pcur = eclp.lunar(jd, eb_del_tdut);
            else pcur = eclp.solar (jd2, eb_del_tdut,d1, d2);
            if (pcur == pold) jd = jd2;
            else break;
            jd2 = jd2 - 1.0/86400.0;  // go in seconds steps
        };
        pcur = pold;

      npflg = 1;
      if (nump > 1)
       {
        if (pcur > eb_spp[nump-2]) npflg = 0;
       }
      if (npflg)
       {
        nump--;
        if (nump >= 0) eb_ept[nump] = jd;
        if (nump > 1)
         {
          if (pcur < eb_spp[nump-1])
           {
            nump--;
            eb_ept[nump] = jd;
           }
         }
        if (nump <= 0) eflg = 1;
       }
     };

    jd += step;
    if (jd > jdf) eflg = 1;

   } while (eflg != 1);

  // now check for maximum eclipse conditions

  calcMaxPos(phi, lamda);

  if ((eb_lunactive == false) && (p > 3))  // central solar eclipse
  {
      eb_jdmaxps = eb_eclmjd[j];
      jd = eb_eclmjd[j] - 600.0/86400.0;  // start 10 min before approximate max time
      phi = eb_cmxlat * degrad;
      lamda = eb_cmxlng * degrad;
      for (i=0; i<1200; i++)
      {
          jd += 1.0/86400.0;  // go in seconds steps
          pcur = eclp.solar (jd, eb_del_tdut,d1, d2);
          if (pcur > 3)
          {
              gx = eclp.GetRSun();
              AppPos (jd, eclp.GetEp2(), d1, d2, 0.0, 1, gx, azim, elev, dist);

              if (elev > eb_maxelv)
              {
                eb_maxelv = elev;
                eb_jdmaxps = jd;
                phi = d1;
                lamda = d2;
               };
          }
      }
      eb_maxelv = elev / degrad;
      eb_cmxlat = phi / degrad;
      eb_cmxlng = lamda / degrad;
      if (eb_cmxlng < 0) eb_cmxlng += 360.0;
  };

  eb_jdstart = eb_spt[0];
  eb_jdstop = eb_ept[0];
  eb_nphase = maxp;

 }

void EclSolar::calcMaxPos(double &lat, double &lng)
 {
  // Get the geographic position of the maximum eclipse
  // in case of a central eclipse the position is approximate
  // lat and lng are in decimal degrees

  int j, p;
  double t, mp2;
  Vec3 rm;
  Vec3 s2;
  Eclipse eclp;

  mp2 = 2.0 * M_PI;

  j = eb_eclselect-1;

  t = eb_eclmjd[j];

  if (eb_lunactive)
  {
      eclp.lunar(t, eb_del_tdut);
      rm = eclp.GetRMoon();

      // get sub lunar point at maximum
      s2 = carpol(rm);
      lat = s2[2];   // just preliminary
      lng = s2[1] - lsidtim (t, 0, eclp.GetEp2())*M_PI/12.0;
      if (lng > mp2) lng -= mp2;
      if (lng < -M_PI) lng += mp2;
      if (lng > M_PI) lng -= mp2;
      if (fabs(lat) < 1.53589) lat = atan(1.00674*tan(lat));
      lat /= degrad;
      lng /= degrad;
      if (lng < 0) lng += 360.0;
      eb_cmxlat = lat;
      eb_cmxlng = lng;
      return;
  }
  else p = eclp.solar(t,eb_del_tdut,lat,lng);

  if (p > 3)
   {
    eb_cmxlat = lat;
    eb_cmxlng = lng;
    eb_cmxlat /= degrad;
    eb_cmxlng /= degrad;
   }
  else
   {
    eclp.maxpos(t,eb_del_tdut,eb_cmxlat,eb_cmxlng);
    eb_cmxlat /= degrad;
    eb_cmxlng /= degrad;
   };

  if (eb_cmxlng < 0) eb_cmxlng += 360.0;
  lat = eb_cmxlat;
  lng = eb_cmxlng;
 }

void EclSolar::getMaxPos(double &lat, double &lng)
 {
  // Get the geographic position of the maximum eclipse

  if (!eb_start_called) eclStart();

  lat = eb_cmxlat;
  lng = eb_cmxlng;
 }

double EclSolar::sunObscure(double l1, double l2, double mag)
{
    // get the Obscuration of the Sun from penumbra l1, umbra l2 and
    // magnitude mag

    double s, a, b, c, m;

    m = l1 - mag * (l1 + l2);
    s = (l1 - l2) / (l1 + l2);
    c = (l1*l1 + l2*l2 - 2*m*m) / (l1*l1 - l2*l2);
    c = acos(c);
    b = (l1*l2 + m*m) / (m*(l1+l2));
    b = acos(b);
    a = M_PI - (b + c);
    s = (s*s*a + b) - s * sin(c);

    return s / M_PI;
}

/*------------------------- EclCentral ---------------------------------*/

int EclSolar::eclPltCentral(bool firstc, double &lat, double &lng)
 {
  /* get next coordinates for central eclipse position
     first = 1 for first call (first point will be found)
     lat, lng : latitude and longitude in decimal degrees

     RETURN: phase (= 4 annular, = 5 total, = 6 annular/total
                    0 if there was no central eclipse)
  */
  double phi, lamda, d1, d2, jd, jd2;
  int kp, kp2, i;
  Eclipse eclp;

  if (!eb_start_called) eclStart();

  if (eb_lunactive)  // just in case
   {
    eb_finished2 = true;
    lat = 0.0;
    lng = 0.0;
    return 0;
   };

  eb_cphs = 0;

  if (firstc)   // find the first occurrence compliant with the step width
   {
    jd = eb_jdstart;
    kp = eclp.solar(jd, eb_del_tdut, phi, lamda);

    while ((kp < 4) && (jd < eb_jdstop))
     {
      jd += double(eb_cstep) / (24.0*60.0);
      eb_lastjd = jd;
      kp = eclp.solar(jd, eb_del_tdut, phi, lamda);
     };

    jd2 = jd;
    for (i=0; i<60; i++)  // get it right to the second
    {
        jd2 = jd2 - 1.0/86400.0;  // go in seconds steps
        kp2 = eclp.solar (jd2, eb_del_tdut,d1, d2);
        if (kp2 == kp)
        {
            phi = d1;
            lamda = d2;
        }
        else break;
    };
    eb_finished2 = false;

   }  // end of firstc
  else
   {
    if (eb_finished2) return 0;

    jd = eb_lastjd + double(eb_cstep) / (24.0*60.0);
    eb_lastjd = jd;
    if (jd > eb_jdstop)
    {
        eb_finished2 = true;
        return 0;
    };

    kp = eclp.solar(jd, eb_del_tdut, phi, lamda);

    if (kp <= 3)  // end of central eclipse.
    {
     eb_finished = true;
     for (i=0; i<60; i++)  // get it right to the second
     {
        jd-= 1.0/86400.0;  // go in seconds steps
        kp = eclp.solar (jd, eb_del_tdut, phi, lamda);
        if (kp > 3) break;
     };
    };
   };

  if (kp > 3)   // central eclipse
   {
    eb_cphs = kp;

    phi /= degrad;
    lamda /= degrad;
    if (lamda < 0.0) lamda += 360.0;
    if (lamda > 360.0) lamda -= 360.0;
    eb_clat = phi;
    eb_clng = lamda;
    lat = eb_clat;
    lng = eb_clng;

    //  djd = eclp.duration(jd, indata->del_tdut, width);
   }
  return kp;
 }

//--------------------Northern and Southern Boundaries -------------------------

void EclSolar::InitBound()
 {
  /* Initialize the calculation for the northern and southern boundaries
     of solar eclipses (umbra or penumbra).
     The function EclStart must have been called first to enable InitBound
     to get the right eclipse.
     The calculation is done in Earth radii.

     OUTPUT:
            eb_ubm : Penumbra base vector
            eb_ube : Shadod base vector for upper boundary
            eb_udm : Penumbra base delta vector
            eb_ude : Shadow delta vector for upper boundary
            eb_lbe : Shadod base vector for lower boundary
            eb_lde : Shadow delta vector for lower boundary
  */

//  const double dm = 0.272493;   // radius of Moon in Earth radii
  double dpn1, dpn2, pan1, pan2, s0;
  Vec3 shmv, u2m, u2e;
  Vec3 ax1, ax2;
  Eclipse eclp;

  if (!eb_start_called) eclStart();
  if (eb_lunactive) return;  // only for solar eclipses

  // beginning of the eclipse
  eclp.penumd (eb_jdstart, eb_del_tdut, eb_ubm, eb_ube, dpn1, pan1);

  // end of the eclipse
  eclp.penumd (eb_jdstop, eb_del_tdut, u2m, u2e, dpn2, pan2);

  dpn1 *= 0.5;
  dpn2 *= 0.5;

  if (eb_penamode == 0)
  {
    dpn1 *= eb_penangle;
    pan1 *= eb_penangle;
    dpn2 *= eb_penangle;
    pan2 *= eb_penangle;
  }

  if (eb_penamode > 0)
  {
      s0 = eb_penangle * tan(pan1);
      s0 = atan(s0);
      if (pan1 > 0)
      {
          dpn1 = dpn1 * s0 / pan1;
          pan1 = s0;
      }
      s0 = eb_penangle * tan(pan2);
      s0 = atan(s0);
      if (pan2 > 0)
      {
          dpn2 = dpn2 * s0 / pan2;
          pan2 = s0;
      }
  }

  // get apex of penumbra cone
  pan1 = tan(pan1);
  if (pan1 != 0) dpn1 = dpn1 / pan1;
  else dpn1 = 1.2 * abs(eb_ubm);  // avoid a crash
  pan2 = tan(pan2);
  if (pan2 != 0) dpn2 = dpn2 / pan2;
  else dpn2 = dpn1;  // avoid a crash

  // get vector perpendicular to movement of shadow
  s0 = - dot(eb_ubm, eb_ube);
  ax1 = eb_ubm + s0 * eb_ube;
  eb_ubm = eb_ubm + (s0 - dpn1) * eb_ube;
  s0 = - dot(u2m, u2e);
  ax2 = u2m + s0 * u2e;
  u2m = u2m + (s0 - dpn2) * u2e;
  shmv = ax1 - ax2;
  ax2 = ax1 * ax2;
  shmv = shmv * ax2;
  shmv = vnorm(shmv);

  // now get the delta vectors
  eb_udm = u2m - eb_ubm;
  eb_lbe = eb_ube - pan1 * shmv;
  eb_ube = eb_ube + pan1 * shmv;
  eb_ude = u2e + pan2 * shmv;
  eb_lde = u2e - pan2 * shmv;
  eb_ube = vnorm(eb_ube);
  eb_lbe = vnorm(eb_lbe);
  eb_ude = vnorm(eb_ude);
  eb_lde = vnorm(eb_lde);
  eb_lde = eb_lde - eb_lbe;
  eb_ude = eb_ude - eb_ube;

  // scale the delta vectors
  dpn1 = eb_jdstop - eb_jdstart;
  if (dpn1 == 0) dpn1 = 1.0;
  else dpn1 = 1.0 / dpn1;
  eb_udm *= dpn1;
  eb_ude *= dpn1;
  eb_lde *= dpn1;
 }

int EclSolar::GNSBound(bool firstc, bool north, double& lat, double& lng)
 {
  /* Get the geographic coordinates of the northern or southern boundaries
     at time t.
     INPUT:  firstc :true for first call
             north : true for southern boundary
             see also InitBound
     OUTPUT:
            lat, lng : latitude and longitude of penumbra boundary.
            If lat > 90 no northern boundary exists.

     RETURN: 1 if time within eclipse, 0 if end of eclipse
   */
  const double flat = 0.996633;  // flatting of the Earth

  double s0, s, r0, r2, dlt, t;
  Vec3 vrm, vre;

  if (eb_lunactive)  // only solar eclipses
  {
   lng = 0.0;
   lat = 100.0;
   return 0;
  };

  if (firstc)
  {
   InitBound();
   t = eb_jdstart;
   eb_lastjd = t;
  }
  else
  {
   t = eb_lastjd + double(eb_cstep) / (24.0*60.0);
   eb_lastjd = t;
  };

  if (t >= eb_jdstop)
  {
   lng = 0.0;
   lat = 100.0;
   return 0;
  };

  // get shadow vector at time t
  vrm = eb_ubm + (t - eb_jdstart) * eb_udm;
  if (north) vre = eb_ube + (t - eb_jdstart) * eb_ude;
  else vre = eb_lbe + (t - eb_jdstart) * eb_lde;
  vre = vnorm (vre);  // direction of penumbra boundary

  s0 = - dot(vrm, vre);   // distance Moon - fundamental plane
  r2 = dot (vrm,vrm);
  dlt = s0*s0 + 1.0 - r2;
  r0 = 1.0 - dlt;

  if (r0 > 0) r0 = sqrt (r0);
  else r0 = 0;      // distance center of Earth - shadow axis

  // calculate the coordinates if there is an intersecton
  if (r0 < 1.0)  // there should be an intersection
   {
    if (dlt > 0) dlt = sqrt(dlt);
    else dlt = 0;
    s = s0 - dlt;  // distance Moon - fundamental plane
    vrm = vrm + s * vre;
    vrm = vnorm(vrm);
    vrm[2] *= flat;    // vector to intersection
    vre = carpol(vrm);
    lng = vre[1] - lsidtim(t,0,0)*0.261799387799; // geographic coordinates
    if (lng > 2*M_PI) lng -= 2.0*M_PI;
    if (lng < 0.0) lng += 2.0*M_PI;
    lat = sqrt(vrm[0]*vrm[0] + vrm[1]*vrm[1])*0.993305615;
    lat = atan23(vrm[2],lat);
    lat /= degrad;
    lng /= degrad;

    if (lng < 0.0) lng += 360.0;
    if (lng > 360.0) lng -= 360.0;
   }
  else
   {
    lat = 100.0;
    lng = 0;
   };

  return 1;
 }

//------------------ Sunrise / Sunset Boundaries ---------------------------

void EclSolar::InRSBound()
 {
  /* Initialize the calculation for the sunrise and sunset boundaries
     of solar eclipses.
     The function EclStart must have been called first to enable InRSBound
     to get the right eclipse.
     The calculation is done in Earth radii.

     OUTPUT:
            eb_ubm : Moon base vector
            eb_ube : Shadod base vector
            eb_udm : Moon delta vector
            eb_ude : Shadow delta vector for
            eb_dpb : Base value for diameter of penumbra
            eb_dpd : delta value for diameter of penumbra
  */

  double pan;
  Vec3 u2m, u2e;
  Eclipse eclp;

  if (!eb_start_called) eclStart();

  // beginning of the eclipse
  eclp.penumd (eb_jdstart, eb_del_tdut, eb_ubm, eb_ube, eb_dpb, pan);

  // end of the eclipse
  eclp.penumd (eb_jdstop, eb_del_tdut, u2m, u2e, eb_dpd, pan);

  // get the delta vectors
  eb_udm = u2m - eb_ubm;
  eb_ude = u2e - eb_ube;
  eb_dpd = eb_dpd - eb_dpb;

  // scale the delta vectors

  pan = eb_jdstop - eb_jdstart;
  if (pan == 0) pan = 1.0;
  else pan = 1.0 / pan;
  eb_udm *= pan;
  eb_ude *= pan;
  eb_dpd *= pan;
 }

int EclSolar::iscrs(double vrc0, double vrc1, double dpn,
                       double& vrx0, double& vrx1, double& vrx20, double& vrx21)
 {
  /* calculate intersection vectors of the penumbral cone with the
     Earth in the fundamental plane to find the locations of rise
     and set.

     INPUT : components of vectors vrc (pointing to the center of the
             penumbra), vre (unit vector perpendicular to the
             fundamental plane).
             dpn : half diameter of penumbra in Earth radii

     OUTPUT: components of the intersection vectors vrx and vrx2 if
             successful.

     RETURN: 1 if intersection successful, 0 otherwise.
  */
  int rtn;
  double a, b, c, f1, f2, f3, f4;

  rtn = 1;
  f1 = vrc0*vrc0;

  if (f1 < 1.0e-60) rtn = 0;
  else
   {
    f2 = 1.0 - dpn*dpn + f1 + vrc1*vrc1;
    f3 = f2 / (2.0*vrc0);
    f4 = vrc1 / vrc0;
    a = 1.0 + vrc1*vrc1 / f1;
    b = f2*vrc1 / f1;
    c = f2*f2 / (4.0*f1) - 1.0;
    if (fabs(a) < 1.0e-60) rtn = 0;
    else
     {
      vrx21 = -0.5 * b / a; ;
      vrx0 = vrx21*vrx21 - c / a;
      if (vrx0 < 0) rtn = 0;
      else
       {
        vrx0 = sqrt(vrx0);
        vrx1 = vrx21 + vrx0;
        vrx21 = vrx21 - vrx0;
        vrx0 = f3 + f4*vrx1;
        vrx20 = f3 + f4*vrx21;
        vrx1 = -vrx1;
        vrx21 = -vrx21;
       };
     };
   };

  return rtn;
 }

int EclSolar::GRSBound(bool firstc, double& lat1, double& lng1, double& lat2, double& lng2)
 {
  /* Get the geographic coordinates of the boundaries for rise/set
     at time t.

     INPUT: firstc : true for first call

     OUTPUT:
            lat1, lng1; lat2, lng2  : latitude and longitude of first
                                      and second point ofboundary.
                                      If lat > 90 no respective point exists.

    RETURN: 0 if end of eclipse, 1 otherwise
  */
  const double flat = 0.996633;  // flatting of the Earth

  double s0, r0, dpn, t;
  Vec3 vrm, vre, vrc, vrx, vrx2;
  Mat3 m1, m2;

  if (eb_lunactive)  // only solar eclipses
  {
   lng1 = 0.0;
   lat1 = 100.0;
   lng2 = 0.0;
   lat2 = 100.0;
   eb_finished = true;
   return 0;
  };

  if (firstc)
  {
   InRSBound();
   t = eb_jdstart + 1.0/86400.0; // just add a second to be beyond the limit
   eb_lastjd = t;
   eb_finished = false;
  }
  else
  {
   t = eb_lastjd + double(eb_cstep) / (24.0*60.0);
   eb_lastjd = t;
  };

  if (t >= eb_jdstop)
  {
   if (eb_finished)
    {
     lng1 = 0.0;
     lat1 = 100.0;
     lng2 = 0.0;
     lat2 = 100.0;
     return 0;
    };
   t = eb_jdstop - 1.0/86400.0;  // just to go to the very end and then stop
   eb_lastjd = t;
   eb_finished = true;
  };

  // get position of penumbra and shadow cone
  vrm = eb_ubm + (t - eb_jdstart) * eb_udm;
  vre = eb_ube + (t - eb_jdstart) * eb_ude;
  vre = vnorm (vre);  // direction of penumbra boundary
  dpn = eb_dpb + (t - eb_jdstart) * eb_dpd;
  dpn *= 0.5;  // half cone diameter
  vrx = carpol(vre);

  m1 = zrot(vrx[1] + M_PI/2.0);
  m2 = xrot(M_PI/2.0 - vrx[2]);
  m1 = m2 * m1;   // rotation from equatorial into fundamental plane
  m2 = mxtrn(m1); // rotation from fundamental plane into equatorial

  // get vector to center of shadow in the fundamental plane
  s0 = - dot(vrm, vre);
  vrc = vrm + s0 * vre;

  r0 = abs(vrc);  // distance between center of Earth and center of shadow
  vrc = mxvct(m1, vrc);

  lng1 = 0;
  lat1 = 100.0;
  lng2 = 0;
  lat2 = 100.0;
  // check whether intersection of Earth and shadow cone are possible
  // in fundamental plane
  if ((r0 > fabs(1.0 - dpn)) && (r0 < fabs(1.0 + dpn)))
   {
    // now find the intersections
    if (iscrs(vrc[0],vrc[1], dpn, vrx[0],vrx[1], vrx2[0],vrx2[1])) lat1 = 0;
    else
     {
      if (iscrs(vrc[1],vrc[0], dpn, vrx[1],vrx[0], vrx2[1],vrx2[0])) lat1 = 0;
     };
   };

  // calculate the coordinates if there is an intersecton
  if (lat1 < 100.0)  // there should be an intersection
   {
    vrx[2] = 0;
    vrx = mxvct(m2, vrx);
    vrx = vnorm(vrx);
    vrx[2] *= flat;    // vector to intersection
    vre = carpol(vrx);
    lng1 = vre[1] - lsidtim(t,0,0)*0.261799387799; // geographic coordinates

    if (lng1 > M_PI) lng1 -= 2.0*M_PI;
    if (lng1 < (-M_PI)) lng1 += 2.0*M_PI;
    lat1 = sqrt(vrx[0]*vrx[0] + vrx[1]*vrx[1])*0.993305615;
    lat1 = atan23(vrx[2],lat1);
    lat1 /= degrad;
    lng1 /= degrad;
   };
  if (lat1 < 100.0)  // intersection #2
   {
    vrx2[2] = 0;
    vrx2 = mxvct(m2, vrx2);
    vrx2 = vnorm(vrx2);
    vrx2[2] *= flat;    // vector to intersection
    vre = carpol(vrx2);
    lng2 = vre[1] - lsidtim(t,0,0)*0.261799387799; // geographic coordinates
    if (lng2 > M_PI) lng2 -= 2.0*M_PI;
    if (lng2 < (-M_PI)) lng2 += 2.0*M_PI;
    lat2 = sqrt(vrx2[0]*vrx2[0] + vrx2[1]*vrx2[1])*0.993305615;
    lat2 = atan23(vrx2[2],lat2);
    lat2 /= degrad;
    lng2 /= degrad;
   };

  return 1;
 }

/*------------------------- EclDetails ---------------------------------*/

double EclSolar::DegFDms (double h)
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

int EclSolar::localStart(int j, double *spt, double *ept, int *spp,
                               int p, char *otxt)
 {
  /* get start and end times of the various phases of the eclipse
     j = index of the eclipse
     spt[i] = start time in MJD for phase i
     ept[i] = end time in MJD for phase i
     spp[i] = kind of phase i
      p = phase

      RETURN: the number of different phases of this eclipse.

     The step width will be 1 min
   */
  int nump, eflg, pcur, pold, maxp, i, npflg;
  double magecl;  // local magnitude of eclipse at jd
  double jd, step, jdf, d1, d2, elrise;
  double azim, elev, dist, phi, lamda;
  char dts[13];
  char outb[127];
  Vec3 gx;
  Eclipse eclp;

  nump = 0;
  maxp = 0;
  eflg = 0;  // end flag
  pold = 0;
  elev = 0;
  eb_lccnt = 0;
  eb_maxps = -1.0;
  eb_maxelv = -1.0;
  phi = eb_geolat * M_PI / 180.0;
  lamda = eb_geolong * M_PI / 180.0;
  jd = eb_eclmjd[j] - 0.5;  // start 12 hours before maximum
  jdf = jd + 1.5;  // emergency stop if something goes wrong with the loop
  step = 1.0/(24.0*60.0);  // stepwidth 1 minute
  eb_ltotb = jd;
  eb_ltote = jd - 1.0;

  do
   {
    if(p < 0)
     {
      pcur = eclp.lunar (jd, eb_del_tdut);
      gx = eclp.GetRMoon();
      magecl = 1.0;  // just set it 1. It's not needed for lunar eclipses.
      elrise = -9.89e-3;  // allow for refraction during rise/set
     }
    else
     {
      pcur = eclp.solar (jd, eb_del_tdut,d1, d2);
      gx = eclp.GetRSun();
      magecl = 0;
      elrise = -1.45e-2;  // allow for refraction during rise/set
     };

    // check whether body is visible from local position.
    if (pcur > 0)
     {
      AppPos (jd, eclp.GetEp2(), phi, lamda, eb_geoheight, 1, gx,
                                 azim, elev, dist);
      if ((p > 0) && (elev >= elrise))
       {
        magecl = getlocmag(jd, eclp.GetEp2(), phi, lamda, eb_geoheight,
                               eclp.GetRSun(),eclp.GetRMoon(), i);
        if (magecl > eb_maxps)
         {
          eb_maxps = magecl;
          eb_maxelv = elev;
          eb_jdmaxps = jd;
         }
       }
     }

    if ((eb_lccnt == 0) && (pcur != 0))
     {
      if ((elev >= elrise) && (magecl > 0))
       {
        eb_lcb1 = jd;
        eb_lccnt = 1;
       }
     }
    if (eb_lccnt == 1)
     {
      if ((elev < elrise) || (pcur == 0) || (magecl == 0))
       {
        eb_lce1 = jd;
        eb_lccnt = 2;
       }
     }
    if ((eb_lccnt == 2) && (pcur != 0))
     {
      if ((elev >= elrise) && (magecl > 0))
       {
        eb_lcb2 = jd;
        eb_lccnt = 3;
       }
     }
    if (eb_lccnt == 3)
     {
      if ((elev < elrise) || (pcur == 0) || (magecl == 0))
       {
        eb_lce2 = jd;
        eb_lccnt = 4;
       }
     }

    // now check the start and stop times for the phases
    if (pcur > pold)
     {
      if(nump < 4)
       {
        spt[nump] = jd;
        spp[nump] = pcur;
        nump++;
        maxp++;
        pold = pcur;
       }
     }
    else if (pcur < pold)
     {
      npflg = 1;
      if (nump > 1)
       {
        if (pcur > spp[nump-2]) npflg = 0;
       }
      if (npflg)
       {
        nump--;
        if (nump >= 0) ept[nump] = jd;
        if (nump > 1)
         {
          if (pcur < spp[nump-1])
           {
            nump--;
            ept[nump] = jd;
           }
         }
        if (nump <= 0) eflg = 1;
       }
      pold = pcur;
     };

    jd += step;
    if (jd > jdf) eflg = 1;

   } while (eflg != 1);

  //  put the respective info into textfile otxt
  if(maxp > 0)
   {
    for (i=0; i<maxp; i++)
     {
      if (p < 0)
       {
        switch (spp[i])
         {
          case 1: strcpy(outb,"penumbral   ");
                  break;
          case 2: strcpy(outb,"tot.penumb  ");
                  break;
          case 3: strcpy(outb,"partial     ");
                  break;
          case 4: strcpy(outb,"totality    ");
                  break;
         }
       }
      else
       {
        switch (spp[i])
         {
          case 1: strcpy(outb,"partial     ");
                  break;
          case 2: strcpy(outb,"n-centr.Ann ");
                  break;
          case 3: strcpy(outb,"n-centr.Tot ");
                  break;
          case 4: strcpy(outb,"annularity  ");
                  break;
          case 5: strcpy(outb,"totality    ");
                  break;
         }
       }

        strcat(otxt,"\n ");
        strcat(otxt, outb);
        strcat(otxt,"\tbegins ");
        dtmstr((spt[i]+eb_tzone/24.0),dts);
        dts[12] = '\0';
        strcat(otxt,dts);
        strcat(otxt, "\tends ");
        dtmstr((ept[i]+eb_tzone/24.0),dts);
        dts[12] = '\0';
        strcat(otxt,dts);
     }
   }
  if (maxp > 0)
   {
    if (eb_lccnt == 1) eb_lce1 = ept[0];
    if (eb_lccnt == 3) eb_lce2 = ept[0];
   }

  if (eb_maxps > 0.8)  // check for local central phase
   {
    jd = eb_jdmaxps - 16.0/(24.0*60.0);
    eb_ltote = jd + 32.0/(24.0*60.0);
    eflg = 0;
    do
     {
      jd += 1.0/86400.0;  // go in seconds steps
      eclp.solar (jd, eb_del_tdut,d1, d2);
      gx = eclp.GetRSun();
      AppPos (jd, eclp.GetEp2(), phi, lamda, eb_geoheight, 1, gx,
                                      azim, elev, dist);
      if (elev >= elrise)
       {
        magecl = getlocmag(jd, eclp.GetEp2(), phi, lamda, eb_geoheight,
                               eclp.GetRSun(),eclp.GetRMoon(), i);
        if (magecl > eb_maxps)
         {
          eb_maxelv = elev;
          eb_maxps = magecl;
          eb_jdmaxps = jd;
         }
        if (i > 0)
         {
          eb_ltotb = jd;
          eflg = 1;
         }
       }
      if (jd > eb_ltote) eflg = 1;

     } while (!eflg);

    if (jd < eb_ltote)
     {
      eflg = 0;
      do
       {
        jd += 1.0/86400.0;  // go in seconds steps
        eclp.solar (jd, eb_del_tdut,d1, d2);
        gx = eclp.GetRSun();
        AppPos (jd, eclp.GetEp2(), phi, lamda, eb_geoheight, 1, gx,
                                        azim, elev, dist);
        if (elev < elrise) eflg = 1;
        magecl = getlocmag(jd, eclp.GetEp2(), phi, lamda, eb_geoheight,
                               eclp.GetRSun(),eclp.GetRMoon(), i);
        if (magecl > eb_maxps)
         {
          eb_maxelv = elev;
          eb_maxps = magecl;
          eb_jdmaxps = jd;
         }
        if (i == 0) eflg = 1;
        if (jd > eb_ltote) eflg = 1;

       } while (!eflg);
      eb_ltote = jd;
     }
    else eb_ltote = eb_ltotb - 1.0;
   }

  return maxp;
 }


void EclSolar::getLocalDetails(char *otxt)
 {
  /* get the details of the eclipse selected in eclbuf.select
     and place the output into otxt
  */
  int j, p, i, ecloutbn;
  int dd, mm, yy, deg, mnt;
  double sec, hh;

  int spp[4], nump;
  double spt[4], ept[4];
  double jd, jdf;
  char dts[13];
  char outb[127];

  if (!eb_start_called) eclStart();
  eb_local_called = true;

  j = eb_eclselect-1;
  p = eb_phase[j];

  ecloutbn = 3;
  sprintf(otxt,"+++ Timezone: %g +++  TT - UTC: %g (sec) +++ Year: %5i +++\n\n", eb_tzone, eb_del_tdut, eb_year);
    switch (p)
     {
      case 1: strcpy(outb,"\t\tPartial Eclipse of the Sun");
              break;
      case 2: strcpy(outb,"\t\tNon-Central Annular Eclipse of the Sun");
              break;
      case 3: strcpy(outb,"\t\tNon-Central Total Eclipse of the Sun");
              break;
      case 4: strcpy(outb,"\t\tAnnular Eclipse of the Sun");
              break;
      case 5: strcpy(outb, "\t\tTotal Eclipse of the Sun");
              break;
      case 6: strcpy(outb, "\t\tAnnular/Total Solar Eclipse");
              break;

      case -1:
      case -2: strcpy(outb, "\t\tPenumbral Eclipse of the Moon");
               break;
      case -3: strcpy(outb, "\t\tPartial eclipse of the Moon");
               break;
      case -4: strcpy(outb, "\t\tTotal eclipse of the Moon");
               break;
     };

    strcat(otxt,outb);
    sprintf(outb,"\n\nMaximum Eclipse at ");
    strcat(otxt,outb);
    dtmstr((eb_eclmjd[j]+eb_tzone/24.0),dts);
    dts[12] = '\0';
    strcat(otxt,dts);
    if (p < 4)
     {
      sprintf(outb,"   with magnitude:%5.2f",eb_magnitude[j]);
      strcat(otxt, outb);
     }

    strcat(otxt,"\n");
    nump = localStart(j, spt, ept, spp, p, otxt);
    if ((p < 4) || (nump < 1)) ecloutbn = 5;

  if (ecloutbn == 3)
   {
    // get start and stop dates for central phase
    jd = spt[nump-1];
    for (i=0; i<nump; i++)
    if ((spt[i] < jd) && (spp[i] > 3)) jd = spt[i]; // start of central phase
    caldat(jd, dd, mm, yy, hh);
    dms (hh,deg,mnt,sec);
    sec = 0;
    i = mnt / eb_cstep;
    mnt = i* int(eb_cstep);   // cut to proper time step
    hh = ddd (deg, mnt, sec);
    jdf = ept[nump-1];
    for (i=0; i<nump; i++)
     if ((ept[i] > jdf) && (spp[i] > 3)) jdf = ept[i]; // end of central phase
   }

  // local circumstances
      strcat(otxt, "\n\n\nLocal Circumstances for ");
      jd = eb_geolat;
      jdf = eb_geolong;
    sprintf(outb,"\nLat: %g   Long: %g   height: %g m\n\n",
                               jd, jdf, eb_geoheight);
    strcat(otxt,outb);
    if (p != 0)
     {
      if (eb_lccnt > 0)
       {
        sprintf(outb,"Eclipse visible from ");
        dtmstr((eb_lcb1+eb_tzone/24.0),dts);
        dts[12] = '\0';
        strcat(outb," ");
        strcat(outb,dts);
        strcat(outb," to ");
        dtmstr((eb_lce1+eb_tzone/24.0),dts);
        dts[12] = '\0';
        strcat(outb,dts);
        if (eb_lccnt > 2)  // this case almost never happens
         {
          strcat(otxt,outb);
          strcpy(outb,"\n\tand from ");
          dtmstr((eb_lcb2+eb_tzone/24.0),dts);
          dts[12] = '\0';
          strcat(outb," ");
          strcat(outb,dts);
          strcat(outb," to ");
          dtmstr((eb_lce2+eb_tzone/24.0),dts);
          dts[12] = '\0';
          strcat(outb,dts);
         }
       }
      else sprintf(outb,"Eclipse not visible");
      strcat(otxt,outb);
     }

   // local solar eclipse magnitude
    if ((p > 0) && (eb_lccnt > 0))
     {
      sprintf(outb,"\nMaximum Eclipse at ");
      strcat(otxt,outb);
      dtmstr((eb_jdmaxps+eb_tzone/24.0),dts);
      dts[12] = '\0';
      strcat(otxt,dts);
      sprintf(outb,"   with magnitude %6.3f", eb_maxps);
      strcat(otxt,outb);
      sprintf(outb,"   elev:%4.1f", 180.0*eb_maxelv/M_PI);
      strcat(otxt,outb);
      if (eb_ltotb <= eb_ltote)
       {      // local central eclipse
        if ((p % 2) == 0) strcpy(outb, "\nannularity from");
        else strcpy(outb, "\ntotality from");
        if ((p == 1) || (p == 6))
                   strcpy(outb, "\ntotality/annularity from");
        strcat(otxt,outb);
        caldat((eb_ltotb+eb_tzone/24.0), dd, mm, yy, hh);
        caldat((eb_ltote+eb_tzone/24.0), dd, mm, yy, jd);
        hh = DegFDms(hh);
        jd = DegFDms(jd);
        jdf = (eb_ltote - eb_ltotb) * 86400.0;
        sprintf(outb,"%8.4f  to%8.4f   del.t:%3.0f sec \n",hh,jd, jdf);
        strcat(otxt,outb);
       }
     }

    eb_maxelv /= degrad;
 }

//---------Northern and Southern Umbra Boundaries ---------------------

double EclSolar::navCourse (double lat1, double lng1, double lat2, double lng2)
{
 // get course (in radians) from (lat1, lng1) to (lat2, lng2) over the Earth surface
 // the geographic coordinates are in decimal degrees

 double lt1, ln1, lt2, ln2, cd, an;

 lt1 = lat1 * degrad;
 ln1 = lng1 * degrad;
 lt2 = lat2 * degrad;
 ln2 = lng2 * degrad;

 cd = sin(lt1)*sin(lt2) + cos(lt1)*cos(lt2)*cos(ln2 - ln1);
 an = acos(cd);
 an = cos(lt1) * sin(an);

 if (an == 0) return 0;  // same spot. didn't move

 cd = (sin(lt2) - sin(lt1)*cd) / an;
 an = acos (cd);

 if (sin(ln2 - ln1) < 0) an = -an;

 return an;
}

void EclSolar::navNewPos (double d, double an, double lat1, double lng1, double &lat2, double &lng2)
{
 /*
    starting from (lat1, lng1) along the great circle for d (radians) with course an (in radians)
    get the new position (lat2, lng2) (in decimal degrees) at the Earth surface
 */

 double cd, sd, lt1, ag;

 ag = an;
 if (ag > M_PI) ag -= 2*M_PI;
 if (ag < -M_PI) ag += 2*M_PI;

 cd = cos(d);
 lt1 = lat1 * degrad;

 sd = cd * sin(lt1) + sin(d) * cos(lt1) * cos(ag);
 lat2 = asin(sd);

 lng2 = cos(lt1) * cos(lat2);

 if (lng2 == 0)  // just in case to avoid a crash
 {
     lat2 = lat1;
     lng2 = lng1;
     return;
 }

 lng2 = (cd - sin(lt1) * sd) / lng2;
 lng2 = acos(lng2);
 lng2 /= degrad;
 if (ag > 0) lng2 = lng1 + lng2;
 else lng2 = lng1 -lng2;
 if(lng2 > 360.0) lng2 -= 360.0;
 if(lng2 < 0) lng2 += 360.0;

 lat2 /= degrad;

}

int EclSolar::centralBound(bool firstc, double& lat1, double& lng1, double& lat2, double& lng2)
{
  /* Get the geographic coordinates of the northern or southern boundaries
     of the umbra at time t.
     INPUT:  firstc :true for first call

     OUTPUT:
            lat, lng : latitude and longitude of umbra boundary in decimal degrees.
            If lat > 90 no boundary exists.

     RETURN: current phase if time within eclipse, <=3 if no central eclipse
   */

  bool lastp;
  int k;
  double dpn1, t;
  double lt1, ln1, lt2, ln2, an;
  Eclipse eclp;

  if (!eb_start_called) eclStart();

  lng1 = 0.0;
  lat1 = 100.0;
  lng2 = 0.0;
  lat2 = 100.0;

  lastp = false;

  if (eb_lunactive) return 0;  // only solar eclipses


  if (firstc)
  {
   k = eclPltCentral(true, lt1, ln1);
  }
  else k = eclPltCentral(false, lt1, ln1);
  t = eb_lastjd;

  if (k <= 3) return k;  // no central eclipse

  k = eclPltCentral(false, lt2, ln2);  // next step
  eb_lastjd = t;  // go back to the original step

  if (k <= 3)  // try the last step instead at the end
  {
      eb_lastjd -= 2.0 * eb_cstep / (24.0*60.0);
      k = eclPltCentral(false, lt2, ln2);
      eb_lastjd = t;
      lastp = true;
  };

  if (k <= 3) return k;  // no central eclipse

  eclp.solar(t, eb_del_tdut, lat1, lng1);
  lat1 = 100.0;
  lng1 = 0;

  an = navCourse (lt1, ln1, lt2, ln2);  // direction of shadow along Earth surface
  an += 0.5*M_PI; // direction perpendicular to shadow movement (right boundary)

  eclp.duration(t, eb_del_tdut, dpn1);  // dpn1 is width of umbra in km
  dpn1 = (dpn1 / 111.1) * 0.0174533;  // radians of umbra width
  dpn1 /= 2.0;
  navNewPos(dpn1, an, lt1, ln1, lat1, lng1);

  an -= M_PI;  // find opposite boundary

  navNewPos(dpn1, an, lt1, ln1, lat2, lng2);

  if (lastp)  // we went the opposite way at the end
  {
      lt1 = lat2;
      ln1 = lng2;
      lat2 = lat1;
      lng2 = lng1;
      lat1 = lt1;
      lng1 = ln1;
      return 0;  // end of eclipse
  }
  else return k;
 }

//-------------------- Shadow Cone -------------------------

void EclSolar::getShadowCone(double mjd, bool umbra, int numpts, double* lat, double* lng)
{
  /*  Get the geographic coordinates of the shadow cone at MJD-time mjd.
      if umbra is true the umbra cone will be returned
      if umbra is false the penumbra will be returned

     OUTPUT:
            lat and lng must be arrays of length numpts into which the numpts points will be placed
            if there is no (total) eclipse at the time, lat will be set 100.0, lng 0.
  */

  const double flat = 0.996633;  // flatting of the Earth

  int j, k1, k2, kmiss;
  double dpn1, pan1, s0, dlt, dta, ag;
  double s, r0, r2, dt1, dt2;
  Vec3 vrm, vre;
  Vec3 ubm, ube;
  Vec3 ax1, ax2;
  Mat3 mx1, mx2;
  Eclipse eclp;

  if (numpts < 2) return;

  for (j=0; j<numpts; j++)  // just in case you got to return empty-handed
  {
      lat[j] = 100.0;
      lng[j] = 0;
  }

  if (!eb_start_called) eclStart();
  if (eb_lunactive) return;  // only for solar eclipses

  if (umbra && (eb_phase[eb_eclselect-1] < 1)) return;

  // get the shadow details
  if(umbra)  eclp.umbra (mjd, eb_del_tdut, ubm, ube, dpn1, pan1);
  else eclp.penumd (mjd, eb_del_tdut, ubm, ube, dpn1, pan1);

  dpn1 *= 0.5;

  if (!umbra)
  {
      if (eb_penamode == 0)
      {
        dpn1 *= eb_penangle;
        pan1 *= eb_penangle;
      }

      if (eb_penamode > 0)
      {
          s0 = eb_penangle * tan(pan1);
          s0 = atan(s0);
          if (pan1 > 0)
          {
              dpn1 = dpn1 * s0 / pan1;
              pan1 = s0;
          }
      }
  };

  // get apex of umbra/penumbra cone
  pan1 = tan(pan1);
  if (pan1 < 0.0000174533) return;  // if cone is smaller that 0.001°
  dpn1 = dpn1 / pan1;

  s0 = - dot(ubm, ube);
  ubm = ubm + (s0 - dpn1) * ube;

  // get any vector perpendicular to the shadow
  ax1[0] = 0;
  ax1[1] = 0;
  ax1[2] = 1.0;
  ax2 = ax1 * ube;
  ax1 = vnorm(ax2) * pan1;

  ax2 = carpol(ube);
  mx1 = zrot(ax2[1]);
  mx2 = yrot(ax2[2]) * mx1;  // transform to a system where x points into the direction of the shadow
  mx1 = mxtrn(mx1);  // to get back to equatorial system after rotation

  ax2 = mxvct(mx2,ax1);  // vector which we will rotate numpts times

  // now loop for numpts points
  dta = 2.0*M_PI / double(numpts);

  for (j=0; j<numpts; j++)
  {
      ag = double(j) * dta;  // rotation angle of the cone vector
      mx2  = xrot(ag);
      ax1 = mxvct(mx2,ax2);
      ax1 = mxvct(mx1, ax1);

      vre = ube + ax1;
      vre = vnorm(vre);  // direction in which to find an intersection
      vrm[0] = ubm[0];
      vrm[1] = ubm[1];
      vrm[2] = ubm[2];

      s0 = - dot(vrm, vre); // distance Apex - fundamental plane
      r2 = dot (vrm,vrm);
      dlt = s0*s0 + 1.0 - r2;
      r0 = 1.0 - dlt;

      if (r0 > 0) r0 = sqrt (r0);
      else r0 = 0;   // distance center of Earth - shadow axis

      // calculate the coordinates if there is an intersecton
      if (r0 < 1.0)  // there should be an intersection
      {
       if (dlt > 0) dlt = sqrt(dlt);
       else dlt = 0;
       s = s0 - dlt;  // distance Apex - fundamental plane
       vrm = vrm + s * vre;
       vrm = vnorm(vrm);
       vrm[2] *= flat;    // vector to intersection
       vre = carpol(vrm);
       lng[j] = vre[1] - lsidtim(mjd,0,0)*0.261799387799; // geographic coordinates
       if (lng[j] > 2*M_PI) lng[j] -= 2.0*M_PI;
       if (lng[j] < 0.0) lng[j] += 2.0*M_PI;
       lat[j] = sqrt(vrm[0]*vrm[0] + vrm[1]*vrm[1])*0.993305615;
       lat[j] = atan23(vrm[2],lat[j]);
       lat[j] /= degrad;
       lng[j] /= degrad;

       if (lng[j] < 0.0) lng[j] += 360.0;
       if (lng[j] > 360.0) lng[j] -= 360.0;
      }
      else  // no intersection.
      {
          lat[j] = 100.0;
          lng[j] = 0;
      };
  }

  k1 = -1;
  k2 = -1;
  kmiss = 0;
  for (j=0; j<numpts; j++)  // check for missing points
  {
      if (lat[j] < 100.0)
      {
          if (k1 < 0) k1 = j;  // first valid point
          k2 = j;              // last valid point
      }
      else kmiss++;
  }

  if ((kmiss < 2) || (kmiss >= (numpts -1))) return;  // cone completely on Earth surface or not at all

  dt1 = double(k1) * dta;
  dt2 = double(k2) * dta;
  k1--;
  k2++;
  if (k1 < 0) k1 = numpts - 1;  // wrap around
  if (k2 >= (numpts-1)) k2 = 0;
  dta = 2.0*M_PI / double(numpts*20);

  for (j=1; j<20; j++) // go in smaller steps to get closer to the borderline
  {
      ag = dt1 - double(j) * dta;  // rotation angle of the cone vector
      mx2  = xrot(ag);
      ax1 = mxvct(mx2,ax2);
      ax1 = mxvct(mx1, ax1);

      vre = ube + ax1;
      vre = vnorm(vre);  // direction in which to find an intersection
      vrm[0] = ubm[0];
      vrm[1] = ubm[1];
      vrm[2] = ubm[2];

      s0 = - dot(vrm, vre); // distance Apex - fundamental plane
      r2 = dot (vrm,vrm);
      dlt = s0*s0 + 1.0 - r2;
      r0 = 1.0 - dlt;

      if (r0 > 0) r0 = sqrt (r0);
      else r0 = 0;   // distance center of Earth - shadow axis

      // calculate the coordinates if there is an intersecton
      if (r0 < 1.0)  // there should be an intersection
      {
       if (dlt > 0) dlt = sqrt(dlt);
       else dlt = 0;
       s = s0 - dlt;  // distance Apex - fundamental plane
       vrm = vrm + s * vre;
       vrm = vnorm(vrm);
       vrm[2] *= flat;    // vector to intersection
       vre = carpol(vrm);
       lng[k1] = vre[1] - lsidtim(mjd,0,0)*0.261799387799; // geographic coordinates
       if (lng[k1] > 2*M_PI) lng[k1] -= 2.0*M_PI;
       if (lng[k1] < 0.0) lng[k1] += 2.0*M_PI;
       lat[k1] = sqrt(vrm[0]*vrm[0] + vrm[1]*vrm[1])*0.993305615;
       lat[k1] = atan23(vrm[2],lat[k1]);
       lat[k1] /= degrad;
       lng[k1] /= degrad;

       if (lng[k1] < 0.0) lng[k1] += 360.0;
       if (lng[k1] > 360.0) lng[k1] -= 360.0;
      }
      else break;
  }

  for (j=1; j<20; j++) // go in smaller steps to get closer to the borderline
  {
      ag = dt2 + double(j) * dta;  // rotation angle of the cone vector
      mx2  = xrot(ag);
      ax1 = mxvct(mx2,ax2);
      ax1 = mxvct(mx1, ax1);

      vre = ube + ax1;
      vre = vnorm(vre);  // direction in which to find an intersection
      vrm[0] = ubm[0];
      vrm[1] = ubm[1];
      vrm[2] = ubm[2];

      s0 = - dot(vrm, vre); // distance Apex - fundamental plane
      r2 = dot (vrm,vrm);
      dlt = s0*s0 + 1.0 - r2;
      r0 = 1.0 - dlt;

      if (r0 > 0) r0 = sqrt (r0);
      else r0 = 0;   // distance center of Earth - shadow axis

      // calculate the coordinates if there is an intersecton
      if (r0 < 1.0)  // there should be an intersection
      {
       if (dlt > 0) dlt = sqrt(dlt);
       else dlt = 0;
       s = s0 - dlt;  // distance Apex - fundamental plane
       vrm = vrm + s * vre;
       vrm = vnorm(vrm);
       vrm[2] *= flat;    // vector to intersection
       vre = carpol(vrm);
       lng[k2] = vre[1] - lsidtim(mjd,0,0)*0.261799387799; // geographic coordinates
       if (lng[k2] > 2*M_PI) lng[k2] -= 2.0*M_PI;
       if (lng[k2] < 0.0) lng[k2] += 2.0*M_PI;
       lat[k2] = sqrt(vrm[0]*vrm[0] + vrm[1]*vrm[1])*0.993305615;
       lat[k2] = atan23(vrm[2],lat[k2]);
       lat[k2] /= degrad;
       lng[k2] /= degrad;

       if (lng[k2] < 0.0) lng[k2] += 360.0;
       if (lng[k2] > 360.0) lng[k2] -= 360.0;
      }
      else break;
  }

}

