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
    QVector<GeoDataPlacemark*> previousObjects(placemarks());
    placemarks().clear();
    foreach (GeoDataPlacemark *placemark, previousObjects) {
        ++total;
        bool flag = andFlag;
        QStringList::const_iterator itr = tagsList.begin();
        for (; itr != tagsList.end(); ++itr) {
            QStringList currentTag = (*itr).split(QLatin1Char('='));
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
            if (currentValue == QLatin1String("*")) {
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
            placemarks().append(placemark);
            // qDebug()<<"ID "<<placemark->osmData().id();
        } else {
            m_rejectedObjects.append(placemark);
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


QVector<GeoDataPlacemark*>::const_iterator TagsFilter::rejectedObjectsBegin() const
{
    return m_rejectedObjects.begin();
}

QVector<GeoDataPlacemark*>::const_iterator TagsFilter::rejectedObjectsEnd() const
{
    return m_rejectedObjects.end();
}
