#ifndef GEODATALOOKAT_P_H
#define GEODATALOOKAT_P_H

#include "GeoDataCoordinates.h"
namespace Marble
{
class GeoDataLookAtPrivate
{
	public :

		GeoDataLookAtPrivate()
		{
		}

		~GeoDataLookAtPrivate()
		{
		}
		
		GeoDataCoordinates *m_coord;
};
} 
#endif

