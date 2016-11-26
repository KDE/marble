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

#include "BaseFilter.h"
#include "TagsFilter.h"
#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "OsmPlacemarkData.h"
#include "GeoDataPlacemark.h"

namespace Marble {

TagsFilter::TagsFilter(GeoDataDocument *geoDocument, const Tags &tagsList)
    : BaseFilter(geoDocument),
      m_accepted(new GeoDataDocument)
{
    int total=0, tagCount=0;
    QVector<GeoDataPlacemark*> previousObjects(placemarks());
    foreach (GeoDataPlacemark *placemark, previousObjects) {
        ++total;
        bool flag = false;
        for (auto const &tag: tagsList) {
            bool contains;
            if (tag.second == QLatin1String("*")) {
                contains = placemark->osmData().containsTagKey(tag.first);
            } else {
                contains = placemark->osmData().containsTag(tag.first, tag.second);
            }
            if (contains) {
                flag = true;
                break;
            }
        }
        if (flag) {
            ++tagCount;
            m_accepted->append(new GeoDataPlacemark(*placemark));
        } else {
            m_rejectedObjects.append(new GeoDataPlacemark(*placemark));
        }
    }
}

TagsFilter::~TagsFilter()
{
    delete m_accepted;
    qDeleteAll(m_rejectedObjects);
}

QVector<GeoDataPlacemark*>::const_iterator TagsFilter::rejectedObjectsBegin() const
{
    return m_rejectedObjects.begin();
}

QVector<GeoDataPlacemark*>::const_iterator TagsFilter::rejectedObjectsEnd() const
{
    return m_rejectedObjects.end();
}

GeoDataDocument *TagsFilter::accepted()
{
    return m_accepted;
}

void TagsFilter::removeAnnotationTags(GeoDataDocument *document)
{
    foreach (auto placemark, document->placemarkList()) {
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
