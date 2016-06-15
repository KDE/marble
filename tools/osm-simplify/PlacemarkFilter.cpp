#include "PlacemarkFilter.h"

#include "GeoDataPlacemark.h"

PlacemarkFilter::PlacemarkFilter(GeoDataDocument *document, const char *type) :
    BaseFilter(document, GeoDataTypes::GeoDataPlacemarkType)
{
    QList<GeoDataObject*> toRemove;
    foreach (GeoDataObject* placemark, m_objects) {
        if( static_cast<GeoDataPlacemark*>(placemark)->geometry()->nodeType() != type) {
            toRemove.append(placemark);
        }
    }

    foreach (GeoDataObject* placemark, toRemove) {
        m_objects.removeOne(placemark);
    }
}

