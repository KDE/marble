//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "AbstractLayer/AbstractLayerData.h"
#include "GpsElement.h"
#include <QtCore/QTime>
#include <QtCore/QString>

/*! \brief representaion of a GPS Waypoint.
 * 
 * A Waypoint is a point of interest, or named feature on a map.
 */
class Waypoint : public AbstractLayerData, public GpsElement 
{
 public:
    Waypoint( double lat, double lon );
    Waypoint( GeoPoint position );
    
    ~Waypoint();
    void setLat( const double &lat );
    void setLon( const double &lon );
    void setElevation( const double &elevation );
    void setTime( const QTime &time );
    void setMagVariation( const double &magVar );
    void setGeoIdHeight( const double &geoHeightId );
    void setGpsSymbol( const QString &gpsSymbol );
    void setType( const QString &type );
    void setFix( const QString &fix );
    void setSatalites( const int &satalites );
    void setHdop( const double &hdop );
    void setVdop( const double &vdop );
    void setPdop( const double &pdop );
    void setAgeOfGpsData( const double &age );
    void setDgpsid( const int &gpsdid );
 private:
     //!latitude.
    double *m_lat;
    //!longitude.
    double *m_lon;
    //!Elevation (in meters) of the point
    double *m_elevation;
    //!Timestamp on waypoint creation
    QTime *m_time;
    //!Magnetic variation (in degrees) at the point
    double *m_magVariation; // 0.0 <= value <=360.0
    /*!
     * Quote from GPX schema:
     * Height (in meters) of geoid (mean sea level) above WGS84 earth
     * ellipsoid. As defined in NMEA GGA message.
     */
    double *m_geoIdHeight;
    //!Text of GPS symbol name.
    QString *m_gpsSymbol;
    //!classification of the waypoint
    QString *m_type;
    /*!\brief Type of GPS fix. 
     * 
     * none means GPS had no fix. To signify the fix info is unknown,
     * leave out fixType entirely. pps = military signal used
     * value comes from list: {'none'|'2d'|'3d'|'dgps'|'pps'}
     */
    QString *m_fix;
    //!number of satalites to calculate fix
    int *m_satalites;
    /*!\brief Horizontal dilution of precision.
     * 
     * Quote Wikipedia: low DOP value represents a better GPS
     * positional accuracy due to the wider angular separation 
     * between the satellites used to calculate a GPS unit's position
     */
    double *m_hdop;
    //!\brief Vertical dilution of presision.
    double *m_vdop;
    //!\brief Position dilution of precision.
    double *m_pdop;
    //!Number of seconds since last DGPS update.
    double *m_ageOfGpsData;
    //!ID of DGPS station used in differential correction.
    int *m_dgpsid; //0<=value <=1023
};

#endif //WAYPOINT_H
