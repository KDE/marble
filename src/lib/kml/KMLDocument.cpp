#include "KMLDocument.h"
#include "placecontainer.h"

KMLDocument::KMLDocument()
{
    m_placecontainer = new PlaceContainer();
}

void KMLDocument::addPlaceMark( PlaceMark* placemark )
{
    m_placecontainer->append( placemark );
}

PlaceContainer& KMLDocument::getPlaceContainer() const
{
    return *m_placecontainer;
}
