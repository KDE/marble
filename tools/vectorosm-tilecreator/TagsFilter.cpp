//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include <QString>
#include <QStringList>

#include "TagsFilter.h"
#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "OsmPlacemarkData.h"
#include "GeoDataPlacemark.h"

namespace Marble {

TagsFilter::TagsFilter(GeoDataDocument *document, const Tags &tagsList, FilterFlag filterFlag)
    : m_accepted(new GeoDataDocument)
{
    for (GeoDataFeature *feature: document->featureList()) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(feature)) {
            bool acceptPlacemark = false;
            auto const & osmData = placemark->osmData();

            if (filterFlag == FilterRailwayService &&
                    osmData.containsTagKey(QStringLiteral("railway")) &&
                    osmData.containsTagKey(QStringLiteral("service"))) {
                acceptPlacemark = false;
            } else {
                for (auto const &tag: tagsList) {
                    bool contains;
                    if (tag.second == QLatin1String("*")) {
                        contains = osmData.containsTagKey(tag.first);
                    } else {
                        contains = osmData.containsTag(tag.first, tag.second);
                    }
                    if (contains) {
                        acceptPlacemark = true;
                        break;
                    }
                }
            }

            if (acceptPlacemark) {
                m_accepted->append(placemark->clone());
            } else {
                m_rejectedObjects.append(placemark->clone());
            }
        }
        else {
            m_accepted->append(feature->clone());
        }
    }
}

TagsFilter::~TagsFilter()
{
    delete m_accepted;
    qDeleteAll(m_rejectedObjects);
}

QVector<GeoDataFeature*>::const_iterator TagsFilter::rejectedObjectsBegin() const
{
    return m_rejectedObjects.begin();
}

QVector<GeoDataFeature*>::const_iterator TagsFilter::rejectedObjectsEnd() const
{
    return m_rejectedObjects.end();
}

GeoDataDocument *TagsFilter::accepted()
{
    return m_accepted;
}

void TagsFilter::removeAnnotationTags(GeoDataDocument *document)
{
    for (auto placemark: document->placemarkList()) {
        auto & osmData = placemark->osmData();
        removeAnnotationTags(osmData);
        for (auto & reference: osmData.nodeReferences()) {
            removeAnnotationTags(reference);
        }
        for (auto & reference: osmData.memberReferences()) {
            removeAnnotationTags(reference);
        }
    }
}

void TagsFilter::removeAnnotationTags(OsmPlacemarkData &osmData)
{
    osmData.removeTag(QLatin1String("comment"));
    osmData.removeTag(QLatin1String("note"));
    osmData.removeTag(QLatin1String("note:de"));
    osmData.removeTag(QLatin1String("fixme"));
    osmData.removeTag(QLatin1String("todo"));
    osmData.removeTag(QLatin1String("source"));
    osmData.removeTag(QLatin1String("source:geometry"));
    osmData.removeTag(QLatin1String("source:name"));
    osmData.removeTag(QLatin1String("source:addr"));
    osmData.removeTag(QLatin1String("source:ref"));
    osmData.removeTag(QLatin1String("source_ref"));
}

}
