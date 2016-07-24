//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "BaseFilter.h"

#include "GeoDataDocument.h"
#include "GeoDataGeometry.h"
#include "GeoDataObject.h"
#include "GeoDataTypes.h"

BaseFilter::BaseFilter(GeoDataDocument* document) :
    m_document(document),
    m_placemarks(document->placemarkList())
{
    // nothing to do
}

BaseFilter::~BaseFilter()
{

}

QVector<GeoDataPlacemark*>::const_iterator BaseFilter::objectsBegin() const
{
  return m_placemarks.begin();
}

QVector<GeoDataPlacemark*>::const_iterator BaseFilter::objectsEnd() const
{
  return m_placemarks.end();
}
