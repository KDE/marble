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

#include "BaseFilter.h"
#include "OsmPlacemarkData.h"

#include <QMap>
#include <QString>

namespace Marble {

class GeoDataDocument;	

class TagsFilter : public BaseFilter
{
public:
    //Filters placemarks which have tags in the hash
    TagsFilter(GeoDataDocument* document, const QStringList& tagsList, bool andFlag = false);
    ~TagsFilter();

    QVector<GeoDataPlacemark*>::const_iterator rejectedObjectsBegin() const;
    QVector<GeoDataPlacemark*>::const_iterator rejectedObjectsEnd() const;

    GeoDataDocument* accepted();

    static void removeAnnotationTags(GeoDataDocument* document);

private:
    static void removeAnnotationTags(OsmPlacemarkData &osmData);

    GeoDataDocument* m_accepted;
    QVector<GeoDataPlacemark*> m_rejectedObjects;
};

}

#endif
