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

#include "PlacemarkFilter.h"
#include <QMap>
#include <QString>


namespace Marble{

class GeoDataDocument;	

class TagsFilter : public PlacemarkFilter
{
public:
	//Filters placemarks which have tags in the hash
	TagsFilter(GeoDataDocument* document, const QStringList& tagsList, bool andFlag = false);
	virtual void process();
    QList<GeoDataObject*>::const_iterator rejectedObjectsBegin() const;
    QList<GeoDataObject*>::const_iterator rejectedObjectsEnd() const;
private:
    QList<GeoDataObject*> m_rejectedObjects;
};

}
#endif
