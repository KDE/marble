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

#include "KmlExtrudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataGeometry.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"

#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( extrude )

GeoNode* KmlextrudeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_extrude ) );

    GeoStackItem parentItem = parser.parentElement();
    
    GeoDataGeometry* geometry;
    bool validParents = false;

    if( parentItem.is<GeoDataPlacemark>() && parentItem.represents( kmlTag_Point ) ) {
        geometry = parentItem.nodeAs<GeoDataPlacemark>()->geometry();
        validParents = true;
    } else if( parentItem.nodeAs<GeoDataGeometry>() ) {
        geometry = parentItem.nodeAs<GeoDataGeometry>();
        validParents = true;
    }

    if( validParents ) {
        QString content = parser.readElementText().trimmed();
        
        if( content == QString( "1" ) ) {
            geometry->setExtrude( true );
        } else {
            geometry->setExtrude( false );
        }
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_extrude << "> containing: " << content
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif
    }

    return 0;
}

}
}
