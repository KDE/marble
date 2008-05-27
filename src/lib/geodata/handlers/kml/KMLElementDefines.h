#ifndef KMLElementDefines_h
#define KMLElementDefines_h
#include "GeoDataParser.h"
#include "KMLElementDictionary.h"

#define parentItemIsFeature \
parentItem.represents( kmlTag_Folder ) || \
parentItem.represents( kmlTag_Placemark ) || \
parentItem.represents( kmlTag_NetworkLink ) || \
parentItem.represents( kmlTag_Document ) || \
parentItem.represents( kmlTag_ScreenOverlay ) || \
parentItem.represents( kmlTag_GroundOverlay )

#define parentItemIsContainer \
parentItem.represents( kmlTag_Folder ) || \
parentItem.represents( kmlTag_Document )

#define parentItemIsOverlay \
parentItem.represents( kmlTag_ScreenOverlay ) || \
parentItem.represents( kmlTag_GroundOverlay )

#endif // KMLElementDefines_h
