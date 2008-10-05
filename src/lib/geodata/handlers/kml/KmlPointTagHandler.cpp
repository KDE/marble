/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "KmlPointTagHandler.h"

#include <QtCore/QDebug>

#include "KmlElementDictionary.h"
#include "GeoDataPlacemark.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataParser.h"

namespace Marble
{

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( Point )

GeoNode* KmlPointTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Point ) );
    // FIXME: there needs to be a check that a coordinates subtag is contained

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.nodeAs<GeoDataPlacemark>() ) {
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_Point << "> returning: " << parentItem.nodeAs<GeoDataPlacemark>()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return parentItem.nodeAs<GeoDataPlacemark>();
    } else if( parentItem.nodeAs<GeoDataMultiGeometry>() ) {
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_Point << "> returning: " << parentItem.nodeAs<GeoDataMultiGeometry>()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
        return parentItem.nodeAs<GeoDataMultiGeometry>();
    }
    return 0;
}

}
