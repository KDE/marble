//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//


#ifndef MARBLE_TAGSFILTER_H
#define MARBLE_TAGSFILTER_H

#include "OsmPlacemarkData.h"

#include <QMap>
#include <QString>

namespace Marble {

class GeoDataDocument;
class GeoDataFeature;

class TagsFilter
{
public:
    typedef QPair<QString, QString> Tag;
    typedef QVector<Tag> Tags;

    enum FilterFlag {
        NoFlag = 0,
        FilterRailwayService
    };

    //Filters placemarks which have tags in the hash
    TagsFilter(GeoDataDocument* document, const Tags& tagsList, FilterFlag filterFlag = NoFlag);
    ~TagsFilter();

    QVector<GeoDataFeature*>::const_iterator rejectedObjectsBegin() const;
    QVector<GeoDataFeature*>::const_iterator rejectedObjectsEnd() const;

    GeoDataDocument* accepted();

    static void removeAnnotationTags(GeoDataDocument* document);

private:
    static void removeAnnotationTags(OsmPlacemarkData &osmData);

    GeoDataDocument* m_accepted;
    QVector<GeoDataFeature*> m_rejectedObjects;
};

}

#endif
