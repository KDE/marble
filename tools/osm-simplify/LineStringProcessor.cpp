//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "LineStringProcessor.h"

#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "GeoDataLineString.h"


LineStringProcessor::LineStringProcessor(GeoDataDocument* document) :
    PlacemarkFilter(document)
{
    QList<GeoDataPlacemark*> toRemove;
    foreach (GeoDataObject* placemark, m_objects) {
        if( static_cast<GeoDataPlacemark*>(placemark)->geometry()->nodeType() != GeoDataTypes::GeoDataLineStringType) {
            toRemove.append(static_cast<GeoDataPlacemark*>(placemark));
        }
    }

    foreach (GeoDataObject* placemark, toRemove) {
        m_objects.removeOne(placemark);
    }
}

void LineStringProcessor::process()
{
    qDebug() << "Polylines to process: " << m_objects.size();

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

    qDebug() << m_document->name();
}

