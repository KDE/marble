#ifndef GEODATALOOKAT_H
#define GEODATALOOKAT_H

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
			 * @brief retrieves the pointer to GeoDataCoordinates object of the GeoDataLookAt object
			 * @return GeoDataCoordinates*
			 */
			GeoDataCoordinates* coordinate() const;

		private:
			GeoDataLookAtPrivate *d;

	};
}
#endif


