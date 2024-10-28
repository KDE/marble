// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_TAGSFILTER_H
#define MARBLE_TAGSFILTER_H

#include "OsmPlacemarkData.h"

#include <QMap>
#include <QString>

namespace Marble
{

class GeoDataDocument;
class GeoDataFeature;

class TagsFilter
{
public:
    typedef QPair<QString, QString> Tag;
    using Tags = QList<Tag>;

    enum FilterFlag {
        NoFlag = 0,
        FilterRailwayService
    };

    // Filters placemarks which have tags in the hash
    TagsFilter(GeoDataDocument *document, const Tags &tagsList, FilterFlag filterFlag = NoFlag);
    ~TagsFilter();

    QList<GeoDataFeature *>::const_iterator rejectedObjectsBegin() const;
    QList<GeoDataFeature *>::const_iterator rejectedObjectsEnd() const;

    GeoDataDocument *accepted();

    static void removeAnnotationTags(GeoDataDocument *document);

private:
    static void removeAnnotationTags(OsmPlacemarkData &osmData);

    GeoDataDocument *m_accepted;
    QList<GeoDataFeature *> m_rejectedObjects;
};

}

#endif
