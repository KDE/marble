//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEODATALOOKAT_H
#define MARBLE_GEODATALOOKAT_H

#include "geodata_export.h"
#include "GeoDataAbstractView.h"
#include "GeoDataCoordinates.h"

namespace Marble
{
    class GeoDataLookAtPrivate;

    class GEODATA_EXPORT GeoDataLookAt : public GeoDataAbstractView
    {
    public:
        GeoDataLookAt();

        GeoDataLookAt(const GeoDataLookAt& other);

        GeoDataLookAt& operator=(const GeoDataLookAt &other);

        ~GeoDataLookAt();

        /**
         * @brief set the altitude in a GeoDataLookAt object
         * @param altitude latitude
         *
         */
        void setAltitude( qreal altitude);

        /**
         * @brief retrieves the altitude of the GeoDataLookAt object
         * @return latitude
         */
        qreal altitude( ) const;

        /**
         * @brief set the latitude in a GeoDataLookAt object
         * @param latitude latitude
         * @param unit units that lon and lat get measured in
         * (default for Radian: north pole at pi/2, southpole at -pi/2)
         */
        void setLatitude( qreal latitude,GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

        /**
         * @brief retrieves the latitude of the GeoDataLookAt object
         * use the unit parameter to switch between Radian and DMS
         * @param unit units that lon and lat get measured in
         * (default for Radian: north pole at pi/2, southpole at -pi/2)
         * @return latitude
         */
        qreal latitude( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

        /**
         * @brief set the longitude in a GeoDataLookAt object
         * @param longitude longitude
         * @param unit units that lon and lat get measured in
         * (default for Radian: north pole at pi/2, southpole at -pi/2)
         */
        void setLongitude( qreal longitude,GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

        /**
         * @brief retrieves the longitude of the GeoDataLookAt object
         * use the unit parameter to switch between Radian and DMS
         * @param unit units that lon and lat get measured in
         * (default for Radian: north pole at pi/2, southpole at -pi/2)
         * @return latitude
         */
        qreal longitude( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

        /**
         * @brief retrieve the lat/lon/alt triple as a GeoDataCoordinates object
         * @return GeoDataCoordinates
         * @see longitude latitude altitude
         */
        GeoDataCoordinates coordinates() const;

        /**
          * @brief Change the distance (in meters) between the camera and the object looked at
          * @see range
          */
        void setRange( qreal range );

        /**
          * @brief Retrieve the distance (in meters) between the camera and the object looked at
          * @see setRange
          */
        qreal range() const;
        
        /**
        * @brief set the GeoDataCoordinates object
        * @param GeoDataCoordinates
        * @see GeoDataCoordinates
        */
        void setCoordinates( const GeoDataCoordinates& coordinates );

        /// Provides type information for downcasting a GeoNode
        virtual QString nodeType() const;

        void detach();
    private:
        GeoDataLookAtPrivate *d;

    };
}
Q_DECLARE_METATYPE( Marble::GeoDataLookAt )
#endif
