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

#include "PlacemarkFilter.h"
#include "TagsFilter.h"
#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "OsmPlacemarkData.h"
#include "GeoDataPlacemark.h"

namespace Marble{

TagsFilter::TagsFilter(GeoDataDocument *document, const QStringList &tagsList, bool andFlag ) : PlacemarkFilter(document)
{
    int total=0, tagCount=0;
	// qDebug()<<"Entered tagFilter";
    QList<GeoDataObject*> previousObjects(m_objects);
	m_objects.clear();
    foreach (GeoDataObject *object, previousObjects) {
        ++total;
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(object);
        bool flag = andFlag;
        QStringList::const_iterator itr = tagsList.begin();
        for (; itr != tagsList.end(); ++itr) {
            QStringList currentTag = (*itr).split('=');
            QString currentKey;
            QString currentValue;
            if (currentTag.size() != 2) {
                qDebug()<< "Invalid tag : "<< currentTag<<" ,rejecting it"<<endl;
                continue;
            } else {
                currentKey = currentTag[0].trimmed();
                currentValue = currentTag[1].trimmed();
            }
            bool contains;
            if (currentValue == "*") {
                contains = placemark->osmData().containsTagKey(currentKey);
            } else {
                contains = placemark->osmData().containsTag(currentKey, currentValue);
			}
            if (!contains) {
                if (andFlag) {
					flag = false;
					break;
				}
            } else {
                if (!andFlag) {
					flag = true;
					break;
				}
			}
		}		
        if (flag) {
            ++tagCount;
			// qDebug()<<"Contained tag";
			m_objects.append(object);
			// qDebug()<<"ID "<<placemark->osmData().id();
        } else {
            m_rejectedObjects.append(object);
		}

	}
	// qDebug()<<"Done TagFiltering";
	// qDebug()<<"Total"<<total;
	// qDebug()<<"TagCount"<<tagcon;
	// qDebug()<<"Is and : "<<andFlag;
}

void TagsFilter::process()
{
	//does nothing
}

}


QList<GeoDataObject*>::const_iterator TagsFilter::rejectedObjectsBegin() const
{
    return m_rejectedObjects.begin();
}

QList<GeoDataObject*>::const_iterator TagsFilter::rejectedObjectsEnd() const
{
    return m_rejectedObjects.end();
}
