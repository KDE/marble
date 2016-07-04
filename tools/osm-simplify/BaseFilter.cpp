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

BaseFilter::BaseFilter(GeoDataDocument* document, const char *type) :
    m_document(document)
{
    foreach (GeoDataFeature* feature, m_document->featureList()) {
        if(feature->nodeType() == type) {
            m_objects.append(feature);
        }
    }
}

BaseFilter::~BaseFilter()
{

}
