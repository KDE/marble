#include "LineStringProcessor.h"

#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "GeoDataLineString.h"

LineStringProcessor::LineStringProcessor(GeoDataDocument* document) :
    PlacemarkFilter(document, GeoDataTypes::GeoDataLineStringType)
{

}

void LineStringProcessor::process()
{
    QList<GeoDataObject*> polylinesToDrop;

    foreach (GeoDataObject* polyline, m_objects) {
        switch(static_cast<GeoDataPlacemark*>(polyline)->visualCategory())
        {
        case GeoDataFeature::HighwayCycleway :
        case GeoDataFeature::HighwayFootway :
        case GeoDataFeature::HighwayLivingStreet :
        case GeoDataFeature::HighwayPath :
        case GeoDataFeature::HighwayPedestrian :
        case GeoDataFeature::HighwayRoad :
        case GeoDataFeature::HighwaySteps :
        case GeoDataFeature::HighwayUnknown :
        case GeoDataFeature::HighwayUnclassified :
            polylinesToDrop.append(polyline);
            break;
        default:
            break;
        }

    }

    int removed = 0;

    foreach (GeoDataObject* polyline, polylinesToDrop) {
        if(m_document->removeOne(static_cast<GeoDataFeature*>(polyline))) {
            ++removed;
        }
    }

    qDebug() << "Polylines dropped: " << removed;
}
