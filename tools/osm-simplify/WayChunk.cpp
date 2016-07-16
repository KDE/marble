//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include <QList>
#include <QVector>
#include <QDebug>

#include "WayChunk.h"
#include "GeoDataCoordinates.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "OsmPlacemarkData.h"

namespace Marble
{

WayChunk::WayChunk(GeoDataPlacemark *placemark, qint64 first, qint64 last)
{
	m_wayList.append(placemark);
	m_first = first;
	m_last = last;
	m_visualCategory = placemark->visualCategory();
}

WayChunk::~WayChunk()
{
	
}

qint64 WayChunk::first() const
{
	return m_first;
}

qint64 WayChunk::last() const
{
	return m_last;
}

void WayChunk::append(GeoDataPlacemark *placemark, qint64 last)
{
	m_wayList.append(placemark);
	m_last = last;

}

void WayChunk::prepend(GeoDataPlacemark *placemark, qint64 first)
{
	m_wayList.prepend(placemark);
	m_first = first;

}

void WayChunk::append(WayChunk *chunk)
{
    m_wayList << chunk->m_wayList;
    m_last = chunk->last();
}

GeoDataPlacemark* WayChunk::merge()
{
    Q_ASSERT(!m_wayList.isEmpty());

    GeoDataPlacemark *placemark = new GeoDataPlacemark(*(m_wayList.first()));
    GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
    QList<GeoDataPlacemark*>::iterator itr = m_wayList.begin();
    QList<GeoDataPlacemark*>::iterator itrEnd = m_wayList.end();
    ++itr;
    for (; itr != itrEnd; ++itr) {
        GeoDataLineString *currentLine = static_cast<GeoDataLineString*>( (*itr)->geometry() );
        currentLine->remove(0);
        (*line) << *currentLine;
	}
	//qDebug()<<"Merging placemark";
    return placemark;
}

void WayChunk::reverse()
{
	std::reverse(m_wayList.begin(), m_wayList.end());
	QList<GeoDataPlacemark*>::iterator itr = m_wayList.begin();
    for (; itr != m_wayList.end(); ++itr) {
        GeoDataPlacemark *placemark = *itr;
        GeoDataLineString *line = static_cast<GeoDataLineString*>(placemark->geometry());
		line->reverse();
	}
    qSwap(m_first, m_last);
}

qint64 WayChunk::id() const
{
	return m_wayList.first()->osmData().id();
}

void WayChunk::printIds() const
{
	QList<GeoDataPlacemark*>::const_iterator itr = m_wayList.begin();
	qDebug()<<"IDs of placemarks in chunk";
    for (; itr != m_wayList.end(); ++itr) {
		qDebug()<<"Id :- "<<(*itr)->osmData().id();
	}
}

int WayChunk::size() const
{
	return m_wayList.size();
}

bool WayChunk::concatPossible(GeoDataPlacemark *placemark) const
{
	GeoDataFeature::GeoDataVisualCategory category = placemark->visualCategory();
    return (category == m_visualCategory);	
}	

GeoDataFeature::GeoDataVisualCategory WayChunk::visualCategory() const
{
	return m_visualCategory;
}


}
