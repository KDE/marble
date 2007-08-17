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

class QPixmap;

/**
 * @brief representaion of a GPS Waypoint.
 * 
 * A Waypoint is a point of interest, or named feature on a map.
 */
class Waypoint : public AbstractLayerData, public GpsElement 
{
 public:
    /**
     * @brief Waypoint constructor from latitude and longitude
     * 
     * This constructor sets all of the pointers to 0 and allows the 
     * AbstractLayerData constructor make a geopoint from the latitude
     * and longitude.
     * @param lat double representation of latitude
     * @param lon double representation of longitude
     */
    Waypoint( double lat, double lon );
    
    /**
     * @brief Constructor froim a geopoint object
     * @param position the geopoint that will be the position of this
     *                 waypoint
     */
    Waypoint( GeoPoint position );
    
    /**
     * @brief simple destructor
     */
    ~Waypoint();
    /*

    void draw ( ClipPainter *painter, const QPoint &point ){}

    void draw(ClipPainter *painter, 
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis){}
    */

    
    /**
     * @brief implementation of AbstractLayerData::draw
     * @see AbstractLayerData::draw
     */
    virtual void draw( ClipPainter *, const QPoint & );
    
    /**
     * @brief implementation of AbstractLayerData::draw
     * @see AbstractLayerData::draw
     */
    virtual void draw( ClipPainter *painter, const QSize &canvasSize, 
                       double radius, Quaternion invRotAxis );
    virtual void printToStream( QTextStream & ) const;
    void printBodyToStream( QTextStream & ) const;
    
    /**
     * @brief m_elevation getter
     */
    double elevation();
    
    /**
     * @brief m_elevation setter
     */
    void setElevation( const double &elevation );
    
    /**
     * @brief m_time getter
     */
    QTime time();
    
    /**
     * @brief m_time setter
     */
    void setTime( const QTime &time );
    
    /**
     * @brief m_magVariation getter
     */
    double magVariation();
    
    /**
     * @brief m_magVariation setter
     */
    void setMagVariation( const double &magVar );
    
    /**
     * @brief m_geoIdHeight getter
     */
    double geoIdHeight();
    
    /**
     * @brief m_geoIdHeight setter
     */
    void setGeoIdHeight( const double &geoHeightId );
    
    /**
     * @brief m_gpsSymbol getter
     */
    QString gpsSymbol();
    
    /**
     * @brief m_gpsSymbol setter
     */
    void setGpsSymbol( const QString &gpsSymbol );
    
    /**
     * @brief m_type getter
     */
    QString type();
    
    /**
     * @brief m_type setter
     */
    void setType( const QString &type );
    
    /**
     * @brief m_fix getter
     */
    QString fix();
    
    /**
     * @brief m_fix setter
     */
    void setFix( const QString &fix );
    
    /**
     * @brief m_satalites getter
     */
    int satalites();
    
    /**
     * @brief m_satalites setter
     */
    void setSatalites( const int &satalites );
    
    /**
     * @brief m_hdop getter
     */
    double hdop();
    
    /**
     * @brief m_hdop setter
     */
    void setHdop( const double &hdop );
    
    /**
     * @brief m_vdop getter
     */
    double vdop();
    
    /**
     * @brief m_vdop setter
     */
    void setVdop( const double &vdop );
    
    /**
     * @brief m_pdop getter
     */
    double pdop();
    
    /**
     * @brief m_pdop setter
     */
    void setPdop( const double &pdop );
    
    /**
     * @brief m_ageOfGpsData getter
     */
    double ageOfGpsData();
    
    /**
     * @brief m_ageOfGpsData setter
     */
    void setAgeOfGpsData( const double &age );
    
    /**
     * @brief m_dgpsid getter
     */
    int dgpsid();
    
    /**
     * @brief m_dgpsid setter
     */
    void setDgpsid( const int &gpsdid );
 private:
    /**
     * @brief Elevation (in meters) of the waypoint
     */
    double *m_elevation;
    
    /**
     * @brief Timestamp on waypoint creation
     */
    QTime *m_time;
    
    /**
     * @brief Magnetic variation (in degrees) at the point
     * 
     * this is a value where 0.0 <= value <= 360.0
     */
    double *m_magVariation;
    
    /**
     * Quote from GPX schema:
     * Height (in meters) of geoid (mean sea level) above WGS84 earth
     * ellipsoid. As defined in NMEA GGA message.
     */
    double *m_geoIdHeight;
    
    /**
     * @brief Text representation of GPS symbol name.
     */
    QString *m_gpsSymbol;
    
    /**
     * @brief classification of the waypoint
     */
    QString *m_type;
    
    /**
     * @brief Type of GPS fix. 
     * 
     * none means GPS had no fix. To signify the fix info is unknown,
     * leave out fixType entirely. pps = military signal used
     * value comes from list: {'none'|'2d'|'3d'|'dgps'|'pps'}
     */
    QString *m_fix;
    
    /**
     * @brief number of satalites to calculate fix
     */
    int *m_satalites;
    
    /**
     * @brief Horizontal dilution of precision.
     * 
     * Quote Wikipedia: low DOP value represents a better GPS
     * positional accuracy due to the wider angular separation 
     * between the satellites used to calculate a GPS unit's position
     */
    double *m_hdop;
    
    /**
     * @brief Vertical dilution of presision.
     */
    double *m_vdop;
    
    /**
     * @brief Position dilution of precision.
     */
    double *m_pdop;
    
    /**
     * @brief Number of seconds since last DGPS update.
     */
    double *m_ageOfGpsData;
    
    /**
     * @brief ID of DGPS station used in differential correction.
     * 
     * a value where:  0 <= value <= 1023
     */
    int *m_dgpsid;
};

#endif //WAYPOINT_H
